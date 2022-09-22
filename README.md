# zerochecker
A command line utility for locating the first and last non-zero samples in audio files.
This is particularly useful for checking to see if a file begins or ends at a zero-crossing,
which can cause clicks and pops when starting playback or at the bounds of loops. 

Aaron Cendan 2022 - [Personal Website](https://aaroncendan.me) | [Buy me a coffee!](https://ko-fi.com/acendan_)

![zerochecker](https://user-images.githubusercontent.com/65690085/186048220-d843ab0d-1e44-40d7-bb7c-a56042da7129.png)

## Usage
In the examples below, it is assumed that '.\' refers to the relative directory where you downloaded
zerochecker.exe. If you're using the command line from another location, you'll need to reference
the fully qualified or relative path accordingly, i.e. 'C:\Users\Aaron\Tools\zerochecker.exe'

    .\zerochecker.exe [options] <files> <folders>
    
    .\zerochecker.exe 'C:\folder\cool_file.wav' 'C:\folder\weird_file.flac'
    .\zerochecker.exe 'C:\folder\subfolder\'
    .\zerochecker.exe -c 'C:\folder\output_log.csv' 'C:\folder\cool_file.wav'
    .\zerochecker.exe --min=0.1 --consec=5 'C:\folder\weird_file.flac'

## Options
    -h|--help              Prints the list of commands
    -v|--version           Prints the current version number
    
    <files> <folders>      Files and/or folders to analyze. Recursively analyzes all .wav and .flac files in folders.
    -c|--csv <output.csv>  Specify output .csv filepath. Generates CSV file from zerochecker output.
    -m|--min <0.003>       Minimum amplitude considered for non-zeros (0.0 - 1.0)
    -x|--max <1.0>         Maximum amplitude considered for non-zeros (0.0 - 1.0)
    -s|--consec <0>        Number of consecutive samples past threshold to be considered a non-zero
    -o|--offset <0>        Number of samples offset from start/end
    -n|--num <-1>          Number of samples to analyze before stopping (-1 = entire file)
    
## Note
- Short options like '-m' should have a space, followed by the desired value.
- Long options like '--min' should have an equals sign instead. Refer to USAGE above.
    
## WIP
- Accept .csv file or text file with filepaths as input
- The help/man page could use a section explaining what all the numbers actually mean/
  how you can interpret the data.
- Sorting options for table columns would be great
- Currently, zerochecker only analyzes up to the first two channels of input files. 
  Need to look into adding support for multichannel analysis.
- Potentially rewrite zerochecking algorithm using buffer.getMagnitude
- While clicks and pops in looping files are somewhat determined by the offset in timing 
  and amplitude between first and last samples, quantifying the 'clickiness' could include some
  pretty [DSP-heavy math](https://ofai.at/papers/oefai-tr-2006-12.pdf) concerning their 
  spectral content, phase, and complex domain representation. Need to investigate further, as 
  I don't think JUCE currently has an out-of-the-box feature for this.
- Blog post/video/tutorial content!
  
# Special Thanks
- [wwerk](https://github.com/wwerk), for making Mac builds of zerochecker!
- [@nickvonkaenel](https://twitter.com/nickvonkaenel), for the initial idea/request to make zerochecker!
- DenisSamilton, for making [CppConsoleTable](https://github.com/DenisSamilton/CppConsoleTable/)
- [JUCE](https://juce.com/), for trivializing many of the potential nuisances in my life
