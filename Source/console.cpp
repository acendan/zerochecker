/*
  ==============================================================================

    console.cpp
    Created: 21 Aug 2022 4:59:19pm
    Author:  Aaron Cendan
	Description: Generate console output table and write .csv file if applicable

  ==============================================================================
*/

#include "console.h"

using namespace zero;

namespace
{
    constexpr auto s_noNonZeroes{ "N/A" };
    constexpr auto s_sep{ "," };
    constexpr auto s_endl{ "\n" };
	constexpr auto s_fullPathHeader{ "Full Path" };
}

Console::Console(const std::optional<juce::String>& csv /*= std::nullopt*/, int numItems, bool monoAnalysis) :
	m_numItems{ numItems }, m_monoAnalysisMode { monoAnalysis }
{
    // Init table
    m_table.clear();
    m_table.setIndent(1, 1);

	// Init stats
	m_stats.clear();
	m_stats.setIndent(1, 1);

	// Per-platform character encoding for output table (unicode by default)
#if defined (JUCE_WINDOWS)
	samilton::ConsoleTable::TableChars chars;
#else
	samilton::ConsoleTable::TableChars chars{ '+', '+', '+', '+','-', '+', '+', '|', '+', '+', '+' };
#endif
	m_table.setTableChars(chars);
	m_stats.setTableChars(chars);

    // Init CSV
    if (csv.has_value() && !csv->isEmpty())
    {
        m_csvFile = juce::File::createLegalPathName(*csv);
        if (!m_csvFile->hasFileExtension("csv"))
        {
            m_csvFile = m_csvFile->withFileExtension("csv");
        }
        m_csvFile->create();
        m_csvText = juce::String();
    }

    // Table headers
	if (m_monoAnalysisMode)
	{
		append({ "File Name", "Mono Compatibility" });
	}
	else
	{
		append({ "File Name", "First Non-Zero (smpls)", "First Non-Zero (sec)", "Last Non-Zero (smpls)", "Last Non-Zero (sec)" });
	}

	m_startTime = juce::Time::getCurrentTime();
}

void Console::print()
{
	m_endTime = juce::Time::getCurrentTime();

	std::cout << std::endl << R"(                             __              __
 ____  ___  _________  _____/ /_  ___  _____/ /_____  _____
/_  / / _ \/ ___/ __ \/ ___/ __ \/ _ \/ ___/ //_/ _ \/ ___/
 / /_/  __/ /  / /_/ / /__/ / / /  __/ /__/ ,< /  __/ /    
/___/\___/_/   \____/\___/_/ /_/\___/\___/_/|_|\___/_/     
)" << std::endl;
    
    std::cout << m_table << std::endl;

	printStats();

	if (m_csvFile.has_value() && m_csvText.has_value() && m_csvFile->existsAsFile() && !m_csvText->isEmpty())
	{
        m_csvFile->replaceWithText(*m_csvText);
        std::cout << "Output CSV to: " << m_csvFile->getFullPathName() << std::endl;
	}

    std::cout << "\n=========================================================================" << std::endl;
}

void Console::printStats()
{
	m_stats.addRow({"Output Stats", "Value" });
	m_stats.addRow({"Total number of files scanned", std::to_string(m_numItems).c_str() });
	const auto duration{ m_endTime - m_startTime };
	m_stats.addRow({"Total execution time", duration.getApproximateDescription().toRawUTF8() });

	if (m_monoAnalysisMode)
	{
		m_stats.addRow({"Number of mono compatible files", std::to_string(m_numMonoFiles).c_str() });
		m_stats.addRow({"Potential space savings by converting to mono", juce::File::descriptionOfSizeInBytes(m_sizeSavingsBytes).toRawUTF8() });
	}
	else
	{
		//TODO: Add unique stats for default zerochecker mode
	}

	std::cout << m_stats << std::endl;
}

void Console::append(const std::initializer_list<const char*>& row, const juce::String& fullPath)
{
	m_table.addRow(row);

    if (m_csvFile.has_value() && m_csvText.has_value())
    {
		// Prep full path column on new rows
		if (m_csvText->isEmpty())
		{
			m_csvText->append(s_fullPathHeader, strlen(s_fullPathHeader));
			m_csvText->append(s_sep, strlen(s_sep));
		}
		else
		{
			m_csvText->append(s_endl, strlen(s_endl));
			m_csvText->append(fullPath, fullPath.length());
			m_csvText->append(s_sep, strlen(s_sep));
		}
		
		// Print table row
		for (const auto& col : row)
		{
			m_csvText->append(col, strlen(col));
			m_csvText->append(s_sep, strlen(s_sep));
		}
    }
}

void Console::append(const File& file)
{
	if (m_monoAnalysisMode)
	{
		m_numMonoFiles++;
		m_sizeSavingsBytes += file.m_file.getSize() - (file.m_file.getSize() / file.m_numChannels);

		auto monoCompatibility{ std::to_string(file.m_monoCompatibility) };
		monoCompatibility.resize(6);
		append({ file.m_file.getFileName().toStdString().c_str(), monoCompatibility.c_str()}, file.m_file.getFullPathName());
	}
	else
	{
		append({ file.m_file.getFileName().toStdString().c_str(),
			(file.m_firstNonZeroSample >= 0) ? std::to_string(file.m_firstNonZeroSample).c_str() : s_noNonZeroes,
			(file.m_firstNonZeroSample >= 0) ? File::relTimeToString(file.m_firstNonZeroTime).toStdString().c_str() : s_noNonZeroes,
			(file.m_lastNonZeroSample >= 0) ? std::to_string(file.m_lastNonZeroSample).c_str() : s_noNonZeroes,
			(file.m_lastNonZeroSample >= 0) ? File::relTimeToString(file.m_lastNonZeroTime).toStdString().c_str() : s_noNonZeroes }, 
			file.m_file.getFullPathName());
	}
}

void Console::progressBar()
{
	static auto item{ 0 };
	if (m_numItems > 0)
	{
		std::cout << "[";
		m_progress += 1.0f / static_cast<float>(m_numItems);
		int pos = static_cast<int>(m_progressBarWidth * m_progress);
		for (int i = 0; i < m_progressBarWidth; ++i)
		{
			if (i < pos)
			{
				std::cout << "=";
			}
			else if (i == pos)
			{
				std::cout << ">";
			}
			else
			{
				std::cout << " ";
			}
		}
		std::cout << "] " << static_cast<int>(m_progress * 100.0f) << "% (" << ++item << "/" << m_numItems << ")\r";
		std::cout.flush();
	}
}
