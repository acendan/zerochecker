/*
  ==============================================================================

    console.h
    Created: 21 Aug 2022 4:59:27pm
    Author:  Aaron Cendan
    Description: Generate console output table and write .csv file if applicable

  ==============================================================================
*/

#pragma once

#include "file.h"
#include "zerochecker.h"

#include <JuceHeader.h>
#include <CppConsoleTable.hpp>

namespace zero
{
	class Console
	{
	public:
		explicit Console(const std::optional<juce::String>& csv = std::nullopt, int numItems = 0,
		                 Checker::AnalysisMode analysisMode = Checker::AnalysisMode::ZERO_CHECKER);

		void print();

		void printStats();

		void append(const std::initializer_list<const char*>& row, const juce::String& fullPath = "");

		void append(const zero::File& file);

		void progressBar();

	private:
		samilton::ConsoleTable m_table{};
		Checker::AnalysisMode m_analysisMode{};

		std::optional<juce::File> m_csvFile{};
		std::optional<juce::String> m_csvText{};

		float m_progress{ 0.0f };
		int m_progressBarWidth{ 70 };
		int m_numItems{ 0 };

		// Analysis stats
		samilton::ConsoleTable m_stats{};
		int m_numMonoFiles{ 0 };
		juce::int64 m_sizeSavingsBytes{ 0 };
		juce::Time m_startTime{};
		juce::Time m_endTime{};
	};
}