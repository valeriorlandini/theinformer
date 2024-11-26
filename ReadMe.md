# The Informer
A VST3/LV2/AU plugin and standalone software that analyzes incoming audio and computes a range of audio descriptors, which are sent as OSC (Open Sound Control) messages. These can be used in other software, such as for sound visualization or real-time audio analysis.
The Informer is compatible with Windows, Linux, and macOS. Pre-built binaries will be available soon.

![immagine](https://github.com/user-attachments/assets/fbb00638-e4ec-4510-88c2-30177856e2fa)

## Pre-built binaries

Compiled binaries for Linux, Windows and macOS can be found in the [Releases section](https://github.com/valeriorlandini/theinformer/releases).

## How to build

Grab the source with `git clone https://github.com/valeriorlandini/theinformer.git`

`cd theinformer` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

The compiled binaries can be found inside `TheInformer_artefacts/Release` (or simply `TheInformer_artefacts` in Linux) folder.
