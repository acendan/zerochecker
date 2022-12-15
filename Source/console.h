/*
  ==============================================================================

    console.h
    Created: 21 Aug 2022 4:59:27pm
    Author:  Aaron Cendan

  ==============================================================================
*/

#pragma once

#include "zerochecker.h"
#include <CppConsoleTable.hpp>

namespace zero
{
    //==============================================================================
    // Parse command line input and provide --help details
    class Command : public juce::ConsoleApplication
    {
    public:
        Command();
        int run(const juce::ArgumentList& args);

    private:
        Checker m_zerochecker{};

        juce::ConsoleApplication::Command m_addFilelist{};
        juce::ConsoleApplication::Command m_genCSV{};
        juce::ConsoleApplication::Command m_monoAnalysis{};
		juce::ConsoleApplication::Command m_magMin{};
		juce::ConsoleApplication::Command m_magMax{};
		juce::ConsoleApplication::Command m_consecSmpls{};
        juce::ConsoleApplication::Command m_smplOffset{};
        juce::ConsoleApplication::Command m_numSmpls{};
    };

    //==============================================================================
    // Generate console output table and write .csv file if applicable
    class ConsoleTable
    {
    public:
        explicit ConsoleTable(const std::optional<juce::String>& csv = std::nullopt, int numItems = 0, bool monoAnalysis = false);

        void print();
		void printStats();
        void append(const std::initializer_list<const char*>& row, const juce::String& fullPath = "");
        void append(const zero::File& file);
        void progressBar();

    private:
        samilton::ConsoleTable m_table{};

        std::optional<juce::File> m_csvFile{};
        std::optional<juce::String> m_csvText{};

        float m_progress{ 0.0f };
        int m_progressBarWidth{ 70 };
        int m_numItems{ 0 };

        bool m_monoAnalysisMode{ false };

		samilton::ConsoleTable m_stats{};
		int m_numMonoFiles{ 0 };
		juce::int64 m_sizeSavingsBytes{ 0 };

		juce::Time m_startTime{};
		juce::Time m_endTime{};
    };
}