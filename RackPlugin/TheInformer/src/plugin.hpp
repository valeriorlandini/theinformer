#pragma once
/*
#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
*/
#include <iostream>
#include <vector>
#include <rack.hpp>
#include <dsp/fft.hpp>
#include <informer.h>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelTheInformer;