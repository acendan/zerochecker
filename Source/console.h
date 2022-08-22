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
    class ConsoleTable
    {
    public:
        ConsoleTable(const std::optional<juce::String>& csv = std::nullopt);

        void print();
        void append(const std::initializer_list<const char*>& row);
        void append(const zero::File& file);

    private:
        samilton::ConsoleTable m_table{};

        std::optional<juce::File> m_csv{};
        std::optional<juce::String> m_csvText{};
    };
}