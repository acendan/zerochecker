/*
  ==============================================================================

    file.h
    Created: 23 Dec 2022 6:02:09pm
    Author:  Aaron Cendan
    Description: Contains first and last non-zero data for a given audio file

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace zero
{
	struct File
	{
		explicit File(juce::File file);

		juce::File m_file{};
		juce::int64 m_firstNonZeroSample{};
		juce::RelativeTime m_firstNonZeroTime{};
		juce::int64 m_lastNonZeroSample{};
		juce::RelativeTime m_lastNonZeroTime{};
		float m_monoCompatibility{0.0f};
		int m_numChannels{0};
		int m_numSamples{0};

		void calculate(juce::AudioFormatReader *reader, juce::int64 startSampleOffset, juce::int64 numSamplesToSearch,
		               double magnitudeRangeMin, double magnitudeRangeMax, int minConsecutiveSamples);

		void calculateMonoCompatibility(juce::AudioFormatReader *reader, juce::int64 startSampleOffset,
		                                juce::int64 numSamplesToSearch);

		static juce::String relTimeToString(const juce::RelativeTime &t);
	};
}