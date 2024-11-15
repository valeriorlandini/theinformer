# sonus
A VST3/LV2/AU plugin and standalone software that analyzes the incoming audio and computes a series of audio descriptors, that are sent as OSC (Open Sound Control) messages, so that they can be used in other softwares (e.g. sound visualization or real time audio analysis).
The Informer runs on Windows, Linux and macOS. Pre-build binaries coming soon.

## Compiled binaries

Soon...

## How to build

Grab the source with `git clone https://github.com/valeriorlandini/theinformer.git` 

`cd theinformer` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (you may neeed to adjust the Visual Studio version if you have an older one)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

`cd build` to put yourself into build folder.

Next run `cmake --build . --config Release`
