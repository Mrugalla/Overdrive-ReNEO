#pragma once

namespace audio
{
	/*samples, numSamples*/
	void encodeMS(float* const*, int) noexcept;
	
	/*samples, numSamples*/
	void decodeMS(float* const*, int) noexcept;
}