# The Informer
A VST3/LV2/AU plugin and standalone software that analyzes incoming audio and computes a range of audio descriptors, which are sent as OSC (Open Sound Control) messages. These can be used in other software, such as for sound visualization or real-time audio analysis.
The Informer is compatible with Windows, Linux, and macOS.

![image](https://github.com/user-attachments/assets/65eaa577-4e5b-4e60-a69f-e085281d5c74)

Currently, these descriptors have been implemented:

### Amplitude descriptors

* Kurtosis

$$ K_n = \frac{\frac{1}{N} \sum_{n=1}^{N} (x[n] - \mu)^4}{\left(\frac{1}{N} \sum_{n=1}^{N} (x[n] - \mu)^2 \right)^2} - 3 $$

* Peak level

$$ P_n = \max_{1 \leq n \leq N}\{\left|x(n)\right|\} $$

* Root mean square

$$ RMS_n = \sqrt{\frac{\sum_{n=1}^{N} x(n)^2}{N}} $$

* Variance

$$ \sigma_n = (\frac{1}{N} \sum_{n=1}^{N} (x[n] - \mu)^2) $$

### Spectral descriptors

* Centroid

$$ SC_n = \frac{\sum_{k=0}^{K/2-1} f(k) \cdot |X(k,n)|}{\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Crest factor

$$ CF_n = \frac{\max_{0 \leq k \leq K/2-1} \{ |X(k,n)| \}}{\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Entropy

$$ H_n = - \frac{\sum_{k=0}^{K/2-1}  \frac{|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2} ln(\frac{|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2})}{ln (K/2)} $$

* Flatness

$$ FL_n = \frac{\exp\Big(\frac{\sum_{k=0}^{K/2-1} ln(| X(k,n) |)}{K/2}\Big)}{\frac{\sum_{k=0}^{K/2-1} | X(k,n) |}{K/2} } $$

* Flux

$$ SF_n = \frac{\sqrt{\sum_{k=0}^{K/2-1} \Big( | X(k,n) | - | X(k,n-1) | \Big)^2
}}{K/2}$$

* Peak frequency

$$ PK_n = \frac{{argmax}_k \{ |X(k,n)| \} f_s}{K}  $$

* Rolloff (at 85%)

$$ R_n = i \text{  such that  } \sum_{k=0}^{i} | X(k,n) | = 0.85 \sum_{k=0}^{K/2-1} | X(k,n) | $$

* Skewness

$$ SK_n = \frac{\sum_{k=0}^{K/2-1}(f(k) - CF_n)^3 |X(k,n)|}{\sum_{k=0}^{K/2-1} |X(k,n)|} $$

* Slope

$$ SL_n = \frac{\sum_{k=0}^{K/2-1}(f(k) - \mu_f)(|X(k,n)| - \frac{\sum_{k=0}^{K/2-1}|X(k,n)|}{K/2})}{\sum_{k=0}^{K/2-1} (f(k) - \mu_f)^2} $$

* Spread

$$ SP_n = \sqrt{\frac{\sum_{k=0}^{K/2-1} (f(k) - CF_n)^2|X(k,n)|^2}{\sum_{k=0}^{K/2-1}|X(k,n)|^2}} $$

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
