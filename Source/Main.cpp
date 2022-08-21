/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

//==============================================================================
int main (int argc, char* argv[])
{

    juce::String str{ "Hello world!" };
	std::cout << str << std::endl;


    return 0;
}
