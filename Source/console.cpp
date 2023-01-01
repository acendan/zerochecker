/*
  ==============================================================================

    console.cpp
    Created: 21 Aug 2022 4:59:19pm
    Author:  Aaron Cendan
	Description: Generate console output table and write .csv file if applicable

  ==============================================================================
*/

#include "console.h"
#include "literals.h"

using namespace zero;

Console::Console(const std::optional<juce::String>& csv /*= std::nullopt*/, int numItems,
                 Checker::AnalysisMode analysisMode) :
		m_numItems{ numItems }, m_analysisMode{ analysisMode }
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
	samilton::ConsoleTable::TableChars chars{ '+', '+', '+', '+', '-', '+', '+', '|', '+', '+', '+' };
#endif
	m_table.setTableChars(chars);
	m_stats.setTableChars(chars);

	m_startTime = juce::Time::getCurrentTime();

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
	switch (m_analysisMode)
	{
	case Checker::AnalysisMode::ZERO_CHECKER:
	{
		append({ "File Name", "First Non-Zero (smpls)", "First Non-Zero (sec)", "Last Non-Zero (smpls)",
		         "Last Non-Zero (sec)" });
		break;
	}
	case Checker::AnalysisMode::MONO_COMPATIBILITY_CHECKER:
	{
		append({ "File Name", "Mono Compatibility" });
		break;
	}
	}
}

void Console::print()
{
	m_endTime = juce::Time::getCurrentTime();

	switch (m_analysisMode)
	{
	case Checker::AnalysisMode::ZERO_CHECKER:
	{
		std::cout << ltrl::zerocheckerASCII;
		break;
	}
	case Checker::AnalysisMode::MONO_COMPATIBILITY_CHECKER:
	{
		std::cout << ltrl::monocheckerASCII;
		break;
	}
	}

	std::cout << m_table << ltrl::endl;

	printStats();

	if (m_csvFile.has_value() && m_csvText.has_value() && m_csvFile->existsAsFile() && !m_csvText->isEmpty())
	{
		m_csvFile->replaceWithText(*m_csvText);
		std::cout << "Output CSV to: " << m_csvFile->getFullPathName() << ltrl::endl;
	}

	std::cout << ltrl::endl << ltrl::divider << ltrl::endl;
}

void Console::printStats()
{
	m_stats.addRow({ "Output Stats", "Value" });
	m_stats.addRow({ "Total number of files scanned", std::to_string(m_numItems).c_str() });
	const auto duration{ m_endTime - m_startTime };
	m_stats.addRow({ "Total execution time", duration.getApproximateDescription().toRawUTF8() });

	switch (m_analysisMode)
	{
	case Checker::AnalysisMode::ZERO_CHECKER:
	{
		//TODO: Add unique stats for default zerochecker mode
		break;
	}
	case Checker::AnalysisMode::MONO_COMPATIBILITY_CHECKER:
	{
		m_stats.addRow({ "Number of mono compatible files", std::to_string(m_numMonoFiles).c_str() });
		m_stats.addRow({ "Potential space savings by converting to mono",
		                 juce::File::descriptionOfSizeInBytes(m_sizeSavingsBytes).toRawUTF8() });
		break;
	}
	}

	std::cout << m_stats << ltrl::endl;
}

void Console::append(const std::initializer_list<const char*>& row, const juce::String& fullPath)
{
	m_table.addRow(row);

	if (m_csvFile.has_value() && m_csvText.has_value())
	{
		// Prep full path column on new rows
		if (m_csvText->isEmpty())
		{
			m_csvText->append(ltrl::fullPathHeader, strlen(ltrl::fullPathHeader));
			m_csvText->append(ltrl::sep, strlen(ltrl::sep));
		}
		else
		{
			m_csvText->append(ltrl::endl, strlen(ltrl::endl));
			m_csvText->append(fullPath, fullPath.length());
			m_csvText->append(ltrl::sep, strlen(ltrl::sep));
		}

		// Print table row
		for (const auto& col : row)
		{
			m_csvText->append(col, strlen(col));
			m_csvText->append(ltrl::sep, strlen(ltrl::sep));
		}
	}
}

void Console::append(const File& file)
{
	switch (m_analysisMode)
	{
	case Checker::AnalysisMode::ZERO_CHECKER:
	{
		append({ file.m_file.getFileName().toStdString().c_str(),
		         (file.m_firstNonZeroSample >= 0) ? std::to_string(file.m_firstNonZeroSample).c_str() : ltrl::nil,
		         (file.m_firstNonZeroSample >= 0) ? File::relTimeToString(file.m_firstNonZeroTime).toStdString().c_str()
		                                          : ltrl::nil,
		         (file.m_lastNonZeroSample >= 0) ? std::to_string(file.m_lastNonZeroSample).c_str() : ltrl::nil,
		         (file.m_lastNonZeroSample >= 0) ? File::relTimeToString(file.m_lastNonZeroTime).toStdString().c_str()
		                                         : ltrl::nil },
		       file.m_file.getFullPathName());
		break;
	}
	case Checker::AnalysisMode::MONO_COMPATIBILITY_CHECKER:
	{
		m_numMonoFiles++;
		m_sizeSavingsBytes += file.m_file.getSize() - (file.m_file.getSize() / file.m_numChannels);

		auto monoCompatibility{ std::to_string(file.m_monoCompatibility) };
		monoCompatibility.resize(6);
		append({ file.m_file.getFileName().toStdString().c_str(), monoCompatibility.c_str() },
		       file.m_file.getFullPathName());
		break;
	}
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
