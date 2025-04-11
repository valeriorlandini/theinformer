/******************************************************************************
Copyright (c) 2023-2025 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

#if __cplusplus >= 202002L
#include<concepts>
#endif

template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type peak(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample peak = (TSample)0.0;

    for (const auto &s : buffer)
    {
        if (abs(s) > peak)
        {
            peak = abs(s);
        }
    }

    return peak;
}

template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type rms(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample rms = (TSample)0.0;

    for (const auto &s : buffer)
    {
        rms += s * s;
    }

    if (buffer.size() > 0)
    {
        rms /= buffer.size();
        rms = std::sqrt(rms);
    }

    return rms;
}

template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type variance(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample variance = (TSample)0.0;

    if (buffer.empty())
    {
        return variance;
    }

    const TSample count = static_cast<TSample>(buffer.size());
    const TSample mean = std::accumulate(buffer.begin(), buffer.end(), (TSample)0.0) / count;

    for (const auto &s : buffer)
    {
        variance += pow(s - mean, (TSample)2.0);
    }

    variance /= count;

    return variance;
}

template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<typename TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type kurtosis(const Container& buffer, const TSample &mean, const TSample &variance)
{
    TSample kurtosis = (TSample)0.0;

    if (buffer.empty() || variance == (TSample)0.0)
    {
        return kurtosis;
    }

    TSample invSqrChVariance = (TSample)1.0 / (variance * variance);

    const TSample count = static_cast<TSample>(buffer.size());

    for (const auto &s : buffer)
    {
        kurtosis += std::pow(s - mean, 4.0f) * invSqrChVariance;
    }

    kurtosis /= count;

    return kurtosis;
}

template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type kurtosis(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample variance = variance(buffer);

    if (buffer.empty() || variance == (TSample)0.0)
    {
        return kurtosis;
    }

    const TSample count = static_cast<TSample>(buffer.size());
    const TSample mean = std::accumulate(buffer.begin(), buffer.end(), (TSample)0.0) / count;

    return kurtosis(buffer, mean, variance);
}

template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<typename TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type centroid(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const bool& only_real = true, const Container& precomputed_frequencies = {})
{
    TSample centroid = static_cast<TSample>(0.0);

    if (stft.empty())
    {
        return centroid;
    }

    unsigned int fft_size = only_real ? stft.size() : stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        TSample fft_bandwidth = static_cast<TSample>(sample_rate) / static_cast<TSample>(fft_size);
        precomputed_frequencies.resize(fft_size);
        precomputed_frequencies.fill(static_cast<TSample>(0.0));
        for (auto b = 0u; b < fft_size / 2u; b++)
        {
            precomputed_frequencies.at(b) = static_cast<TSample>(b) * fft_bandwidth;
        }
    }

    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        centroid += precomputed_frequencies.at(k) * std::abs(stft.at(k));
        magn_sum += std::abs(stft.at(k));
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        centroid /= magn_sum;
    }
    else
    {
        centroid = static_cast<TSample>(0.0);
    }

    return centroid;
}

#endif // DESCRIPTORS_H_
