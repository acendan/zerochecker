# zerochecker
A command line utility for analyzing audio files. **zerochecker** currently features two different analysis modes:

1. [zerochecker](https://github.com/acendan/zerochecker/wiki/1.-zerochecker)
> _...allows you to locate the first and last non-zero samples in audio files. This can be used to identify leading/trailing silence or discrepencies across assets that should be tightly synchronized._

2. [mono compatibility checker](https://github.com/acendan/zerochecker/wiki/2.-mono-compatibility-checker)
> _...allows you to scan multichannel files for mono folddown compatibility, determining whether all channels contain the same content. This can be used to dramatically reduce space consumption and more accurately control spatialization of content in games._

Please refer to their respective pages in [the wiki](https://github.com/acendan/zerochecker/wiki) for a thorough breakdown of what they are, 
why you might want to use them, and of course, how to utilize the information they provide in a practical manner.

Aaron Cendan 2022 - [Personal Website](https://aaroncendan.me) | [Buy me a coffee!](https://ko-fi.com/acendan_)

![zerochecker](https://user-images.githubusercontent.com/65690085/186048220-d843ab0d-1e44-40d7-bb7c-a56042da7129.png)

## Usage
In the examples below, it is assumed that `.\zerochecker.exe` refers to the relative directory where you downloaded **zerochecker**.
You will need to reference the fully qualified or relative path accordingly, i.e. `C:\Users\Aaron\Tools\zerochecker.exe`

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
- Long options like '--min' should have an equals sign instead. Refer to **Usage** above for examples.
    
## WIP
- Accept .csv file or text file with filepaths as input
- Generate Reaper batch converter script 
- Complete the wiki... (better explanation of using via command line)
- Open command line if run via double click in Explorer??
- The help/man page could use a section explaining what all the numbers actually mean/
  how you can interpret the data.
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
