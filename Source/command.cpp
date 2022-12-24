/*
  ==============================================================================

    command.cpp
    Created: 23 Dec 2022 8:37:21pm
    Author:  Aaron Cendan
	Description: Basic container for a JUCE console command and its corresponding value

  ==============================================================================
*/

#include "command.h"

using namespace zero;

template<typename T>
auto Command<T>::operator<=>(const auto& that)
{
	return this->val <=> that.val;
}