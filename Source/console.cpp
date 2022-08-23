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

//==============================================================================
// Add all commands and options available from command line, incl help text
Command::Command()
{
	// Parse input files
	m_addFilelist = juce::ConsoleApplication::Command({ "", "<files> <folders>", "Files and/or folders to analyze", "Recursively analyzes all .wav and .flac files in folders.",
		[this](const juce::ArgumentList& args)
		{
			for (const auto& arg : args.arguments)
			{
				juce::File file{ arg.resolveAsFile() };
				if (file.isDirectory())
				{
					for (const auto& child : file.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "*.wav;*.flac"))
					{
						m_zerochecker.m_files.push_back(zero::File(child));
					}
				}
				else if (file.existsAsFile() && file.hasFileExtension("wav;flac"))
				{
					m_zerochecker.m_files.push_back(zero::File(file));
				}
			}
		} });
	addCommand(m_addFilelist);

	// Parse optional csv
	m_genCSV = juce::ConsoleApplication::Command({ "-c|--csv", "-c|--csv <output.csv>", "Specify output .csv filepath", "Generates CSV file from zerochecker output.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_csv = args.getValueForOption("-c|--csv");
		} });
	addCommand(m_genCSV);

	// Magnitude range minimum
	m_magMin = juce::ConsoleApplication::Command({ "-m|--min", "-m|--min <0.003>", "Minimum amplitude considered for non-zeros (0.0 - 1.0)", "If a sample is greater than this value, it will be considered a non-zero. Must be less than maximum (-x|--max)",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_magnitudeRangeMin = args.getValueForOption("-m|--min").getDoubleValue();
		} });
	addCommand(m_magMin);

	// Magnitude range maximum
	m_magMax = juce::ConsoleApplication::Command({ "-x|--max", "-x|--max <1.0>", "Maximum amplitude considered for non-zeros (0.0 - 1.0)", "If a sample is less than this value, it will be considered a non-zero. Must be greater than minimum (-m|--min).",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_magnitudeRangeMax = args.getValueForOption("-x|--max").getDoubleValue();
		} });
	addCommand(m_magMax);

	// Mininimum consecutive samples
	m_consecSmpls = juce::ConsoleApplication::Command({ "-s|--consec", "-s|--consec <0>", "Number of consecutive samples past threshold to be considered a non-zero", "If this is greater than 0, then multiple consecutive samples must exceed threshold for detecting non-zeros.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_minConsecutiveSamples = args.getValueForOption("-s|--consec").getIntValue();
		} });
	addCommand(m_consecSmpls);

	// Sample offset from start or end
	m_smplOffset = juce::ConsoleApplication::Command({ "-o|--offset", "-o|--offset <0>", "Number of samples offset from start/end", "Relative offset from start and end of file before analysis of level.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_sampleOffset = args.getValueForOption("-o|--offset").getIntValue();
		} });
	addCommand(m_smplOffset);

	// Number of samples to search
	m_numSmpls = juce::ConsoleApplication::Command({ "-n|--num", "-n|--num <-1>", "Number of samples to analyze before stopping (-1 = entire file)", "Restricts the number of samples analyzed before stopping.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_numSamplesToSearch = args.getValueForOption("-n|--num").getIntValue();
		} });
	addCommand(m_numSmpls);

	// Help & version
	addHelpCommand("-h|--help", juce::String("ABOUT:\n    zerochecker v") + ProjectInfo::versionString +
		R"( - https://github.com/acendan/zerochecker
    Aaron Cendan 2022 - https://aaroncendan.me | https://ko-fi.com/acendan_

USAGE:
    .\zerochecker.exe [options] <files> <folders> 
    .\zerochecker.exe 'C:\folder\cool_file.wav' 'C:\folder\weird_file.flac'
    .\zerochecker.exe 'C:\folder\subfolder\'
    .\zerochecker.exe -c 'C:\folder\output_log.csv' 'C:\folder\cool_file.wav'
    .\zerochecker.exe --min=0.1 --consec=5 'C:\folder\weird_file.flac'

NOTE:
    Short options like '-m' should have a space, followed by the desired value.
    Long options like '--min' should have an equals sign instead. Refer to USAGE above.

OPTIONS:)", true);
	addVersionCommand("-v|--version", ProjectInfo::versionString);
}

int Command::run(const juce::ArgumentList& args)
{
	// Run help command by default
	if (args.size() == 0)
	{
		return findAndRunCommand(args);
	}
	
	// Parse args
	juce::StringArray filelist{};
	for (const auto& arg : args.arguments)
	{
		if (arg.isOption())
		{
			findAndRunCommand(juce::ArgumentList(args.executableName, juce::StringArray(arg.text, args.getValueForOption(arg.text))));
		}
		else if (!arg.isOption() && !arg.text.containsIgnoreCase("csv"))
		{
			filelist.addIfNotAlreadyThere(arg.text);
		}
	}

	// Process files
	m_addFilelist.command(juce::ArgumentList(args.executableName, filelist));

	// Run zerochecker
	if (!m_zerochecker.m_files.empty())
	{
		m_zerochecker.processFiles();
	}

	return 0;
}

//==============================================================================
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
        m_csvFile = juce::File::createLegalPathName(csv.operator*());
        if (!m_csvFile->hasFileExtension("csv"))
        {
            m_csvFile = m_csvFile->withFileExtension("csv");
        }
        m_csvFile->create();
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

	if (m_csvFile.has_value() && m_csvText.has_value() && m_csvFile->existsAsFile() && !m_csvText->isEmpty())
	{
        m_csvFile->replaceWithText(m_csvText.operator*());
        std::cout << "Output CSV to: " << m_csvFile->getFullPathName() << std::endl;
	}

    std::cout << "\n=========================================================================" << std::endl;
}

void ConsoleTable::append(const std::initializer_list<const char*>& row)
{
	m_table.addRow(row);

    if (m_csvFile.has_value() && m_csvText.has_value())
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
