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

Grab the source with `git clone https://github.com/valeriorlandini/theinformer.git`

`cd theinformer` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

The compiled binaries can be found inside `TheInformer_artefacts/Release` (or simply `TheInformer_artefacts` in Linux) folder.
