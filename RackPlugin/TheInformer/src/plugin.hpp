#pragma once
#include <rack.hpp>
#include <dsp/fft.hpp>
#include <iostream>
#include <vector>
#include <informer.h>

#include "oscpkt.hh"
#include "udp.hh"

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelTheInformer;