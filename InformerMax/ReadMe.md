# sonus
A collection of various Max/MSP objects for creative patching coded in C++. This package was born as a collection of some custom externals
I made for different projects, so they are not really linked by a common thread. They anyway more or less deal with math,
artificial intelligence and weird sounds.

Currently finished externals are:

### Generators
* `sonus.bytebeat~` A bytebeat parser and generator
* `sonus.byteplay~` An efficient player of many bytebeat formulas
* `sonus.ecaosc~` Elementary cellular automata oscillator
* `sonus.harmosc~` Harmonic additive oscillator
* `sonus.lorenz~` Lorenz attractor based generator
* `sonus.neurosc~` Neural network based oscillator
* `sonus.nwosc~` User defined wavetable oscillator
* `sonus.nowave~` Scrambled wavetable oscillator
* `sonus.pdosc~` Phase distorted oscillator
* `sonus.pulsar~` Pulsar oscillator
* `sonus.roessler~` Rössler attractor based generator
* `sonus.rsosc~` Oscillator with random shapes
* `sonus.vaosc~` Multishape bandlimited oscillator
* `sonus.window~` Window functions generator

### Effects and Sample Manipulation
* `sonus.bitinv~` Bitcrusher and bit inverter
* `sonus.bitman~` Bit manipulator
* `sonus.biquads~` Parallel biquad filters
* `sonus.buffx` Apply effects to buffers
* `sonus.bufsnap` Save snapshots of a buffer
* `sonus.cheby~` Chebyshev waveshaper
* `sonus.crossloop~` Buffer loop player with crossfade
* `sonus.cryptoverb~` Eerie stereo reverberator
* `sonus.fselector~` Continuous selector/mixer
* `sonus.halaphon~` Circular panner across multiple speakers
* `sonus.quadpan~` Quadraphonic panner
* `sonus.summer~` Simple signal mixer
* `sonus.wavesets` Wavesets based sample manipulation

### Neural Networks and AI
* `sonus.acti` Activation functions
* `sonus.acti~` Activation functions (signal rate)
* `sonus.ca` Cellular automata system
* `sonus.dnn` Deep neural network implementation
* `sonus.ga` Genetic algorithm implementation
* `sonus.ga.num` Genetic algorithm implementation (numbers version)
* `sonus.linden` Lindenmayer generative system
* `sonus.neuron` Artificial neuron implementation
* `sonus.perceptron~` Classic perceptron implementation

### Math and Utilities
* `sonus.blist` Output different items from a list
* `sonus.bozo` Output a 1 and then a 0 when a bang is received
* `sonus.edist` Euclidean distance between two points
* `sonus.ftomel` Frequency to mel converter
* `sonus.hop` Hot inlets operations
* `sonus.lline` Similar to line, but for list of numbers
* `sonus.logistic` Logistic map calculator
* `sonus.lrandom` Random items from a list
* `sonus.lurn` Non repeating random sequences
* `sonus.mathc` Mathematical constants
* `sonus.mbox` Pop up dialogs generator
* `sonus.meltof` Mel to frequency converter
* `sonus.mima~` Signal extremes analyzer
* `sonus.multirandom` Multi-outlet version of random object
* `sonus.mumul` Multiply numbers for the same factor
* `sonus.mumul~` Multiply signals for the same factor
* `sonus.quadpanner` Quadraphonic panner coefficients
* `sonus.summer` Numbers summator
* `sonus.tent` Tent map calculator
* `sonus.wtp` "The Well-Tuned Piano" tuner

The collection also features six Max for Live devices that use some sonus externals to create synths, effects and modulators.

## Compiled binaries

Compiled binaries for both Windows and macOS can be found in the [Releases section](https://github.com/valeriorlandini/sonus/releases). Please note that some of the listed externals may not yet be available in the latest compiled version.

## How to build

Grab the source inside your `Documents/Max 9/Packages` (or `Max 8`, according to your Max/MSP version) folder with `git clone https://github.com/valeriorlandini/sonus.git --recursive` 

`cd sonus` and then `mkdir build` to create a folder with your various build files.

`cd build` to put yourself into that folder.

### Mac 

Run `cmake -G Xcode ..`

Next run `cmake --build . --config Release` or open the Xcode project from this "build" folder and use the GUI.

Note: you can add the `-jX` option where X is the number of cores to use (e.g. `-j4`). This can help speed up your builds, though it may sometimes interleave the error output, making troubleshooting more challenging.

If you are running on a Mac M1+ machine, you will likely see an error `cannot be loaded due to system security policy` when loading your externals in Max. To resolve this, you can ad-hoc codesign your external with `codesign --force --deep -s - myobject.mxo`.

### Windows

You can run `cmake --help` to get a list of the options available. 

Visual Studio 2022:

`cmake -G "Visual Studio 17 2022" ..`

Visual Studio 2019:

`cmake -G "Visual Studio 16 2019" ..`

Visual Studio 2017:

`cmake -G "Visual Studio 15 2017 Win64" ..`

Having generated the projects, you can now build by opening the .sln file in the build folder with the Visual Studio app (just double-click the .sln file) or you can build on the command line like this:

`cmake --build . --config Release`

Note: you can add the `-jX` option where X is the number of cores to use (e.g. `-j4`). This can help speed up your builds, though it may sometimes interleave the error output, making troubleshooting more challenging.

