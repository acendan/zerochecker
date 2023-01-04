/*
  ==============================================================================

    command.h
    Created: 23 Dec 2022 8:37:31pm
    Author:  Aaron Cendan
	Description: Basic container for a JUCE console command and its corresponding value

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace zero
{
	template<typename T>
	struct Command
	{
		Command() = default;

		explicit Command(T t) : val{ std::move(t) } { }

		auto operator<=>(const Command& that);

		juce::ConsoleApplication::Command cmd{};
		T val{};
	};
}
