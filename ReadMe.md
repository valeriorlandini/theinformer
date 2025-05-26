# The Informer
A VST3/LV2/AU plugin and standalone software that analyzes incoming audio and computes a range of audio descriptors, which are sent as OSC (Open Sound Control) messages. These can be used in other software, such as for sound visualization or real-time audio analysis.
The Informer is compatible with Windows, Linux, and macOS.

![image](https://github.com/user-attachments/assets/65eaa577-4e5b-4e60-a69f-e085281d5c74)

Complementing The Informer, there is also *The Case Officer*, a Max for Live device that receives data from The Informer so that they can be used as modulation sources within Live.
Additionally, there is a C++ header-only library available for integrating the descriptor calculation algorithms into your own software.

Currently, these descriptors have been implemented:

### Amplitude descriptors

* Kurtosis

$$ K_n = \frac{\frac{1}{N} \sum_{n=1}^{N} (x(n) - \mu)^4}{\left(\sqrt{\frac{1}{N} \sum_{n=1}^{N} (x(n) - \mu)^2} \right)^4} - 3 $$

* Peak level

$$ P_n = \max_{1 \leq n \leq N}\{\left|x(n)\right|\} $$

* Root mean square

$$ RMS_n = \sqrt{\frac{\sum_{n=1}^{N} x(n)^2}{N}} $$

* Skewness

$$ S_n = \frac{\frac{1}{N}\sum_{n=1}^{N}(x - \mu)^3}{\left(\sqrt{\frac{1}{N} \sum_{n=1}^{N} (x(n) - \mu)^2} \right)^3} $$

* Variance

$$ \sigma_n = (\frac{1}{N} \sum_{n=1}^{N} (x[n] - \mu)^2) $$

* Zero crossing rate

$$ ZCR_n = \frac{1}{N-1} \sum_{n=2}^{N}|\mathrm{sgn}(x(n))-\mathrm{sgn}(x(n-1))| $$

### Spectral descriptors

* Centroid

$$ SC_n = \frac{\sum_{k=0}^{K/2-1} f(k) \cdot |X(k,n)|}{\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Crest factor

$$ CF_n = \frac{\max_{0 \leq k \leq K/2-1} \{ |X(k,n)| \}}{\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Decrease

$$ D_n = \frac{\sum_{k=1}^{K/2-1}  \frac{|X(k,n)| - |X(0,n)|}{k}}{\sum_{k=1}^{K/2-1}|X(k,n)|} $$

* Entropy

$$ H_n = - \frac{\sum_{k=0}^{K/2-1}  \frac{|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2} log_2\left(\frac{|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2}\right)}{log_2 (K/2)} $$

* Flatness

$$ FL_n = \frac{\exp\Big(\frac{\sum_{k=0}^{K/2-1} ln(| X(k,n) |)}{K/2}\Big)}{\frac{\sum_{k=0}^{K/2-1} | X(k,n) |}{K/2} } $$

* Flux

$$ SF_n = \frac{\sqrt{\sum_{k=0}^{K/2-1} \Big( | X(k,n) | - | X(k,n-1) | \Big)^2
}}{K/2}$$

* Irregularity

$$ SI_n = \frac{\sum_{k=1}^{K/2-1} | X(k,n) - X(k-1,n) |}{\sum_{k=0}^{K/2-1} | X(k,n) |} $$

* Kurtosis

$$ SK_n = \frac{\sum_{k=0}^{K/2-1}(f(k) - SC_n)^4 |X(k,n)|}{\left(\sqrt{\frac{\sum_{k=0}^{K/2-1} (f(k) - SC_n)^2|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2}}\right)^4\sum_{k=0}^{K/2-1} |X(k,n)|} - 3 $$

* Peak frequency

$$ PK_n = \frac{{argmax}_k \{ |X(k,n)| \} f_s}{K}  $$

* Rolloff (at 85%)

$$ R_n = i \text{  such that  } \sum_{k=0}^{i} | X(k,n) | = 0.85 \sum_{k=0}^{K/2-1} | X(k,n) | $$

* Skewness

$$ SS_n = \frac{\sum_{k=0}^{K/2-1}(f(k) - SC_n)^3 |X(k,n)|}{\left(\sqrt{\frac{\sum_{k=0}^{K/2-1} (f(k) - SC_n)^2|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2}}\right)^3\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Slope

$$ SL_n = \frac{\sum_{k=0}^{K/2-1}(f(k) - \mu_f)(|X(k,n)| - \frac{\sum_{k=0}^{K/2-1}|X(k,n)|}{K/2})}{\sum_{k=0}^{K/2-1} (f(k) - \mu_f)^2} $$

* Spread

$$ SP_n = \sqrt{\frac{\sum_{k=0}^{K/2-1} (f(k) - SC_n)^2|X(k,n)|}{\sum_{k=0}^{K/2-1}|X(k,n)|}} $$

Please note: when the _normalize_ parameter is enabled, all descriptors are adjusted to ensure they fall within the [0.0, 1.0] range. While some descriptors naturally adhere to this range or have well-defined boundaries (e.g., those typically limited to [0.0, Nyquist frequency]), others —such as kurtosis, skewness, and slope— are adjusted based on empirical observations. As a result, the normalize option is best suited for artistic purposes where exact precision is not essential, focusing instead on preventing values from falling outside the expected range, rather than for detailed sound analysis.

## Pre-built binaries

Compiled binaries for Linux, Windows and macOS can be found in the [Releases section](https://github.com/valeriorlandini/theinformer/releases).

## How to build

Grab the source with `git clone https://github.com/valeriorlandini/theinformer.git --recursive`

`cd theinformer` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

The compiled binaries can be found inside `TheInformer_artefacts/Release` (or simply `TheInformer_artefacts` in Linux) folder.

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