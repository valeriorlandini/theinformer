# Table of contents

- [The Informer](#the-informer)
  - [Amplitude descriptors](#amplitude-descriptors)
  - [Spectral descriptors](#spectral-descriptors)
- [Pre-built binaries](#pre-built-binaries)
- [How to build the plugin/standalone](#how-to-build-the-pluginstandalone)
- [The Case Officer Max for Live device](#the-case-officer-max-for-live-device)
- [Informer C++ library](#informer-c-library)
- [pyinformer: Informer Python bindings](#pyinformer-informer-python-bindings)


# The Informer
A VST3/LV2/AU plugin and standalone software that analyzes incoming audio and computes a range of audio descriptors, which are sent as OSC (Open Sound Control) messages. These can be used in other software, such as for sound visualization or real-time audio analysis.
The Informer is compatible with Windows, Linux, and macOS.

<img width="1229" height="618" alt="The Informer plugin screenshot" src="https://github.com/user-attachments/assets/74425c5e-355f-4e3c-9c11-be8704c5e837" />


Complementing *The Informer*, there is also *The Case Officer*, a Max for Live device that receives data from *The Informer* so that they can be used as modulation sources within Live.
Additionally, there is a C++ header-only library available for integrating the descriptor calculation algorithms into your own software.

Currently, these descriptors have been implemented:

### Amplitude descriptors

* Kurtosis
  
<img src="https://github.com/user-attachments/assets/9558f3d2-0482-4553-ae91-6dd73f5d41c6" />

* Peak level

<img src="https://github.com/user-attachments/assets/25900b04-0133-43de-9cd3-755e0b8b6bd9" />

* Root mean square

<img src="https://github.com/user-attachments/assets/f0d30d24-44b0-40a6-ab1e-621fc8ae3662" />

* Skewness

<img src="https://github.com/user-attachments/assets/b2eae4f0-2180-4134-9574-25739d21a34b" />

* Variance

<img src="https://github.com/user-attachments/assets/245da9b8-1862-4a3b-a2e1-7f9a70315e55" />

* Zero crossing rate

<img src="https://github.com/user-attachments/assets/a035a4b8-d75f-4f81-9a9b-b5578cbb8d14" />


### Spectral descriptors

* Centroid

<img src="https://github.com/user-attachments/assets/ef6b6052-d3d7-4b54-9dbc-59dc886c16e0" />

* Crest factor

<img src="https://github.com/user-attachments/assets/9eaf1945-f163-4a0a-937f-600456a29a77" />

* Decrease

<img src="https://github.com/user-attachments/assets/44119bb1-52c9-496f-814a-0bda7beb6824" />

* Entropy

<img alt="specentropy svg" src="https://github.com/user-attachments/assets/60c90830-0f39-4dc9-a261-383ed2173767" />

* Flatness

<img src="https://github.com/user-attachments/assets/28c46cce-bd0e-4b76-aacc-e469953fede0" />

* Flux

<img src="https://github.com/user-attachments/assets/8c1a983f-d60a-44d7-9607-125ee1b6d99e" />

* Irregularity

<img src="https://github.com/user-attachments/assets/4b2c55e9-bcd9-4aff-b160-7280a3ac07c4" />

* Kurtosis

<img src="https://github.com/user-attachments/assets/27bde64f-4585-4987-80ec-abe1e16eede6" />

* Peak frequency

<img src="https://github.com/user-attachments/assets/199537b9-2b1f-4cf0-88b7-7fc144bec2df" />

* Rolloff (at 85%)

<img src="https://github.com/user-attachments/assets/e17213e7-25e9-4226-97fe-be677563f4d2" />

* Skewness

<img src="https://github.com/user-attachments/assets/cc0c5371-0a1d-4534-a7b7-83faa8a7f5d7" />

* Slope

<img src="https://github.com/user-attachments/assets/c0097ca7-978c-4f05-bcf7-d510418c3d23" />

* Spread

<img src="https://github.com/user-attachments/assets/a5a82bed-d69a-4a71-84b4-b3ce6b558c24" />


Please note: when the _normalize_ parameter is enabled, all descriptors are adjusted to ensure they fall within the [0.0, 1.0] range. While some descriptors naturally adhere to this range or have well-defined boundaries (e.g., those typically limited to [0.0, Nyquist frequency]), others (such as kurtosis, skewness, and slope) are adjusted based on empirical observations. As a result, the normalize option is best suited for artistic purposes where exact precision is not essential, focusing instead on preventing values from falling outside the expected range, rather than for detailed sound analysis.
In addition to the descriptors mentioned above, a simplified spectrum is provided, consisting of a user-specified number of equal-octave bands (ranging from 2 to 16 via the user interface). For each of these bands, the reported value corresponds to the square root of the highest magnitude among all the original frequency bands of the full spectrogram that fall within that equal-octave band.

## Pre-built binaries

Compiled binaries for Linux, Windows and macOS can be found in the [Releases section](https://github.com/valeriorlandini/theinformer/releases).

## How to build the plugin/standalone

Grab the source with `git clone https://github.com/valeriorlandini/theinformer.git --recursive`

`cd theinformer` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

The compiled binaries can be found inside `TheInformer_artefacts/Release` (or simply `TheInformer_artefacts` in Linux) folder.

## _The Case Officer_ Max for Live device

*The Case Officer* is a Max for Live device that receives data from *The Informer* so that they can be used as modulation sources within Live. It runs on Live 12 (Windows/macOS), and can be found in the `Max4Live` folder of this repository.

![image](https://github.com/user-attachments/assets/4571a509-41de-47e1-9bf7-15aedf0520ca)


## _Informer_ C++ library

In `Library` folder, there is `informer.h`, a MIT-licensed C++ header-only library to use the plugin algorithms in any application. The library can be used in two ways: by directly calling the provided functions or by creating an instance of the implemented class and then computing and retrieving the descriptors from there.

The library is simply imported with the inclusion of its header:
```cpp
#include "informer.h"
```

For the first option, there are two namespaces inside the `Informer` namespace: `Amplitude` and `Frequency`. Once you have an iterable container with floating point values (of any type) representing a buffer, you can compute the amplitude descriptors according to the following example:

```cpp
std::vector<double> myBuffer = /* your buffer */

auto rms = Informer::Amplitude::rms(myBuffer);
```

For descriptors like kurtosis and skewness, which uses the mean and the variance, these values can be passed as optional arguments or, if not provided, they are computed by the function.
For frequency descriptors, the functions expect an iterable container with floating point values representing the magnitudes of each bin obtained from the Fourier transform, for example:

```cpp
std::vector<double> fftMag = /* your FFT magnitudes */

auto irregularity = Informer::Frequency::irregularity(fftMag);
```

There is a built-in function to calculate the normalized magnitudes if you have the result of a real valued FFT in the canonical form of `(real[0], real[SR/2], real[1], imag[1], real[2], imag[2], ...)`:

```cpp
// With realFftResult being a container with the result of a real valued FFT
std::vector<double> fftMag = Informer::Frequency::magnitudes(realFftResult);
```

For descriptors needing the sample rate, this can be specified (otherwise it is set to 44100 Hz). When a descriptor uses other descriptors (such as kurtosis and skewness), these can be passed as parameters, otherwise they are computed.
Finally, for descriptors expressed in Hertz, like centroid and spread, the frequencies of the FFT bins can be passed as a vector to speed up the function (that would compute them otherwise). A utility function `precompute_frequencies` is available for this scope:

```cpp
double sampleRate = 44100.0;
unsigned int fftSize = 8192;

auto precomputed_frequencies = Informer::Frequency::precompute_frequencies(fftSize, sampleRate);
```

For class implementation, it can be instantiated by passing a buffer and a series of FFT magnitudes:

```cpp
// Sample rate defaults to 44100.0, rolloff point defaults to 0.85,
// previous FFT magnitudes to 0.0 (with same current FFT magnitudes size)
// Last parameter tells to compute the descriptors immediately
// If buffer or magnitudes are not passed, they default to empty vectors
// and their corresponding descriptors are not computed
// fftSize parameter can be used to specify the FFT size when magnitudes
// are not passed. If fftSize is not passed or set to 0, FFT size is
// (magnitudes size - 1) * 2
Informer::Informer<float> informer(buffer, fftMag, sampleRate, rolloffPoint, previousFftMags, fftSize, true);

// Retrieve descriptors (once they have been computed)
auto peak = informer.get_time_descriptor("peak");
auto centroid = informer.get_frequency_descriptor("centroid"));

// Store new audio values
informer.set_buffer(newBuffer);
informer.set_magnitudes(newMagnitudes);
// If you have the result of a real valued FFT, use this function
// and magnitudes are automatically computed
informer.set_stft(newStft);

// Compute new descriptors
informer.compute_descriptors();
```

## _pyinformer_: _Informer_ Python bindings

In `PyInformer` folder, there is the necessary stuff to create Python bindings to the C++ library, so that you can use all the functions of the library in Python.

To build and install the bindings, inside the main repository folder:

`pip install ./PyInformer`

<details>
<summary>(Expand this section to build only without automatic installation)</summary>

To build the bindings, inside `PyInformer` folder:

* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

You will find a dynamic library file that begins with `pyinformer.cpython`: place inside your Python library folder or inside your Python project folder.

</details>

You can begin to use the library right away with:

```python
import pyinformer
```

There  are two submodules, `pyinformer.amplitude` and `pyinformer.frequency`. Once you have a list of floats or a NumPy 1D array representing a buffer with its samples or the magnitudes of a real FFT, you can do for example:

```python
import numpy as np

# Generate a random buffer and scale it to [-1, 1] range
example_buffer = np.random.rand(4096) * 2.0 - 1.0

zero_crossing_rate = pyinformer.amplitude.zerocrossing(example_buffer)
```

Arguments to be passed to the various functions are the same and respect the same order of the corresponding C++ ones.

The Python library has also a class-based implementantion, mirroring the one provided by the C++ counterpart. For example, provided that you have a buffer and its corresponding FFT magnitudes:

```python
import pyinformer 

descriptors = pyinformer.Informer(sample_rate=sr, stft_size=n_fft)

# Buffer and magnitudes can be also set during class initialization
descriptors.set_buffer(buffer)
descriptors.set_magnitudes(magnitudes)

descriptors.compute_descriptors()
centroid = descriptors.get_frequency_descriptor('centroid')
```
