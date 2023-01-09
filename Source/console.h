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
	class Checker;

	class Console
	{
	public:
		Console(Checker& checker, const std::optional<juce::String>& csv = std::nullopt, int numItems = 0);

		void print();

		void printStats();
		void printCsv();

		void promptProcess();
		static bool promptContinue(std::string_view question);

		void append(const std::initializer_list<const char*>& row, const juce::String& fullPath = "");

		void append(const zero::File& file);

		void progressBar(std::optional<int> resetNumItems = std::nullopt);

	private:
		Checker& m_checker;

		samilton::ConsoleTable m_table{};
		samilton::ConsoleTable m_stats{};

		std::optional<juce::File> m_csvFile{};
		std::optional<juce::String> m_csvText{};

		float m_progress{ 0.0f };
		int m_progressBarWidth{ 70 };
		int m_numItems{ 0 };

		juce::Time m_startTime{};
		juce::Time m_endTime{};
	};
}