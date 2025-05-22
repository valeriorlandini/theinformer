#pragma once
#include <rack.hpp>
#include <dsp/fft.hpp>
#include <vector>
#include "informer.h"
#include "oscpp/include/oscpp/client.hpp"

            #include <iostream>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelTheInformer;