/*
  ==============================================================================

    zerochecker.cpp
    Created: 21 Aug 2022 10:05:47am
    Author:  Aaron Cendan

  ==============================================================================
*/

#include "zerochecker.h"
#include "console.h"
#include "literals.h"

#include <execution>

using namespace zero;

Checker::Checker()
{
	m_formatMngr.registerBasicFormats();

	// Parse input files
	m_files.cmd = juce::ConsoleApplication::Command(
			{ "", "<files> <folders>", "Files and/or folders to analyze",
			  "Recursively analyzes all .wav and .flac files in folders.",
			  [this](const juce::ArgumentList& args)
			  {
				  for (const auto& arg : args.arguments)
				  {
					  juce::File file{ arg.resolveAsFile() };
					  if (file.isDirectory())
					  {
						  for (const auto& child : file.findChildFiles(
								  juce::File::TypesOfFileToFind::findFiles, true,
								  "*.wav;*.flac"))
						  {
							  m_files.val.emplace_back(child);
						  }
					  }
					  else if (file.existsAsFile() &&
					           file.hasFileExtension("wav;flac"))
					  {
						  m_files.val.emplace_back(file);
					  }
				  }
			  }});
	addCommand(m_files.cmd);

	// Mono analysis mode
	m_monoAnalysisThreshold.cmd = juce::ConsoleApplication::Command(
			{ "-m|--mono", "-m|--mono <0.9>",
			  "monochecker. Similarity threshold (0.0 - 1.0), where 1.0 is identical across all channels.",
			  "Overrides zerochecking to scan channels for mono compatibility.",
			  [this](const juce::ArgumentList& args)
			  {
				  m_analysisMode = AnalysisMode::MONO_COMPATIBILITY_CHECKER;
				  m_monoAnalysisThreshold.val = std::clamp(
						  args.getValueForOption("-m|--mono").getDoubleValue(), 0.0, 1.0);
			  }});
	addCommand(m_monoAnalysisThreshold.cmd);

	// Parse optional csv
	m_csv.cmd = juce::ConsoleApplication::Command(
			{ "-c|--csv", "-c|--csv <output.csv>", "Specify output .csv filepath",
			  "Generates CSV file from zerochecker output.",
			  [this](const juce::ArgumentList& args)
			  {
				  m_csv.val = args.getValueForOption("-c|--csv");
			  }});
	addCommand(m_csv.cmd);

	// Sample offset from start or end
	m_sampleOffset.cmd = juce::ConsoleApplication::Command(
			{ "-o|--offset", "-o|--offset <0>", "Number of samples offset from start/end",
			  "Relative offset from start and end of file before analysis of level.",
			  [this](const juce::ArgumentList& args)
			  {
				  m_sampleOffset.val = args.getValueForOption("-o|--offset").getIntValue();
			  }});
	addCommand(m_sampleOffset.cmd);

	// Number of samples to search
	m_numSamplesToSearch.cmd = juce::ConsoleApplication::Command(
			{ "-n|--num", "-n|--num <-1>", "Number of samples to analyze before stopping (-1 = entire file)",
			  "Restricts the number of samples analyzed before stopping.",
			  [this](const juce::ArgumentList& args)
			  {
				  m_numSamplesToSearch.val = args.getValueForOption("-n|--num").getIntValue();
			  }});
	addCommand(m_numSamplesToSearch.cmd);

	// Mininimum consecutive samples
	m_minConsecutiveSamples.cmd = juce::ConsoleApplication::Command(
			{ "-s|--consec", "-s|--consec <0>",
			  "Number of consecutive samples past threshold to be considered a non-zero",
			  "If this is greater than 0, then multiple consecutive samples must exceed threshold for detecting non-zeros.",
			  [this](const juce::ArgumentList& args)
			  {
				  m_minConsecutiveSamples.val = args.getValueForOption(
						  "-s|--consec").getIntValue();
			  }});
	addCommand(m_minConsecutiveSamples.cmd);

	// Magnitude range maximum
	m_magnitudeRangeMax.cmd = juce::ConsoleApplication::Command(
			{ "-x|--max", "-x|--max <1.0>", "Maximum amplitude considered for non-zeros (0.0 - 1.0)",
			  "If a sample is less than this value, it will be considered a non-zero. Must be greater than minimum (-y|--min).",
			  [this](const juce::ArgumentList& args)
			  {
				  m_magnitudeRangeMax.val = std::clamp(args.getValueForOption("-x|--max").getDoubleValue(), 0.0, 1.0);
			  }});
	addCommand(m_magnitudeRangeMax.cmd);

	// Magnitude range minimum
	m_magnitudeRangeMin.cmd = juce::ConsoleApplication::Command(
			{ "-y|--min", "-y|--min <0.003>", "Minimum amplitude considered for non-zeros (0.0 - 1.0)",
			  "If a sample is greater than this value, it will be considered a non-zero. Must be less than maximum (-x|--max)",
			  [this](const juce::ArgumentList& args)
			  {
				  m_magnitudeRangeMin.val = std::clamp(args.getValueForOption("-y|--min").getDoubleValue(), 0.0, 1.0);
			  }});
	addCommand(m_magnitudeRangeMin.cmd);

	// Help & version
	addHelpCommand("-h|--help", juce::String("ABOUT:\n    zerochecker v") + ProjectInfo::versionString +
	                            ltrl::helpText, true);
	addVersionCommand("-v|--version", ProjectInfo::versionString);
}

int Checker::run(const juce::ArgumentList& args)
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
			findAndRunCommand(juce::ArgumentList(args.executableName,
			                                     juce::StringArray(arg.text, args.getValueForOption(arg.text))));
		}
		else if (!arg.isOption() && !arg.text.containsIgnoreCase("csv"))
		{
			filelist.addIfNotAlreadyThere(arg.text);
		}
	}

	// Fetch input files
	m_files.cmd.command(juce::ArgumentList(args.executableName, filelist));

	// Run zerochecker
	if (!m_files.val.empty())
	{
		processFiles();
	}

	return 0;
}

void Checker::processFiles()
{
	Console console{ m_csv.val, static_cast<int>(m_files.val.size()), m_analysisMode };

	std::mutex m;

	auto updateProgress = [&m, &console]()
	{
		std::lock_guard<std::mutex> guard(m);
		console.progressBar();
	};

	auto appendZeroFile = [&m, &console](const File& zeroFile)
	{
		std::lock_guard<std::mutex> guard(m);
		console.append(zeroFile);
	};

	auto monoAnalyze = [&](File& zeroFile)
	{
		updateProgress();
		if (auto* reader = m_formatMngr.createReaderFor(zeroFile.m_file))
		{
			zeroFile.calculateMonoCompatibility(reader, m_sampleOffset.val, m_numSamplesToSearch.val);

			// Only append files above threshold
			if (zeroFile.m_monoCompatibility > m_monoAnalysisThreshold.val)
			{
				appendZeroFile(zeroFile);
			}

			delete reader;
		}
	};

	auto zeroCheck = [&](File& zeroFile)
	{
		updateProgress();
		if (auto* reader = m_formatMngr.createReaderFor(zeroFile.m_file))
		{
			zeroFile.calculate(reader, m_sampleOffset.val, m_numSamplesToSearch.val, m_magnitudeRangeMin.val,
			                   m_magnitudeRangeMax.val, m_minConsecutiveSamples.val);
			appendZeroFile(zeroFile);

			delete reader;
		}
	};

	// Execute in parallel
	switch (m_analysisMode)
	{
	case AnalysisMode::ZERO_CHECKER:
		std::for_each(std::execution::par_unseq, m_files.val.begin(), m_files.val.end(), zeroCheck);
		break;
	case AnalysisMode::MONO_COMPATIBILITY_CHECKER:
		std::for_each(std::execution::par_unseq, m_files.val.begin(), m_files.val.end(), monoAnalyze);
		break;
	}

	console.print();
}