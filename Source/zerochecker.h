/*
  ==============================================================================

    zerochecker.h
    Created: 21 Aug 2022 10:05:38am
    Author:  Aaron Cendan

  ==============================================================================
*/

#pragma once

#include "file.h"
#include "command.h"

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

		enum class AnalysisMode
		{
			ZERO_CHECKER, MONO_COMPATIBILITY_CHECKER
		};
		AnalysisMode m_analysisMode{ AnalysisMode::ZERO_CHECKER };

		zero::Command<std::vector<File>> m_files{};
		zero::Command<std::optional<juce::String>> m_csv{ std::nullopt };
		zero::Command<juce::int64> m_sampleOffset{ 0 };
		zero::Command<juce::int64> m_numSamplesToSearch{ -1 };
		zero::Command<double> m_magnitudeRangeMin{ 0.003 };
		zero::Command<double> m_magnitudeRangeMax{ 1.0 };
		zero::Command<int> m_minConsecutiveSamples{ 0 };
		zero::Command<double> m_monoAnalysisThreshold{ 0.99 };

	private:
		juce::AudioFormatManager m_formatMngr{};
	};
}
