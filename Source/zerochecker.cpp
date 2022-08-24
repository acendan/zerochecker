/*
  ==============================================================================

    zerochecker.cpp
    Created: 21 Aug 2022 10:05:47am
    Author:  Aaron Cendan

  ==============================================================================
*/

#include "zerochecker.h"
#include "console.h"
#include <utility>

using namespace zero;

namespace
{
	// Scan in reverse from end of file; modification of juce::AudioFormatReader::searchForLevel
	juce::int64 reverseSearchForLevel(juce::AudioFormatReader* reader, 
		juce::int64 endSampleOffset,
		juce::int64 numSamplesToSearch,
		double magnitudeRangeMinimum,
		double magnitudeRangeMaximum,
		int minimumConsecutiveSamples)
	{
		if (numSamplesToSearch == 0)
			return -1;

		const int bufferSize = 4096;
		juce::HeapBlock<int> tempSpace(bufferSize * 2 + 64);

		int* tempBuffer[3] = { tempSpace.get(),
							   tempSpace.get() + bufferSize,
							   nullptr };

		int consecutive = 0;
		juce::int64 firstMatchPos = -1;

		jassert(magnitudeRangeMaximum > magnitudeRangeMinimum);

		auto doubleMin = juce::jlimit(0.0, (double)std::numeric_limits<int>::max(), magnitudeRangeMinimum * std::numeric_limits<int>::max());
		auto doubleMax = juce::jlimit(doubleMin, (double)std::numeric_limits<int>::max(), magnitudeRangeMaximum * std::numeric_limits<int>::max());
		auto intMagnitudeRangeMinimum = juce::roundToInt(doubleMin);
		auto intMagnitudeRangeMaximum = juce::roundToInt(doubleMax);
		
		juce::int64 startSample = reader->lengthInSamples - endSampleOffset;

		while (numSamplesToSearch != 0)
		{
			auto numThisTime = (int)juce::jmin(std::abs(numSamplesToSearch), (juce::int64)bufferSize);
			juce::int64 bufferStart = startSample - bufferSize;

			if (numSamplesToSearch < 0)
				bufferStart -= numThisTime;

			if (bufferStart >= reader->lengthInSamples)
				break;

			reader->read(tempBuffer, 2, bufferStart, numThisTime, false);
			auto num = numThisTime;

			while (--num >= 0)
			{
				if (numSamplesToSearch < 0)
					--startSample;

				bool matches = false;
				auto index = (int)(startSample - bufferStart - 1);

				if (reader->usesFloatingPointData)
				{
					const float sample1 = std::abs(((float*)tempBuffer[0])[index]);

					if (sample1 >= magnitudeRangeMinimum
						&& sample1 <= magnitudeRangeMaximum)
					{
						matches = true;
					}
					else if (reader->numChannels > 1)
					{
						const float sample2 = std::abs(((float*)tempBuffer[1])[index]);

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
						firstMatchPos = startSample;

					if (++consecutive >= minimumConsecutiveSamples)
					{
						if (firstMatchPos < 0 || firstMatchPos > reader->lengthInSamples)
							return -1;

						return reader->lengthInSamples - firstMatchPos;
					}
				}
				else
				{
					consecutive = 0;
					firstMatchPos = -1;
				}

				if (numSamplesToSearch > 0)
					--startSample;
			}

			if (numSamplesToSearch > 0)
				numSamplesToSearch -= numThisTime;
			else
				numSamplesToSearch += numThisTime;
		}

		return -1;
	}
}

//==============================================================================
File::File(juce::File file) : m_file{ std::move(file) } {}

void File::calculate(juce::AudioFormatReader* reader, juce::int64 startSampleOffset, juce::int64 numSamplesToSearch,
	double magnitudeRangeMin, double magnitudeRangeMax, int minConsecutiveSamples)
{
	if (numSamplesToSearch == -1)
	{
		numSamplesToSearch = reader->lengthInSamples;
	}

	m_firstNonZeroSample = reader->searchForLevel(startSampleOffset, numSamplesToSearch, magnitudeRangeMin, magnitudeRangeMax, minConsecutiveSamples);
	m_firstNonZeroTime = juce::RelativeTime(static_cast<double>(m_firstNonZeroSample) / reader->sampleRate);
	m_lastNonZeroSample = reverseSearchForLevel(reader, startSampleOffset, numSamplesToSearch, magnitudeRangeMin, magnitudeRangeMax, minConsecutiveSamples);
	m_lastNonZeroTime = juce::RelativeTime(static_cast<double>(m_lastNonZeroSample) / reader->sampleRate);
}

juce::String File::relTimeToString(const juce::RelativeTime& t)
{
	auto str{ juce::String(t.inSeconds()) };
	auto prd{ str.indexOfChar('.') };

	// #TODO: Expose number of places after decimal
	return ((prd > -1) ? str.substring(0, prd + 4) : str);
}

//==============================================================================
Checker::Checker()
{
	m_formatMngr.registerBasicFormats();
}

void Checker::processFiles()
{
	ConsoleTable console{ m_csv };
	
	for (auto& zeroFile : m_files)
    {
		if (auto* reader = m_formatMngr.createReaderFor(zeroFile.m_file))
        {
			zeroFile.calculate(reader, m_sampleOffset, m_numSamplesToSearch, m_magnitudeRangeMin, m_magnitudeRangeMax, m_minConsecutiveSamples);
			console.append(zeroFile);

			delete reader;
        }
    }

	console.print();
}
