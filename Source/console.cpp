/*
  ==============================================================================

    console.cpp
    Created: 21 Aug 2022 4:59:19pm
    Author:  Aaron Cendan

  ==============================================================================
*/

#include "console.h"

using namespace zero;

namespace
{
    constexpr auto s_noNonZeroes{ "N/A" };
    constexpr auto s_sep{ "," };
    constexpr auto s_endl{ "\n" };
}

ConsoleTable::ConsoleTable(const std::optional<juce::String>& csv /*= std::nullopt*/)
{
    // Init table
    m_table.clear();
    m_table.setIndent(1, 1);
	samilton::ConsoleTable::TableChars chars;
	m_table.setTableChars(chars);

    // Init CSV
    if (csv.has_value() && !csv->isEmpty())
    {
        m_csv = juce::File::createLegalPathName(csv.value());
        if (!m_csv->hasFileExtension("csv"))
        {
            m_csv = m_csv->withFileExtension("csv");
        }
        m_csv->create();
        m_csvText = juce::String();
    }

    // Table headers
    append({ "File Name", "First Non-Zero (smpls)", "First Non-Zero (sec)", "Last Non-Zero (smpls)", "Last Non-Zero (sec)" });
}

void ConsoleTable::print()
{
    std::cout << R"(                             __              __            
 ____  ___  _________  _____/ /_  ___  _____/ /_____  _____
/_  / / _ \/ ___/ __ \/ ___/ __ \/ _ \/ ___/ //_/ _ \/ ___/
 / /_/  __/ /  / /_/ / /__/ / / /  __/ /__/ ,< /  __/ /    
/___/\___/_/   \____/\___/_/ /_/\___/\___/_/|_|\___/_/     
)" << std::endl;
    
    std::cout << m_table << std::endl;

	if (m_csv.has_value() && m_csvText.has_value() && m_csv->existsAsFile() && !m_csvText->isEmpty())
	{
        m_csv->replaceWithText(m_csvText.value());
        std::cout << "Output CSV to: " << m_csv->getFullPathName() << std::endl;
	}

    std::cout << R"(
zerochecker - Aaron Cendan 2022 - https://github.com/acendan/zerochecker
=========================================================================
)" << std::endl;
}

void ConsoleTable::append(const std::initializer_list<const char*>& row)
{
	m_table.addRow(row);

    if (m_csv.has_value() && m_csvText.has_value())
    {
		if (!m_csvText->isEmpty())
		{
			m_csvText->append(s_endl, strlen(s_endl));
		}
		for (const auto& col : row)
		{
			m_csvText->append(col, strlen(col));
			m_csvText->append(s_sep, strlen(s_sep));
		}
    }
}

void ConsoleTable::append(const File& file)
{
	append({ file.m_file.getFileName().toStdString().c_str(),
		(file.m_firstNonZeroSample >= 0) ? juce::String(file.m_firstNonZeroSample).toStdString().c_str() : s_noNonZeroes,
		(file.m_firstNonZeroSample >= 0) ? File::relTimeToString(file.m_firstNonZeroTime).toStdString().c_str() : s_noNonZeroes,
		(file.m_lastNonZeroSample >= 0) ? juce::String(file.m_lastNonZeroSample).toStdString().c_str() : s_noNonZeroes,
		(file.m_lastNonZeroSample >= 0) ? File::relTimeToString(file.m_lastNonZeroTime).toStdString().c_str() : s_noNonZeroes });
}
