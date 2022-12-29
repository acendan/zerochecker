# zerochecker
A command line utility for analyzing audio files. **zerochecker** currently features two different modes, or ways of examining audio files:

1. [zerochecker (default)](https://github.com/acendan/zerochecker/wiki/1.-zerochecker)
> _...allows you to locate the first and last non-zero samples in audio files. This can be used to identify leading/trailing silence, the likelihood of clicking sounds in looping files, or alignment differences between assets that should be tightly synchronized._

2. [mono compatibility checker](https://github.com/acendan/zerochecker/wiki/2.-mono-compatibility-checker)
> _...allows you to scan multichannel files for mono folddown compatibility, determining whether all channels contain the same content. This can be used to dramatically reduce space consumption and more accurately control spatialization of content in games._

**Please [visit the wiki](https://github.com/acendan/zerochecker/wiki)** for setup instructions, a breakdown of each mode, and of course, how to utilize the results in a practical manner!

***

Aaron Cendan 2022 - [Personal Website](https://aaroncendan.me) | [Buy me a coffee!](https://ko-fi.com/acendan_)

![zerochecker](https://user-images.githubusercontent.com/65690085/186048220-d843ab0d-1e44-40d7-bb7c-a56042da7129.png)

## WIP
- [ ] Accept text file with filepaths as input
- [ ] Generate Reaper batch converter script 
- [x] Complete the wiki... (better explanation of using via command line)
- [ ] Open command line if run via double click in Explorer??
- [ ] The help/man page could use a section explaining what all the numbers actually mean/
  how you can interpret the data.
- [ ] While clicks and pops in looping files are somewhat determined by the offset in timing 
  and amplitude between first and last samples, quantifying the 'clickiness' could include some
  pretty [DSP-heavy math](https://ofai.at/papers/oefai-tr-2006-12.pdf) concerning their 
  spectral content, phase, and complex domain representation. Need to investigate further, as 
  I don't think JUCE currently has an out-of-the-box feature for this.
- [ ] Blog post/video/tutorial content!
  
# Special Thanks
- [wwerk](https://github.com/wwerk), for making Mac builds of zerochecker!
- [@nickvonkaenel](https://twitter.com/nickvonkaenel), for the initial idea/request to make zerochecker!
- DenisSamilton, for making [CppConsoleTable](https://github.com/DenisSamilton/CppConsoleTable/)
- [JUCE](https://juce.com/), for trivializing many of the potential nuisances in my life
