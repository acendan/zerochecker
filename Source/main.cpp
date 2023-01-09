/*
  ==============================================================================

	Main.cpp
	Created: 21 Aug 2022 10:05:38am
	Author:  Aaron Cendan

  ==============================================================================
*/

#include "zerochecker.h"

int main(int argc, char* argv[])
{
	try
	{
		zero::Checker zerochecker;
		return zerochecker.run({ argc, argv });
	}
	catch (...)
	{
		return 1;
	}
}
