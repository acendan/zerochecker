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
    -h|--help        Prints the list of commands
    -v|--version     Prints the current version number

    -c|--csv <output.csv>   Specify output .csv filepath
    -m|--mono <0.9>         Mono folddown compatibility checker. Set threshold (0.0 - 1.0) for output, where 1.0 is identical across all channels.
    -o|--offset <0>         Number of samples offset from start/end
    -n|--num <-1>           Number of samples to analyze before stopping (-1 = entire file)
    -s|--consec <0>         Number of consecutive samples past threshold to be considered a non-zero
    -x|--max <1.0>          Maximum amplitude considered for non-zeros (0.0 - 1.0)
    -y|--min <0.003>        Minimum amplitude considered for non-zeros (0.0 - 1.0)
    
## Note
- Short options like '-m' should have a space, followed by the desired value.
- Long options like '--min' should have an equals sign instead. Refer to USAGE above.
    
## Mono Folddown Compatibility Checker (-m|--mono)
This mode allows you to scan multi-channel audio files for mono compatibility. In other words, it checks to see if all of the channels in that file contain the exact same information. Requires zerochecker v0.0.2+.
 
### Why does that matter?
Most importantly, it means that you can easily batch process those multichannel files to just be mono. This saves a significant chunk of memory (~half the size for a stereo file, ~quarter of the size for quad, you get the point). In the case of game development, it also means you could be saving a bit of CPU overhead on decoding and processing multichannel assets. This can also help improve those assets' in-game spatialization, which often relies on time-based delays between channels and can inadvertently create a subtle Haas effect if multichannel assets contain the same content in each channel.
 
### What does the Mono Compatibility output mean?
The compatibility factor in the output log is a scale of 0.0 - 1.0, where 1.0 means there is a perfect sample-accurate match between channels. If you want to just process files that are exactly the same (1.0), then you can go ahead and do that; though I encourage you to take a look at some of the 0.90-0.95 matches, you can probably convert those to mono too.
 
Once you've got a list of files that you want to fold to mono, you can sprinkle some scripting magic to check them all out in your version control platform of choice and overwrite them as mono. Reaper's batch processor is great for overwriting assets as mono. 

![image](https://user-images.githubusercontent.com/65690085/191663408-f142029a-96f6-4eec-a2cd-ba5787cbe071.png)
    
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
