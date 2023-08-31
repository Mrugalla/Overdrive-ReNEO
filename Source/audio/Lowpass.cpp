#include "Lowpass.h"

namespace audio
{
	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::getXFromFc(Float fc) noexcept
	{
		return std::exp(static_cast<Float>(-Tau) * fc);
	}

	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::getXFromHz(Float hz, Float Fs) noexcept
	{
		return getXFromFc(hz / Fs);
	}

	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::getXFromMs(Float ms, Float Fs) noexcept
	{
		const auto decayInSamples = ms * Fs * static_cast<Float>(.001);
		return getXFromSamples(decayInSamples);
	}

	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::getXFromSamples(Float smpls) noexcept
	{
		return std::exp(static_cast<Float>(-1) / smpls);
	}

	//

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::makeFromDecayInSamples(Float d) noexcept
	{
		setX(std::exp(static_cast<Float>(-1) / d));
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::makeFromDecayInSecs(Float d, Float Fs) noexcept
	{
		makeFromDecayInSamples(d * Fs);
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::makeFromDecayInFc(Float fc) noexcept
	{
		setX(getXFromFc(fc));
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::makeFromDecayInHz(Float hz, Float Fs) noexcept
	{
		setX(getXFromHz(hz, Fs));
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::makeFromDecayInMs(Float d, Float Fs) noexcept
	{
		const auto decayInSamples = d * Fs * static_cast<Float>(.001);
		makeFromDecayInSamples(decayInSamples);
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::copyCutoffFrom(const Lowpass<Float, AutoGain>& other) noexcept
	{
		a0 = other.a0;
		b1 = other.b1;
	}

	template<typename Float, bool AutoGain>
	Lowpass<Float, AutoGain>::Lowpass(const Float _startVal) :
		a0(static_cast<Float>(1)),
		b1(static_cast<Float>(0)),
		y1(_startVal),
		startVal(_startVal)
	{}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::reset()
	{
		a0 = static_cast<Float>(1);
		b1 = static_cast<Float>(0);
		y1 = startVal;
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::operator()(Float* buffer, Float val, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
			buffer[s] = processSample(val);
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::operator()(Float* buffer, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
			buffer[s] = processSample(buffer[s]);
	}

	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::operator()(Float sample) noexcept
	{
		return processSample(sample);
	}

	template<typename Float, bool AutoGain>
	Float Lowpass<Float, AutoGain>::processSample(Float x0) noexcept
	{
		y1 = x0 * a0 + y1 * b1;
		return y1;
	}

	template<typename Float, bool AutoGain>
	void Lowpass<Float, AutoGain>::setX(Float x) noexcept
	{
		const auto one = static_cast<Float>(1);
		a0 = one - x;

		if (AutoGain)
			b1 = x * (one - a0);
		else
			b1 = x;
	}

	template struct Lowpass<float, true>;
	template struct Lowpass<double, true>;
	template struct Lowpass<float, false>;
	template struct Lowpass<double, false>;
}