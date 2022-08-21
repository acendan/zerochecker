// Aaron Cendan 2022

#include <JuceHeader.h>

//==============================================================================
int main (int argc, char* argv[])
{
	for (auto i{ 1 }; i < argc; ++i)
	{
		juce::String str{ argv[i] };
		std::cout << str << std::endl;
	}

    return 0;
}
