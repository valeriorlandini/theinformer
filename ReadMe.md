# The Informer
A VST3/LV2/AU plugin and standalone software that analyzes incoming audio and computes a range of audio descriptors, which are sent as OSC (Open Sound Control) messages. These can be used in other software, such as for sound visualization or real-time audio analysis.
The Informer is compatible with Windows, Linux, and macOS.

![immagine](https://github.com/user-attachments/assets/fbb00638-e4ec-4510-88c2-30177856e2fa)

Currently, these descriptors have been implemented:

### Amplitude descriptors

* Kurtosis
* Peak level
* Root mean square
* Variance

### Spectral descriptors

* Centroid
* Crest factor
* Entropy
* Flatness
* Peak frequency
* Rolloff (at 85%)
* Skewness
* Slope
* Spread

*Please note that some of these descriptors may be available in the latest source version only, and not in the latest binary release yet*

Please note: when the _normalize_ parameter is enabled, all descriptors are adjusted to ensure they fall within the [0.0, 1.0] range. While some descriptors naturally adhere to this range or have well-defined boundaries (e.g., those typically limited to [0.0, Nyquist frequency]), others —such as kurtosis, skewness, and slope— are adjusted based on empirical observations. As a result, the normalize option is best suited for artistic purposes where exact precision is not essential, focusing instead on preventing values from falling outside the expected range, rather than for detailed sound analysis.

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
