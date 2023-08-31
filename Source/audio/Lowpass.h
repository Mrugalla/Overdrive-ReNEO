#pragma once
#include <cmath>

namespace audio
{
	/* Float-Type, AutoGain */
	template<typename Float, bool AutoGain>
	struct Lowpass
	{
		static constexpr Float Pi = static_cast<Float>(3.141592653589793238462643383279502884197169399375105820974944592307816406286);
		static constexpr Float Tau = static_cast<Float>(2) * Pi;

		/* decay */
		static Float getXFromFc(Float) noexcept;
		/* decay, Fs */
		static Float getXFromHz(Float, Float) noexcept;
		/* decay, Fs */
		static Float getXFromMs(Float, Float) noexcept;
		/* decay */
		static Float getXFromSamples(Float) noexcept;

		/* decay */
		void makeFromDecayInSamples(Float) noexcept;
		/* decay, Fs */
		void makeFromDecayInSecs(Float, Float) noexcept;
		/* fc */
		void makeFromDecayInFc(Float) noexcept;
		/* decay, Fs */
		void makeFromDecayInHz(Float, Float) noexcept;
		/* decay, Fs */
		void makeFromDecayInMs(Float, Float) noexcept;

		void copyCutoffFrom(const Lowpass<Float, AutoGain>&) noexcept;

		/* startVal, autogain */
		Lowpass(const Float = static_cast<Float>(0));

		void reset();

		/* buffer, val, numSamples */
		void operator()(Float*, Float, int) noexcept;
		/* buffer, numSamples */
		void operator()(Float*, int) noexcept;
		/* val */
		Float operator()(Float) noexcept;

		void setX(Float) noexcept;

		Float a0, b1, y1, startVal;

		Float processSample(Float) noexcept;
	};

	using LowpassF = Lowpass<float, false>;
	using LowpassD = Lowpass<double, false>;
	using LowpassFGain = Lowpass<float, true>;
	using LowpassDGain = Lowpass<double, true>;
}