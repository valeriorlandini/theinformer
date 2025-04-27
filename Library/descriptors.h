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
#include <sys/stat.h>
#include <vector>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace Informer
{

/* TIME DOMAIN DESCRIPTORS */

namespace Amplitude
{

// PEAK
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type peak(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample peak = static_cast<TSample>(0.0);

    for (const auto &s : buffer)
    {
        if (abs(s) > peak)
        {
            peak = abs(s);
        }
    }

    return peak;
}

// ROOT MEAN SQUARE
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type rms(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample rms_amp = static_cast<TSample>(0.0);

    for (const auto &s : buffer)
    {
        rms_amp += s * s;
    }

    if (buffer.size() > 0)
    {
        rms_amp /= buffer.size();
        rms_amp = std::sqrt(rms_amp);
    }

    return rms_amp;
}

// VARIANCE
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type variance(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample amp_variance = static_cast<TSample>(0.0);

    if (buffer.empty())
    {
        return amp_variance;
    }

    const TSample count = static_cast<TSample>(buffer.size());
    const TSample mean = std::accumulate(buffer.begin(), buffer.end(), static_cast<TSample>(0.0)) / count;

    for (const auto &s : buffer)
    {
        amp_variance += pow(s - mean, (TSample)2.0);
    }

    amp_variance /= count;

    return amp_variance;
}

// KURTOSIS
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type kurtosis(const Container& buffer, const TSample &mean = static_cast<TSample>(-10000.0), const TSample &amp_variance = static_cast<TSample>(-10000.0))
{
    TSample amp_kurtosis = static_cast<TSample>(0.0);
    TSample var = amp_variance;
    TSample amp_mean = mean;

    if (var < static_cast<TSample>(-9999.0))
    {
        var = variance(buffer);
    }

    if (mean < static_cast<TSample>(-9999.0))
    {
        amp_mean = std::accumulate(buffer.begin(), buffer.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(buffer.size());
    }
    
    if (buffer.empty() || var == static_cast<TSample>(0.0))
    {
        return amp_kurtosis;
    }

    TSample invSqrChVariance = (TSample)1.0 / (var * var);

    for (const auto &s : buffer)
    {
        amp_kurtosis += std::pow(s - amp_mean, (TSample)4.0);
    }

    amp_kurtosis /= static_cast<TSample>(buffer.size());
    amp_kurtosis *= invSqrChVariance;
    amp_kurtosis -= (TSample)3.0;

    return amp_kurtosis;
}

// SKEWNESS
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type skewness(const Container& buffer, const TSample &mean = static_cast<TSample>(-10000.0), const TSample &amp_variance = static_cast<TSample>(-10000.0))
{
    TSample skewness = static_cast<TSample>(0.0);
    TSample var = amp_variance;
    TSample amp_mean = mean;

    if (var < static_cast<TSample>(-9999.0))
    {
        var = variance(buffer);
    }

    if (mean < static_cast<TSample>(-9999.0))
    {
        amp_mean = std::accumulate(buffer.begin(), buffer.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(buffer.size());
    }

    if (buffer.empty() || var == static_cast<TSample>(0.0))
    {
        return skewness;
    }

    TSample invDenominator = (TSample)1.0 / std::pow(std::sqrt(var), (TSample)3.0);

    for (const auto &s : buffer)
    {
        skewness += std::pow(s - amp_mean, (TSample)3.0);
    }

    skewness /= static_cast<TSample>(buffer.size());
    skewness *= invDenominator;

    return skewness;
}

// ZERO CROSSING RATE
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type zerocrossing(const Container& buffer)
{
    using TSample = typename Container::value_type;
    TSample zerocrossingrate = static_cast<TSample>(0.0);

    if (buffer.size() < 2)
    {
        return zerocrossingrate;
    }

    for (auto s = 1; s < buffer.size(); s++)
    {
        zerocrossingrate += std::abs(static_cast<TSample>(!std::signbit(buffer.at(s))) - static_cast<TSample>(!std::signbit(buffer.at(s - 1))));
    }

    zerocrossingrate /= static_cast<TSample>(buffer.size() - 1);
}

} // namespace Informer::Amplitude


/* FREQUENCY DOMAIN DESCRIPTORS */

namespace Frequency
{

// SPECTRAL CENTROID
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type centroid(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {})
{
    TSample centroid = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return centroid;
    }

    unsigned int fft_size = stft.size() / 2u;

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

// SPECTRAL CREST FACTOR
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type crestfactor(const Container& stft)
{
    TSample scf = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return scf;
    }

    unsigned int fft_size = stft.size() / 2u;

    TSample magn_max = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        if (std::abs(stft.at(k)) > magn_max)
        {
            magn_max = std::abs(stft.at(k));
        }
        magn_sum += std::abs(stft.at(k));
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        scf = magn_max / magn_sum;
    }

    return scf;
}

// SPECTRAL DECREASE
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type decrease(const Container& stft)
{
    TSample decrease = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return decrease;
    }

    unsigned int fft_size = stft.size() / 2u;

    TSample magn_diff_sum = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 1u; k < fft_size; k++)
    {
        magn_diff_sum += (std::abs(stft.at(k)) - std::abs(stft.at(0))) / static_cast<TSample>(k);
        magn_sum += std::abs(stft.at(k));
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        return decrease;
    }

    decrease = magn_diff_sum / magn_sum;

    return decrease;
}

// SPECTRAL ENTROPY
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type entropy(const Container& stft)
{
    TSample h = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return h;
    }

    unsigned int fft_size = stft.size() / 2u;

    TSample power_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 1u; k < fft_size; k++)
    {
        power_sum += stft.at(k) * stft.at(k);
    }

    if (power_sum > static_cast<TSample>(0.0))
    {
        for (unsigned int k = 1u; k < fft_size; k++)
        {
            TSample p = stft.at(k) * stft.at(k) / power_sum;
            h += p * std::log(p);
        }
    }

    h /= std::log(static_cast<TSample>(fft_size));
    h *= static_cast<TSample>(-1.0);

    return h;
}

// SPECTRAL FLATNESS
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type flatness(const Container& stft)
{
    TSample specflatness = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return specflatness;
    }

    unsigned int fft_size = stft.size() / 2u;

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample ln_magn_sum = static_cast<TSample>(0.0);
    const TSample delta = static_cast<TSample>(1e-10);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum += std::abs(stft.at(k));
        ln_magn_sum += std::log(std::abs(stft.at(k)) + delta);
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        specflatness = std::exp(ln_magn_sum / static_cast<TSample>(fft_size)) / (magn_sum / static_cast<TSample>(fft_size));
    }

    return specflatness;
}

// SPECTRAL FLUX
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type flux(const Container& stft, const Container& previous_stft)
{
    TSample specflux = static_cast<TSample>(0.0);

    if (stft.size() != previous_stft.size() || stft.empty())
    {
        return specflux;
    }

    unsigned int fft_size = stft.size() / 2u;

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        specflux += std::pow(std::abs(stft.at(k)) - std::abs(previous_stft.at(k)), static_cast<TSample>(2.0));
    }

    specflux = std::sqrt(specflux);
    specflux /= static_cast<TSample>(fft_size);

    return specflux;
}

// SPECTRAL IRREGULARITY
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type irregularity(const Container& stft)
{
    TSample irr = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return irr;
    }

    unsigned int fft_size = stft.size() / 2u;

    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum += std::abs(stft.at(k));

        if (k > 1u) 
        {
            irr += std::abs(stft.at(k) - stft.at(k - 1u));
        }
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        irr /= magn_sum;
    }
    else
    {
        irr = static_cast<TSample>(0.0);
    }

    return irr;
}

// SPECTRAL KURTOSIS
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type kurtosis(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {}, const TSample& spectral_centroid = static_cast<TSample>(-1.0), const TSample& spectral_spread = static_cast<TSample>(-1.0))
{
    TSample skurtosis = static_cast<TSample>(0.0);
    TSample scentroid = spectral_centroid;
    TSample sspread = spectral_spread;

    if (stft.size() < 2)
    {
        return skurtosis;
    }

    unsigned int fft_size = stft.size() / 2u;

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

    if (spectral_centroid < static_cast<TSample>(0.0))
    {
        scentroid = centroid(stft, sample_rate, precomputed_frequencies);
    }

    if (spectral_spread < static_cast<TSample>(0.0))
    {
        sspread = spread(stft, scentroid, sample_rate, precomputed_frequencies);
    }

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample numerator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += std::pow(precomputed_frequencies.at(k) - scentroid, static_cast<TSample>(4.0)) * std::abs(stft.at(k));
        magn_sum += std::abs(stft.at(k));
    }

    if (magn_sum * sspread > static_cast<TSample>(0.0))
    {
        skurtosis = numerator / (magn_sum * std::pow(sspread, static_cast<TSample>(4.0)));
        skurtosis -= static_cast<TSample>(3.0);
    }

    return skurtosis;
}

// SPECTRAL PEAK
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type peak(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {})
{
    TSample peak = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return peak;
    }

    unsigned int fft_size = stft.size() / 2u;

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

    unsigned int magn_max_idx = 0;
    TSample magn_max = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        if (std::abs(stft.at(k)) > magn_max)
        {
            magn_max = std::abs(stft.at(k));
            magn_max_idx = k;
        }
    }

    return precomputed_frequencies.at(magn_max_idx);
}

// SPECTRAL ROLLOFF
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type rolloff(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const TSample& rolloff_point = static_cast<TSample>(0.85), const Container& precomputed_frequencies = {})
{
    if (stft.size() < 2)
    {
        return static_cast<TSample>(0.0);
    }

    unsigned int fft_size = stft.size() / 2u;

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
        magn_sum += std::abs(stft.at(k));
    }

    TSample rolloff_thresh = rolloff_point * magn_sum;
    TSample cumul_magn = static_cast<TSample>(0.0);
    unsigned int rolloff_idx = fft_size - 1u;

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        cumul_magn += std::abs(stft.at(k));

        if (cumul_magn >= rolloff_thresh)
        {
            rolloff_idx = k;
            break;
        }
    }

    return precomputed_frequencies.at(rolloff_idx);
}

// SPECTRAL SKEWNESS
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type skweness(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {}, const TSample& spectral_centroid = static_cast<TSample>(-1.0), const TSample& spectral_spread = static_cast<TSample>(-1.0))
{
    TSample sskewness = static_cast<TSample>(0.0);
    TSample scentroid = spectral_centroid;
    TSample sspread = spectral_spread;

    if (stft.size() < 2)
    {
        return sskewness;
    }

    unsigned int fft_size = stft.size() / 2u;

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

    if (spectral_centroid < static_cast<TSample>(0.0))
    {
        scentroid = centroid(stft, sample_rate, precomputed_frequencies);
    }

    if (spectral_spread < static_cast<TSample>(0.0))
    {
        sspread = spread(stft, scentroid, sample_rate, precomputed_frequencies);
    }

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample numerator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += std::pow(precomputed_frequencies.at(k) - scentroid, static_cast<TSample>(3.0)) * std::abs(stft.at(k));
        magn_sum += std::abs(stft.at(k));
    }

    if (magn_sum * sspread > static_cast<TSample>(0.0))
    {
        sskewness = numerator / (magn_sum * std::pow(sspread, static_cast<TSample>(3.0)));
    }

    return sskewness;
}

// SPECTRAL SLOPE
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type slope(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {})
{
    TSample sslope = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return sslope;
    }

    unsigned int fft_size = stft.size() / 2u;

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

    TSample mean_frequency = std::accumulate(precomputed_frequencies.begin(), precomputed_frequencies.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(fft_size);
    TSample magn_sum_weighted = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum_weighted += std::abs(stft.at(k));
    }

    magn_sum_weighted /= static_cast<TSample>(fft_size);

    TSample numerator = static_cast<TSample>(0.0);
    TSample denominator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += (precomputed_frequencies.at(k) - mean_frequency) * (std::abs(stft.at(k)) - magn_sum_weighted);
        denominator += std::pow(precomputed_frequencies.at(k) - mean_frequency, static_cast<TSample>(2.0));
    }

    if (denominator > static_cast<TSample>(0.0))
    {
        sslope = numerator / denominator;
    }

    return sslope;
}

// SPECTRAL SPREAD
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type spread(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const Container& precomputed_frequencies = {}, TSample centroid = static_cast<TSample>(-1.0))
{
    TSample sspread = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return sspread;
    }

    unsigned int fft_size = stft.size() / 2u;

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

    if (centroid < static_cast<TSample>(0.0))
    {
        centroid = centroid(stft, sample_rate, precomputed_frequencies);
    }

    TSample numerator = static_cast<TSample>(0.0);
    TSample power_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += (precomputed_frequencies.at(k) - centroid) * (precomputed_frequencies.at(k) - centroid) * (stft.at(k) * stft.at(k));
        power_sum += stft.at(k) * stft.at(k);
    }

    if (power_sum > static_cast<TSample>(0.0))
    {
        sspread = std::sqrt(numerator / power_sum);
    }
    else
    {
        sspread = static_cast<TSample>(0.0);
    }

    return sspread;
}


} // namespace Informer::Frequency


/* CLASS INTERFACE */

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Informer
{
public:
    Informer() = default;
    ~Informer() = default;

    bool set_sample_rate(const TSample& rate)
    {
        if (rate > static_cast<TSample>(0.0))
        {
            sample_rate = rate;
            return true;
        }
        return false;
    }

    bool set_rolloff_point(const TSample& rolloff)
    {
        if (rolloff > static_cast<TSample>(0.0) && rolloff < static_cast<TSample>(1.0))
        {
            rolloff_point = rolloff;
            return true;
        }
        return false;
    }

    bool set_stft(const std::vector<TSample>& stft)
    {
        if (stft.size() > 2)
        {
            this->previous_stft = this->stft;
            this->stft = stft;
            return true;
        }
        return false;
    }

    bool set_buffer(const std::vector<TSample>& buffer)
    {
        if (!buffer.empty())
        {
            this->buffer = buffer;
            return true;
        }
        return false;
    }

    std::vector<TSample> get_stft() const
    {
        return stft;
    }

    std::vector<TSample> get_buffer() const
    {
        return buffer;
    }

    TSample get_sample_rate() const
    {
        return sample_rate;
    }

    TSample get_rolloff_point() const
    {
        return rolloff_point;
    }

    std::vector<TSample> get_precomputed_frequencies() const
    {
        return precomputed_frequencies;
    }

    void set_precomputed_frequencies(const std::vector<TSample>& frequencies)
    {
        precomputed_frequencies = frequencies;
    }

    void time_descriptors(const std::vector<TSample>& buffer)
    {
        this->buffer = buffer;
    }

    void calculate_frequency_descriptors(const std::vector<TSample>& stft, const std::vector<TSample>& previous_stft)
    {
        this->stft = stft;
        this->previous_stft = previous_stft;
    }

    // Time domain descriptors
    TSample amp_peak() { return Amplitude::peak(buffer); }
    TSample amp_rms() { return Amplitude::rms(buffer); }
    TSample amp_variance() { return Amplitude::variance(buffer); }
    TSample amp_kurtosis(const std::vector<TSample>& buffer)
    {
        TSample mean = std::accumulate(buffer.begin(), buffer.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(buffer.size());
        TSample amp_variance = Amplitude::variance(buffer);

        return Amplitude::kurtosis(buffer, mean, amp_variance);
    }
    TSample amp_skewness(const std::vector<TSample>& buffer)
    {
        TSample mean = std::accumulate(buffer.begin(), buffer.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(buffer.size());
        TSample amp_variance = Amplitude::variance(buffer);

        return Amplitude::skewness(buffer, mean, amp_variance);
    }
    TSample amp_zerocrossing() { return Amplitude::zerocrossing(buffer); }

    // Frequency domain descriptors
    TSample spectral_centroid() { return Frequency::centroid(stft, sample_rate, precomputed_frequencies); }
    TSample spectral_crestfactor() { return Frequency::crestfactor(stft); }
    TSample spectral_decrease() { return Frequency::decrease(stft); }
    TSample spectral_entropy() { return Frequency::entropy(stft); }
    TSample spectral_flatness() { return Frequency::flatness(stft); }
    TSample spectral_flux() { return Frequency::flux(stft, previous_stft); }
    TSample spectral_irregularity() { return Frequency::irregularity(stft); }
    TSample spectral_kurtosis()
    {
        if (this->centroid < static_cast<TSample>(0.0))
        {
            this->centroid = Frequency::centroid(stft, sample_rate, precomputed_frequencies);
        }

        if (this->spread < static_cast<TSample>(0.0))
        {
            this->spread = Frequency::spread(stft, sample_rate, precomputed_frequencies, centroid);
        }

        return Frequency::kurtosis(stft, sample_rate, precomputed_frequencies, centroid, spread);
    }
    TSample spectral_peak() { return Frequency::peak(stft, sample_rate, precomputed_frequencies); }
    TSample spectral_rolloff() { return Frequency::rolloff(stft, sample_rate, rolloff_point, precomputed_frequencies); }
    TSample spectral_skweness()
    {
        if (this->centroid < static_cast<TSample>(0.0))
        {
            this->centroid = Frequency::centroid(stft, sample_rate, precomputed_frequencies);
        }

        if (this->spread < static_cast<TSample>(0.0))
        {
            this->spread = Frequency::spread(stft, sample_rate, precomputed_frequencies, centroid);
        }

        return Frequency::skweness(stft, sample_rate, precomputed_frequencies, centroid, spread);
    }
    TSample spectral_slope() { return Frequency::slope(stft, sample_rate, precomputed_frequencies); }
    TSample spectral_spread()
    {
        if (this->centroid < static_cast<TSample>(0.0))
        {
            this->centroid = Frequency::centroid(stft, sample_rate, precomputed_frequencies);
        }
        return Frequency::spread(stft, sample_rate, precomputed_frequencies, centroid);
    }

private:
    std::vector<TSample> precomputed_frequencies;
    std::vector<TSample> stft;
    std::vector<TSample> previous_stft;
    std::vector<TSample> buffer;
    TSample sample_rate = static_cast<TSample>(44100.0);
    TSample rolloff_point = static_cast<TSample>(0.85);
    TSample centroid = static_cast<TSample>(-1.0);
    TSample spread = static_cast<TSample>(-1.0);
};

} // namespace Informer 

#endif // DESCRIPTORS_H_
