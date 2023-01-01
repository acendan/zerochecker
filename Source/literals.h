/*
  ==============================================================================

    literals.h
    Created: 31 Dec 2022 7:38:28pm
    Author:  Aaron Cendan
    Description: Hard-coded strings, mostly used for printing to console

  ==============================================================================
*/

#pragma once

namespace zero::ltrl
{
	constexpr auto zerocheckerASCII{ R"(
                             __              __
 ____  ___  _________  _____/ /_  ___  _____/ /_____  _____
/_  / / _ \/ ___/ __ \/ ___/ __ \/ _ \/ ___/ //_/ _ \/ ___/
 / /_/  __/ /  / /_/ / /__/ / / /  __/ /__/ ,< /  __/ /
/___/\___/_/   \____/\___/_/ /_/\___/\___/_/|_|\___/_/

)"
	};

	constexpr auto monocheckerASCII{ R"(
                                  _               _
                                 | |             | |
 _ __ ___   ___  _ __   ___   ___| |__   ___  ___| | _____ _ __
| '_ ` _ \ / _ \| '_ \ / _ \ / __| '_ \ / _ \/ __| |/ / _ \ '__|
| | | | | | (_) | | | | (_) | (__| | | |  __/ (__|   <  __/ |
|_| |_| |_|\___/|_| |_|\___/ \___|_| |_|\___|\___|_|\_\___|_|

)" };

	constexpr auto helpText{ R"( - https://github.com/acendan/zerochecker
    Aaron Cendan 2022 - https://aaroncendan.me | https://ko-fi.com/acendan_

USAGE:
	.\zerochecker.exe [options] <files> <folders>

	# Run zerochecker, default analysis mode. No optional parameters required. Scan two different files.
	.\zerochecker.exe 'C:\folder\cool_file.wav' 'C:\folder\weird_file.flac'

	# Run monochecker, mono compatibility mode [-m]. Scans all audio files in subfolder (recursively).
	.\zerochecker.exe -m 'C:\folder\subfolder\'

	# Run zerochecker, outputting results to a .csv file.
	.\zerochecker.exe -c 'C:\folder\output_log.csv' 'C:\folder\subfolder\'

	# Run zerochecker, various optional parameters set.
	.\zerochecker.exe --min=0.1 --consec=5 'C:\folder\weird_file.flac'

NOTE:
    Short options like '-x' should have a space, followed by the desired value.
    Long options like '--max' should have an equals sign instead. Refer to USAGE above.

OPTIONS:)" };

	constexpr auto sep{ "," };
	constexpr auto endl{ "\n" };
	constexpr auto divider{ "=========================================================================" };
	constexpr auto nil{ "N/A" };

	constexpr auto fullPathHeader{ "Full Path" };
}