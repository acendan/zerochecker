/*
  ==============================================================================

	Main.cpp
	Created: 21 Aug 2022 10:05:38am
	Author:  Aaron Cendan

  ==============================================================================
*/

#include "zerochecker.h"

//==============================================================================
int main (int argc, char* argv[])
{
	// Process input file args
	std::vector<zero::File> files;
	auto pushValidatedAudioFile = [&files](const juce::File& file)
	{
		if (file.existsAsFile() && file.hasFileExtension("wav;flac"))
		{
			files.push_back(zero::File(file));
		}
	};

	// Process input file args
	for (auto i{ 1 }; i < argc; ++i)
	{
		juce::File file{ argv[i] };
		if (file.isDirectory())
		{
			for (const auto& dirFile : file.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "*.wav;*.flac"))
			{
				pushValidatedAudioFile(dirFile);
			}
		}
		else
		{
			pushValidatedAudioFile(file);
		}
	}

	// Run zerochecker
	zero::Checker zerochecker{ std::move(files) };

    return 0;
}
