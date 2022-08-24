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
        explicit ConsoleTable(const std::optional<juce::String>& csv = std::nullopt);

        void print();
        void append(const std::initializer_list<const char*>& row);
        void append(const zero::File& file);

    private:
        samilton::ConsoleTable m_table{};

        std::optional<juce::File> m_csvFile{};
        std::optional<juce::String> m_csvText{};
    };
}