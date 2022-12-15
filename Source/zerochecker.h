/*
  ==============================================================================

    zerochecker.h
    Created: 21 Aug 2022 10:05:38am
    Author:  Aaron Cendan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <optional>

namespace zero 
{
    //==============================================================================
    // Contains first and last non-zero data for a given audio file
    struct File
    {
	    explicit File(juce::File file);

		juce::File m_file{};
		juce::int64 m_firstNonZeroSample{};
        juce::RelativeTime m_firstNonZeroTime{};
        juce::int64 m_lastNonZeroSample{};
        juce::RelativeTime m_lastNonZeroTime{};
		float m_monoCompatibility{ 0.0f };
		int m_numChannels{ 0 };
		int m_numSamples{ 0 };

        void calculate(juce::AudioFormatReader* reader, juce::int64 startSampleOffset, juce::int64 numSamplesToSearch, 
			double magnitudeRangeMin, double magnitudeRangeMax, int minConsecutiveSamples);

        void calculateMonoCompatibility(juce::AudioFormatReader* reader, juce::int64 startSampleOffset, juce::int64 numSamplesToSearch);
        
        static juce::String relTimeToString(const juce::RelativeTime& t);
    };

    //==============================================================================
    // Manager for processing zero files
    class Checker
    {
    public:
        Checker();
		
        void processFiles();

		// Public options set via zero::Command
        std::vector<File> m_files{};
		std::optional<juce::String> m_csv{ std::nullopt };
		juce::int64 m_sampleOffset{ 0 };
		juce::int64 m_numSamplesToSearch{ -1 };
		bool m_monoAnalysisMode{ false };
		double m_monoAnalysisThreshold{ 0.99 };
		double m_magnitudeRangeMin{ 0.003 };
		double m_magnitudeRangeMax{ 1.0 };
		int m_minConsecutiveSamples{ 0 };

    private:
        juce::AudioFormatManager m_formatMngr{};
        std::unique_ptr<juce::AudioFormatReaderSource> m_readerSrc{};
    };
}
