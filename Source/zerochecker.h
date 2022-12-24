/*
  ==============================================================================

    zerochecker.h
    Created: 21 Aug 2022 10:05:38am
    Author:  Aaron Cendan

  ==============================================================================
*/

#pragma once

#include "file.h"

#include <JuceHeader.h>
#include <optional>

namespace zero 
{
    class Checker : public juce::ConsoleApplication
    {
    public:
        Checker();
	    int run(const juce::ArgumentList& args);
        void processFiles();

	    // Container for console command and its corresponding value
	    template<typename T>
	    struct CmdValue
	    {
			CmdValue() = default;
			explicit CmdValue(T t) : val{ std::move(t) } {}
		    auto operator<=>(const auto& that);

		    juce::ConsoleApplication::Command cmd{};
		    T val{};
	    };

		CmdValue<std::vector<File>> m_files{};
		CmdValue<std::optional<juce::String>> m_csv{ std::nullopt };
		CmdValue<juce::int64> m_sampleOffset{ 0 };
		CmdValue<juce::int64> m_numSamplesToSearch{ -1 };
		CmdValue<double> m_magnitudeRangeMin{ 0.003 };
		CmdValue<double> m_magnitudeRangeMax{ 1.0 };
		CmdValue<int> m_minConsecutiveSamples{ 0 };
		CmdValue<bool> m_monoAnalysisMode{ false };
		double m_monoAnalysisThreshold{ 0.99 };

    private:
        juce::AudioFormatManager m_formatMngr{};
    };
}
