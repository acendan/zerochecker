/*
  ==============================================================================

    zerochecker.h
    Created: 21 Aug 2022 10:05:38am
    Author:  Aaron Cendan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace zero 
{
    //==============================================================================
    struct File
    {
        juce::File m_file;

		juce::int64 m_firstNonZeroSample{};
        juce::RelativeTime m_firstNonZeroTime{};
        juce::int64 m_lastNonZeroSample{};
        juce::RelativeTime m_lastNonZeroTime{};

        void calculate(juce::AudioFormatReader* reader, juce::int64 startSampleOffset = 0, juce::int64 numSamplesToSearch = -1, 
            double magnitudeRangeMin = 0.005, double magnitudeRangeMax = 1.0, int minConsecutiveSamples = 0);
        
        juce::String toString() const;
        static juce::String relTimeToString(const juce::RelativeTime& t);
    };

    //==============================================================================
    class Checker
    {
    public:
        Checker(std::vector<File>&& files);

        void processFiles();
    
    private:
        std::vector<File> m_files;

        juce::AudioFormatManager m_formatMngr;
        std::unique_ptr<juce::AudioFormatReaderSource> m_readerSrc;
    };
}