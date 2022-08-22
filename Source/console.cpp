/*
  ==============================================================================

    console.cpp
    Created: 21 Aug 2022 4:59:19pm
    Author:  Aaron Cendan

  ==============================================================================
*/

#include "console.h"

using namespace zero;

Console::Console()
{
    // Init table with headers
    m_table.clear();
    m_table.addRow({ "File Name", "First Non-Zero (smpls)", "First Non-Zero (sec)", "Last Non-Zero (smpls)", "Last Non-Zero (sec)" });

    //m_table.setAlignment(samilton::Alignment::centre);
    m_table.setIndent(1, 1);

	samilton::ConsoleTable::TableChars chars;
	//chars.centreSeparation = '+';
	m_table.setTableChars(chars);
}

void Console::print()
{
    std::cout << m_table << std::endl;
}

void Console::appendZeroFile(const File& file)
{
    m_table.addRow({ file.m_file.getFileName().toStdString().c_str(), 
        juce::String(file.m_firstNonZeroSample).toStdString().c_str(),
        File::relTimeToString(file.m_firstNonZeroTime).toStdString().c_str(),
        juce::String(file.m_lastNonZeroSample).toStdString().c_str(),
        File::relTimeToString(file.m_lastNonZeroTime).toStdString().c_str() });
}
