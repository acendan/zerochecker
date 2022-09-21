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
	constexpr auto s_fullPathHeader{ "Full Path" };
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
						m_zerochecker.m_files.emplace_back(child);
					}
				}
				else if (file.existsAsFile() && file.hasFileExtension("wav;flac"))
				{
					m_zerochecker.m_files.emplace_back(zero::File(file));
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

	// Mono analysis mode
	m_monoAnalysis = juce::ConsoleApplication::Command({ "-m|--mono", "-m|--mono <0.9>", "Mono folddown compatibility checker. Set threshold (0.0 - 1.0) for output, where 1.0 is identical across all channels.", "Overrides zerochecking to scan channels for mono folddown compatibility.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_monoAnalysisThreshold = std::clamp(args.getValueForOption("-m|--mono").getDoubleValue(), 0.0, 1.0);
			m_zerochecker.m_monoAnalysisMode = true;
		} });
	addCommand(m_monoAnalysis);

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

	// Mininimum consecutive samples
	m_consecSmpls = juce::ConsoleApplication::Command({ "-s|--consec", "-s|--consec <0>", "Number of consecutive samples past threshold to be considered a non-zero", "If this is greater than 0, then multiple consecutive samples must exceed threshold for detecting non-zeros.",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_minConsecutiveSamples = args.getValueForOption("-s|--consec").getIntValue();
		} });
	addCommand(m_consecSmpls);

	// Magnitude range maximum
	m_magMax = juce::ConsoleApplication::Command({ "-x|--max", "-x|--max <1.0>", "Maximum amplitude considered for non-zeros (0.0 - 1.0)", "If a sample is less than this value, it will be considered a non-zero. Must be greater than minimum (-y|--min).",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_magnitudeRangeMax = std::clamp(args.getValueForOption("-x|--max").getDoubleValue(), 0.0, 1.0);
		} });
	addCommand(m_magMax);

	// Magnitude range minimum
	m_magMin = juce::ConsoleApplication::Command({ "-y|--min", "-y|--min <0.003>", "Minimum amplitude considered for non-zeros (0.0 - 1.0)", "If a sample is greater than this value, it will be considered a non-zero. Must be less than maximum (-x|--max)",
		[this](const juce::ArgumentList& args)
		{
			m_zerochecker.m_magnitudeRangeMin = std::clamp(args.getValueForOption("-y|--min").getDoubleValue(), 0.0, 1.0);
		} });
	addCommand(m_magMin);

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
    Short options like '-x' should have a space, followed by the desired value.
    Long options like '--max' should have an equals sign instead. Refer to USAGE above.

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
ConsoleTable::ConsoleTable(const std::optional<juce::String>& csv /*= std::nullopt*/, int numItems, bool monoAnalysis) : 
	m_numItems{ numItems }, m_monoAnalysisMode { monoAnalysis }
{
    // Init table
    m_table.clear();
    m_table.setIndent(1, 1);

	// Per-platform character encoding for output table (unicode by default)
#if defined (JUCE_WINDOWS)
	samilton::ConsoleTable::TableChars chars;
#else
	samilton::ConsoleTable::TableChars chars{ '+', '+', '+', '+','-', '+', '+', '|', '+', '+', '+' };
#endif
	m_table.setTableChars(chars);

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
}

void ConsoleTable::print()
{
	std::cout << std::endl << R"(                             __              __            
 ____  ___  _________  _____/ /_  ___  _____/ /_____  _____
/_  / / _ \/ ___/ __ \/ ___/ __ \/ _ \/ ___/ //_/ _ \/ ___/
 / /_/  __/ /  / /_/ / /__/ / / /  __/ /__/ ,< /  __/ /    
/___/\___/_/   \____/\___/_/ /_/\___/\___/_/|_|\___/_/     
)" << std::endl;
    
    std::cout << m_table << std::endl;

	if (m_csvFile.has_value() && m_csvText.has_value() && m_csvFile->existsAsFile() && !m_csvText->isEmpty())
	{
        m_csvFile->replaceWithText(*m_csvText);
        std::cout << "Output CSV to: " << m_csvFile->getFullPathName() << std::endl;
	}

    std::cout << "\n=========================================================================" << std::endl;
}

void ConsoleTable::append(const std::initializer_list<const char*>& row, const juce::String& fullPath)
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

void ConsoleTable::append(const File& file)
{
	if (m_monoAnalysisMode)
	{
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

void ConsoleTable::progressBar()
{
	static auto item{ 0 };
	if (m_numItems > 0)
	{
		std::cout << "[";
		m_progress += 1.0f / m_numItems;
		int pos = m_progressBarWidth * static_cast<int>(m_progress);
		for (int i = 0; i < m_progressBarWidth; ++i)
		{
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << static_cast<int>(m_progress * 100.0f) << "% (" << ++item << "/" << m_numItems << ")\r";
		std::cout.flush();
	}
}
