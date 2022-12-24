/*
  ==============================================================================

    file.cpp
    Created: 23 Dec 2022 6:02:19pm
    Author:  Aaron Cendan
    Description: Contains first and last non-zero data for a given audio file

  ==============================================================================
*/

#include "file.h"

using namespace zero;

namespace
{
	constexpr auto s_epsilon{ 0.0005f };

	enum class SearchDirection
	{
		FORWARD, REVERSE
	};

	// Scan in either direction from start or end of file; modification of juce::AudioFormatReader::searchForLevel
	juce::int64 searchForLevel(juce::AudioFormatReader* reader,
	                           SearchDirection searchDirection,
	                           juce::int64 startSampleOffset,
	                           juce::int64 numSamplesToSearch,
	                           double magnitudeRangeMinimum,
	                           double magnitudeRangeMaximum,
	                           int minimumConsecutiveSamples)
	{
		if (numSamplesToSearch == 0)
		{
			return -1;
		}

		const int bufferSize = 4096;
		juce::HeapBlock<int> tempSpace(bufferSize * 2 + 64);

		int* tempBuffer[3] = { tempSpace.get(),
		                       tempSpace.get() + bufferSize,
		                       nullptr };

		int consecutive = 0;
		juce::int64 firstMatchPos = -1;

		jassert (magnitudeRangeMaximum > magnitudeRangeMinimum);

		auto doubleMin = juce::jlimit(0.0, (double) std::numeric_limits<int>::max(),
		                              magnitudeRangeMinimum * std::numeric_limits<int>::max());
		auto doubleMax = juce::jlimit(doubleMin, (double) std::numeric_limits<int>::max(),
		                              magnitudeRangeMaximum * std::numeric_limits<int>::max());
		auto intMagnitudeRangeMinimum = juce::roundToInt(doubleMin);
		auto intMagnitudeRangeMaximum = juce::roundToInt(doubleMax);

		juce::int64 startSample = (searchDirection == SearchDirection::FORWARD) ?
		                          startSampleOffset : reader->lengthInSamples - startSampleOffset;

		while (numSamplesToSearch != 0)
		{
			auto numThisTime = (int) juce::jmin(std::abs(numSamplesToSearch), (juce::int64) bufferSize);
			juce::int64 bufferStart = (searchDirection == SearchDirection::FORWARD) ?
			                          startSample : startSample - bufferSize;

			if (numSamplesToSearch < 0)
			{
				bufferStart -= numThisTime;
			}

			if (bufferStart >= reader->lengthInSamples)
			{
				break;
			}

			reader->read(tempBuffer, 2, bufferStart, numThisTime, false);
			auto num = numThisTime;

			while (--num >= 0)
			{
				if (numSamplesToSearch < 0)
				{
					--startSample;
				}

				bool matches = false;
				auto index = static_cast<int>((searchDirection == SearchDirection::FORWARD) ?
				                              (startSample - bufferStart) : (startSample - bufferStart - 1));

				if (reader->usesFloatingPointData)
				{
					const float sample1 = std::abs(((float*) tempBuffer[0])[index]);

					if (sample1 >= magnitudeRangeMinimum
					    && sample1 <= magnitudeRangeMaximum)
					{
						matches = true;
					}
					else if (reader->numChannels > 1)
					{
						const float sample2 = std::abs(((float*) tempBuffer[1])[index]);

						matches = (sample2 >= magnitudeRangeMinimum
						           && sample2 <= magnitudeRangeMaximum);
					}
				}
				else
				{
					const int sample1 = std::abs(tempBuffer[0][index]);

					if (sample1 >= intMagnitudeRangeMinimum
					    && sample1 <= intMagnitudeRangeMaximum)
					{
						matches = true;
					}
					else if (reader->numChannels > 1)
					{
						const int sample2 = std::abs(tempBuffer[1][index]);

						matches = (sample2 >= intMagnitudeRangeMinimum
						           && sample2 <= intMagnitudeRangeMaximum);
					}
				}

				if (matches)
				{
					if (firstMatchPos < 0)
					{
						firstMatchPos = startSample;
					}

					if (++consecutive >= minimumConsecutiveSamples)
					{
						if (firstMatchPos < 0 || firstMatchPos > reader->lengthInSamples)
						{
							return -1;
						}

						return (searchDirection == SearchDirection::FORWARD) ?
						       firstMatchPos : reader->lengthInSamples - firstMatchPos;
					}
				}
				else
				{
					consecutive = 0;
					firstMatchPos = -1;
				}

				if (numSamplesToSearch > 0)
				{
					(searchDirection == SearchDirection::FORWARD) ? ++startSample : --startSample;
				}
			}

			if (numSamplesToSearch > 0)
			{
				numSamplesToSearch -= numThisTime;
			}
			else
			{
				numSamplesToSearch += numThisTime;
			}
		}

		return -1;
	}
}

//==============================================================================
File::File(juce::File file) : m_file{ std::move(file) } { }

void File::calculate(juce::AudioFormatReader* reader, juce::int64 startSampleOffset, juce::int64 numSamplesToSearch,
                     double magnitudeRangeMin, double magnitudeRangeMax, int minConsecutiveSamples)
{
	if (numSamplesToSearch == -1)
	{
		numSamplesToSearch = reader->lengthInSamples;
	}

	m_firstNonZeroSample = searchForLevel(reader, SearchDirection::FORWARD, startSampleOffset, numSamplesToSearch,
	                                      magnitudeRangeMin, magnitudeRangeMax, minConsecutiveSamples);
	m_firstNonZeroTime = juce::RelativeTime(static_cast<double>(m_firstNonZeroSample) / reader->sampleRate);
	m_lastNonZeroSample = searchForLevel(reader, SearchDirection::REVERSE, startSampleOffset, numSamplesToSearch,
	                                     magnitudeRangeMin, magnitudeRangeMax, minConsecutiveSamples);
	m_lastNonZeroTime = juce::RelativeTime(static_cast<double>(m_lastNonZeroSample) / reader->sampleRate);
}

juce::String File::relTimeToString(const juce::RelativeTime& t)
{
	auto str{ juce::String(t.inSeconds()) };
	auto prd{ str.indexOfChar('.') };

	// #TODO: Expose number of places after decimal
	return ((prd > -1) ? str.substring(0, prd + 4) : str);
}

void zero::File::calculateMonoCompatibility(juce::AudioFormatReader* reader, juce::int64 startSampleOffset,
                                            juce::int64 numSamplesToSearch)
{
	m_numChannels = static_cast<int>(reader->numChannels);
	m_numSamples = static_cast<int>((numSamplesToSearch > 0) ? numSamplesToSearch : reader->lengthInSamples);
	const auto compatibilityIncr{ 1.0f / static_cast<float>(m_numSamples) };

	if (m_numChannels == 1)
	{
		m_monoCompatibility = -1.0f;
		return;
	}

	juce::AudioSampleBuffer buffer{ m_numChannels, m_numSamples };
	reader->read(&buffer, 0, m_numSamples, startSampleOffset, true, true);

	std::vector<float> sampleBuffer;
	for (auto sample{ 0 }; sample < m_numSamples; ++sample)
	{
		sampleBuffer.clear();
		sampleBuffer.reserve(m_numChannels);

		for (auto channel{ 0 }; channel < m_numChannels; ++channel)
		{
			auto channelData{ buffer.getReadPointer(channel) };
			sampleBuffer.emplace_back(channelData[sample]);
		}

		// Check if all channels are equal to the first element
		auto equalsFirstSample = [&sampleBuffer](float f)
		{
			return std::abs(f - sampleBuffer[0]) < s_epsilon;
		};

		if (std::all_of(sampleBuffer.begin() + 1, sampleBuffer.end(), equalsFirstSample))
		{
			m_monoCompatibility += compatibilityIncr;
		}
	}

	m_monoCompatibility = std::min(m_monoCompatibility, 1.0f);
}