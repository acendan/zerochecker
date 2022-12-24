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
	// #TODO: Thoroughly document all methods and functions

	zero::Checker zerochecker;
	return zerochecker.run({ argc, argv });
}
