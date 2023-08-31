#pragma once
#include "AudioUtils.h"
#include "Lowpass.h"

namespace audio
{
	struct Gate
	{
		static constexpr float MinDb = -400.f;
        static constexpr float RiseMs = 1.f;
        static constexpr float FallMs = 80.f;
        static constexpr float RatioDb = 80.f;

        struct LevelDetector
        {
            LevelDetector() :
                buffer(),
                lp(MinDb),
                sampleRate(1.f),
                atkX(0.f),
                rlsX(0.f),
                env(MinDb)
            {
            }

            void prepare(float _sampleRate, int blockSize) noexcept
            {
                sampleRate = _sampleRate;
                atkX = lp.getXFromMs(RiseMs, sampleRate);
                rlsX = lp.getXFromMs(FallMs, sampleRate);
                buffer.resize(blockSize);
            }

            void operator()(const float* smpls, int numSamples) noexcept
            {
                for (auto s = 0; s < numSamples; ++s)
                    buffer[s] = synthesize(smpls[s]);
            }

            const float* data() const noexcept
            {
                return buffer.data();
            }

        protected:
            std::vector<float> buffer;
            LowpassF lp;
            float sampleRate, atkX, rlsX, env;

            float synthesize(float smpl) noexcept
            {
                const auto rect = std::abs(smpl);
                const auto rectDb = Decibels::gainToDecibels(rect);

                if (env < rectDb)
                    lp.setX(atkX);
                else
                    lp.setX(rlsX);

                env = lp(rectDb);

                return env;
            }
        };

        inline float downwardsExpander(float x, float t, float r) noexcept
        {
            // https://www.desmos.com/calculator/9hezo4krbt
            if (x > t)
                return x;
            else
                return t + r * (x - t);
        }

        inline float computeGainDb(float transferCurve, float level) noexcept
        {
            return transferCurve - level;
        }

        inline float computeGainDbDownwardsExpander(float level, float threshold, float ratio) noexcept
        {
            const auto tc = downwardsExpander(level, threshold, ratio);
            return computeGainDb(tc, level);
        }

		Gate() :
			visualizerValues{ 0.f, 0.f },
			lvlDetectors(),
			wetSmoothMaxDbs{ 0.f, 0.f }
		{}

		void prepare(float fs, int blockSize)
		{
			for(auto& lvl : lvlDetectors)
				lvl.prepare(fs, blockSize);
		}

		void operator()(float* const* samples, int numChannels, int numSamples,
			float thresholdDb) noexcept
		{
			for (auto ch = 0; ch < numChannels; ++ch)
			{
				auto smpls = samples[ch];
				auto& lvlDetector = lvlDetectors[ch];
				lvlDetector(smpls, numSamples);
				const auto levelBuf = lvlDetector.data();

				auto& wetSmoothMaxDb = wetSmoothMaxDbs[ch];
				wetSmoothMaxDb = MinDb;

				for (auto s = 0; s < numSamples; ++s)
				{
					const auto dry = smpls[s];
					const auto lvl = levelBuf[s];
                    const auto gainDownwardsExpanderDb = computeGainDbDownwardsExpander(lvl, thresholdDb, RatioDb);
                    wetSmoothMaxDb = std::max(wetSmoothMaxDb, gainDownwardsExpanderDb);
                    const auto wet = dry * Decibels::decibelsToGain(gainDownwardsExpanderDb, MinDb);
                    smpls[s] = wet;
				}

				visualizerValues[ch] = Decibels::decibelsToGain(wetSmoothMaxDb, MinDb);
			}
		}

		void processDisabled() noexcept
		{
			for(auto& v: visualizerValues)
				v = 0.f;
		}

		std::array<std::atomic<float>, 2> visualizerValues;
	protected:
		std::array<LevelDetector, 2> lvlDetectors;
		std::array<float, 2> wetSmoothMaxDbs;
	};
}