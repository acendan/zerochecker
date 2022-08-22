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
    class Console
    {
    public:
        Console();

        void print();
        void appendZeroFile(const zero::File& file);

    private:
        samilton::ConsoleTable m_table{};
    };
}