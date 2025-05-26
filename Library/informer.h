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

#ifndef INFORMER_H_
#define INFORMER_H_

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

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
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type kurtosis(const Container& buffer,
typename Container::value_type mean = static_cast<typename Container::value_type>(-10000.0),
typename Container::value_type amp_variance = static_cast<typename Container::value_type>(-10000.0))
{
    using TSample = typename Container::value_type;

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
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type skewness(const Container& buffer,
typename Container::value_type mean = static_cast<typename Container::value_type>(-10000.0),
typename Container::value_type amp_variance = static_cast<typename Container::value_type>(-10000.0))
{
    using TSample = typename Container::value_type;

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

    for (auto s = 1u; s < buffer.size(); s++)
    {
        zerocrossingrate += std::abs(static_cast<TSample>(!std::signbit(buffer[s])) - static_cast<TSample>(!std::signbit(buffer[s - 1])));
    }

    zerocrossingrate /= static_cast<TSample>(buffer.size() - 1);

    return zerocrossingrate;
}

} // namespace Informer::Amplitude


/* FREQUENCY DOMAIN DESCRIPTORS */

namespace Frequency
{

// UTILITY FUNCTION: precompute the frequencies for the given STFT size and sample rate
template <typename ISize, typename TSample>
std::vector<TSample> precompute_frequencies(ISize stft_size = static_cast<ISize>(4096), TSample sample_rate = static_cast<TSample>(44100.0))
{
    std::vector<TSample> precomputed_frequencies = {};
    unsigned int size = static_cast<unsigned int>(stft_size);

    if (size > 2u)
    {
        TSample fft_bandwidth = static_cast<TSample>(sample_rate) / static_cast<TSample>(size);
        precomputed_frequencies.resize(size / 2u + 1u, static_cast<TSample>(0.0));
        for (auto b = 0u; b < size / 2u + 1; b++)
        {
            precomputed_frequencies[b] = static_cast<TSample>(b) * fft_bandwidth;
        }
    }

    return precomputed_frequencies;
}

// UTILITY FUNCTION: from a real valued FFT, calculate the magnitudes
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
std::vector<typename Container::value_type> magnitudes(const Container& stft)
{
    using TSample = typename Container::value_type;

    std::vector<TSample> magnitudes_vector;

    if (stft.size() < 2u)
    {
        return magnitudes_vector;
    }

    magnitudes_vector.assign(stft.size() / 2 + 1, static_cast<TSample>(0.0));

    // DC offset
    magnitudes_vector[0] = stft[0];

    // Values up to Nyquist bin - 1
    for (auto b = 2; b < stft.size(); b += 2)
    {
        TSample real = stft[b];
        TSample imag = stft[b];
        magnitudes_vector[b / 2] = sqrt(real * real + imag * imag) / static_cast<TSample>(stft.size());
    }

    // Nyquist frequency
    magnitudes_vector[stft.size() / 2 + 1] = stft[1];

    return magnitudes_vector;
}

// SPECTRAL CENTROID
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type centroid(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {}, unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;
    
    TSample centroid = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return centroid;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        centroid += precomputed_frequencies[k] * std::abs(magnitudes[k]);
        magn_sum += std::abs(magnitudes[k]);
        //std::cout << "Magnitude sum: " << magn_sum << std::endl;
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        //std::cout << magn_sum << "\n";
        centroid /= magn_sum;
        //std::cout << "Centroid: " << centroid << std::endl;
    }
    else
    {
        centroid = static_cast<TSample>(0.0);
    }

    return centroid;
}

// SPECTRAL SPREAD
// Not in alphabetical order because it is used in the kurtosis and skewness functions
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type spread(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
typename Container::value_type spectral_centroid = static_cast<typename Container::value_type>(-1.0),
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    TSample sspread = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return sspread;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    if (spectral_centroid < static_cast<TSample>(0.0))
    {
        spectral_centroid = centroid(magnitudes, sample_rate, precomputed_frequencies);
    }

    TSample numerator = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += (precomputed_frequencies[k] - spectral_centroid) * (precomputed_frequencies[k] - spectral_centroid) * std::abs(magnitudes[k]);
        magn_sum += std::abs(magnitudes[k]);
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        sspread = std::sqrt(numerator / magn_sum);
    }
    else
    {
        sspread = static_cast<TSample>(0.0);
    }

    return sspread;
}

// SPECTRAL CREST FACTOR
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type crestfactor(const Container& magnitudes)
{
    using TSample = typename Container::value_type;

    TSample scf = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return scf;
    }

    unsigned int fft_size = magnitudes.size();

    TSample magn_max = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        if (std::abs(magnitudes[k]) > magn_max)
        {
            magn_max = std::abs(magnitudes[k]);
        }
        magn_sum += std::abs(magnitudes[k]);
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        scf = magn_max / magn_sum;
    }

    return scf;
}

// SPECTRAL DECREASE
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type decrease(const Container& magnitudes)
{
    using TSample = typename Container::value_type;

    TSample decrease = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return decrease;
    }

    unsigned int fft_size = magnitudes.size();

    TSample magn_diff_sum = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 1u; k < fft_size; k++)
    {
        magn_diff_sum += (std::abs(magnitudes[k]) - std::abs(magnitudes[0])) / static_cast<TSample>(k);
        magn_sum += std::abs(magnitudes[k]);
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        decrease = magn_diff_sum / magn_sum;
    }

    return decrease;
}

// SPECTRAL ENTROPY
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type entropy(const Container& magnitudes)
{
    using TSample = typename Container::value_type;

    TSample h = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return h;
    }

    unsigned int fft_size = magnitudes.size();

    TSample power_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        power_sum += magnitudes[k] * magnitudes[k];
    }

    if (power_sum > static_cast<TSample>(0.0))
    {
        for (unsigned int k = 0u; k < fft_size; k++)
        {
            TSample p = magnitudes[k] * magnitudes[k] / power_sum;
            if (p > static_cast<TSample>(0.0))
            {
                h += p * std::log2(p);
            }
        }
    }

    h /= std::log2(static_cast<TSample>(fft_size));
    h *= static_cast<TSample>(-1.0);

    return h;
}

// SPECTRAL FLATNESS
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type flatness(const Container& magnitudes)
{
    using TSample = typename Container::value_type;

    TSample specflatness = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return specflatness;
    }

    unsigned int fft_size = magnitudes.size();

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample ln_magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum += std::abs(magnitudes[k]);
        if (std::abs(magnitudes[k]) > static_cast<TSample>(0.0))
        {
            ln_magn_sum += std::log(std::abs(magnitudes[k]));
        }
        else
        {
            ln_magn_sum += std::log(static_cast<TSample>(0.00001));
        }
    }

    if (magn_sum > static_cast<TSample>(0.0))
    {
        specflatness = std::exp(ln_magn_sum / static_cast<TSample>(fft_size)) / (magn_sum / static_cast<TSample>(fft_size));
    }

    return specflatness;
}

// SPECTRAL FLUX
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type flux(const Container& magnitudes, const Container& previous_magnitudes)
{
    using TSample = typename Container::value_type;

    TSample specflux = static_cast<TSample>(0.0);

    if (magnitudes.size() != previous_magnitudes.size() || magnitudes.empty())
    {
        return specflux;
    }

    unsigned int fft_size = magnitudes.size();

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        specflux += std::pow(std::abs(magnitudes[k]) - std::abs(previous_magnitudes[k]), static_cast<TSample>(2.0));
    }

    specflux = std::sqrt(specflux);
    specflux /= static_cast<TSample>(fft_size);

    return specflux;
}

// SPECTRAL IRREGULARITY
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type irregularity(const Container& magnitudes)
{
    using TSample = typename Container::value_type;

    TSample irr = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return irr;
    }

    unsigned int fft_size = magnitudes.size();

    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum += std::abs(magnitudes[k]);

        if (k > 0u)
        {
            irr += std::abs(magnitudes[k] - magnitudes[k - 1u]);
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
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type kurtosis(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
typename Container::value_type spectral_centroid = static_cast<typename Container::value_type>(-1.0),
typename Container::value_type spectral_spread = static_cast<typename Container::value_type>(-1.0),
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    TSample skurtosis = static_cast<TSample>(0.0);
    TSample scentroid = spectral_centroid;
    TSample sspread = spectral_spread;

    if (magnitudes.empty())
    {
        return skurtosis;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    if (spectral_centroid < static_cast<TSample>(0.0))
    {
        scentroid = centroid(magnitudes, sample_rate, precomputed_frequencies);
    }

    if (spectral_spread < static_cast<TSample>(0.0))
    {
        sspread = spread(magnitudes, sample_rate, precomputed_frequencies, scentroid);
    }

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample numerator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += std::pow(precomputed_frequencies[k] - scentroid, static_cast<TSample>(4.0)) * std::abs(magnitudes[k]);
        magn_sum += std::abs(magnitudes[k]);
    }

    if (magn_sum * sspread > static_cast<TSample>(0.0))
    {
        skurtosis = numerator / (magn_sum * std::pow(sspread, static_cast<TSample>(4.0)));
        skurtosis -= static_cast<TSample>(3.0);
    }

    return skurtosis;
}

// SPECTRAL PEAK
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type peak(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    TSample peak = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return peak;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    unsigned int magn_max_idx = 0;
    TSample magn_max = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        if (std::abs(magnitudes[k]) > magn_max)
        {
            magn_max = std::abs(magnitudes[k]);
            magn_max_idx = k;
        }
    }

    return precomputed_frequencies[magn_max_idx];
}

// SPECTRAL ROLLOFF
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type rolloff(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
typename Container::value_type rolloff_point = static_cast<typename Container::value_type>(0.85),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    if (magnitudes.empty())
    {
        return static_cast<TSample>(0.0);
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum += std::abs(magnitudes[k]);
    }

    TSample rolloff_thresh = rolloff_point * magn_sum;
    TSample cumul_magn = static_cast<TSample>(0.0);
    unsigned int rolloff_idx = fft_size - 1u;

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        cumul_magn += std::abs(magnitudes[k]);

        if (cumul_magn >= rolloff_thresh)
        {
            rolloff_idx = k;
            break;
        }
    }

    return precomputed_frequencies[rolloff_idx];
}

// SPECTRAL SKEWNESS
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type skweness(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
typename Container::value_type spectral_centroid = static_cast<typename Container::value_type>(-1.0),
typename Container::value_type spectral_spread = static_cast<typename Container::value_type>(-1.0),
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    TSample sskewness = static_cast<TSample>(0.0);
    TSample scentroid = spectral_centroid;
    TSample sspread = spectral_spread;

    if (magnitudes.empty())
    {
        return sskewness;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    if (spectral_centroid < static_cast<TSample>(0.0))
    {
        scentroid = centroid(magnitudes, sample_rate, precomputed_frequencies);
    }

    if (spectral_spread < static_cast<TSample>(0.0))
    {
        sspread = spread(magnitudes, sample_rate, precomputed_frequencies, scentroid);
    }

    TSample magn_sum = static_cast<TSample>(0.0);
    TSample numerator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += std::pow(precomputed_frequencies[k] - scentroid, static_cast<TSample>(3.0)) * std::abs(magnitudes[k]);
        magn_sum += std::abs(magnitudes[k]);
    }

    if (magn_sum * sspread > static_cast<TSample>(0.0))
    {
        sskewness = numerator / (magn_sum * std::pow(sspread, static_cast<TSample>(3.0)));
    }

    return sskewness;
}

// SPECTRAL SLOPE
template <typename Container>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type>
#endif
typename Container::value_type slope(const Container& magnitudes,
typename Container::value_type sample_rate = static_cast<typename Container::value_type>(44100.0),
std::vector<typename Container::value_type>& precomputed_frequencies = {},
unsigned int stft_size = 0u)
{
    using TSample = typename Container::value_type;

    TSample sslope = static_cast<TSample>(0.0);

    if (magnitudes.empty())
    {
        return sslope;
    }

    unsigned int fft_size = magnitudes.size();
    unsigned int bins = stft_size > 0u ? stft_size : (fft_size - 1u) * 2u;

    if (precomputed_frequencies.size() < bins)
    {
        precomputed_frequencies = precompute_frequencies(bins, sample_rate);
    }

    TSample mean_frequency = std::accumulate(precomputed_frequencies.begin(), precomputed_frequencies.end(), static_cast<TSample>(0.0)) / static_cast<TSample>(fft_size);
    TSample magn_sum_weighted = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        magn_sum_weighted += std::abs(magnitudes[k]);
    }

    magn_sum_weighted /= static_cast<TSample>(fft_size);

    TSample numerator = static_cast<TSample>(0.0);
    TSample denominator = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += (precomputed_frequencies[k] - mean_frequency) * (std::abs(magnitudes[k]) - magn_sum_weighted);
        denominator += std::pow(precomputed_frequencies[k] - mean_frequency, static_cast<TSample>(2.0));
    }

    if (denominator > static_cast<TSample>(0.0))
    {
        sslope = numerator / denominator;
    }

    return sslope;
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
    Informer(const std::vector<TSample>& buffer = {},
             const std::vector<TSample>& magnitudes = {},
             const TSample& sample_rate = static_cast<TSample>(44100.0),
             const TSample& rolloff_point = static_cast<TSample>(0.85),
             const std::vector<TSample>& previous_magnitudes = {},
             const unsigned int& stft_size = 0u,
             const bool& compute = true)
    {
        if (!buffer.empty())
        {
            set_buffer(buffer);
        }

        set_sample_rate(sample_rate);

        if (!magnitudes.empty())
        {
            set_magnitudes(magnitudes);
        }

        if (stft_size > 2u)
        {
            set_stft_size(stft_size);
        }
        else if (magnitudes.size() > 2u)
        {
            set_stft_size((magnitudes_.size() - 1u) * 2u);
        }
        else
        {
            set_stft_size(4096u);
        }

        if (!previous_magnitudes.empty())
        {
            set_previous_magnitudes(previous_magnitudes);
        }

        set_rolloff_point(rolloff_point);

        if (compute)
        {
            compute_descriptors();
        }
    }

    ~Informer() = default;

    bool set_sample_rate(const TSample& sample_rate)
    {
        if (sample_rate > static_cast<TSample>(0.0))
        {
            sample_rate_ = sample_rate;

            if (stft_size_ > 2)
            {
                precomputed_frequencies_ = Frequency::precompute_frequencies(stft_size_, sample_rate_);
            }

            return true;
        }

        return false;
    }

    bool set_rolloff_point(const TSample& rolloff)
    {
        if (rolloff > static_cast<TSample>(0.0) && rolloff < static_cast<TSample>(1.0))
        {
            rolloff_point_ = rolloff;

            return true;
        }

        return false;
    }

    bool set_stft_size(const unsigned int& stft_size)
    {
        if (stft_size > 2u)
        {
            stft_size_ = stft_size;

            precomputed_frequencies_ = Frequency::precompute_frequencies(stft_size_, sample_rate_);

            return true;
        }

        return false;
    }

    bool set_magnitudes_from_stft(const std::vector<TSample>& stft)
    {
        if (stft.size() > 2u)
        {
            set_magnitudes(Frequency::magnitudes(stft), true);

            return true;
        }

        return false;
    }

    bool set_previous_magnitudes_from_stft(const std::vector<TSample>& previous_stft)
    {
        if (previous_stft.size() > 2u)
        {
            set_previous_magnitudes(Frequency::magnitudes(previous_stft));

            return true;
        }

        return false;
    }

    bool set_magnitudes(const std::vector<TSample>& magnitudes, bool update_stft_size = false)
    {
        if (magnitudes.size() > 2)
        {
            if (!magnitudes_.empty())
            {
                set_previous_magnitudes(magnitudes_);
            }
            else
            {
                previous_magnitudes_.resize(magnitudes_.size(), static_cast<TSample>(0.0));
            }

            if (magnitudes.size() != magnitudes_.size() && update_stft_size)
            {
                magnitudes_ = magnitudes;
                set_stft_size((magnitudes_.size() - 1u) * 2u);
            }
            else
            {
                magnitudes_ = magnitudes;
            }

            return true;
        }

        return false;
    }

    bool set_previous_magnitudes(const std::vector<TSample>& previous_magnitudes)
    {
        if (previous_magnitudes.size() == magnitudes_.size())
        {
            previous_magnitudes_ = previous_magnitudes;

            return true;
        }

        return false;
    }

    bool set_buffer(const std::vector<TSample>& buffer)
    {
        if (!buffer.empty())
        {
            buffer_ = buffer;

            return true;
        }

        return false;
    }

    bool compute_descriptors(bool compute_time = true, bool compute_freq = true)
    {
        // Time domain descriptors
        if (!buffer_.empty() && compute_time)
        {
            amp_peak();
            amp_rms();
            amp_variance();
            amp_kurtosis();
            amp_skewness();
            amp_zerocrossing();
        }

        // Frequency domain descriptors
        if (magnitudes_.size() > 2 && compute_freq)
        {
            spectral_centroid();
            spectral_spread();
            spectral_crestfactor();
            spectral_decrease();
            spectral_entropy();
            spectral_flatness();
            spectral_flux();
            spectral_irregularity();
            spectral_kurtosis();
            spectral_peak();
            spectral_rolloff();
            spectral_skweness();
            spectral_slope();
        }

        return true;
    }

    std::vector<TSample> get_magnitudes() const
    {
        return magnitudes_;
    }

    std::vector<TSample> get_previous_magnitudes() const
    {
        return previous_magnitudes_;
    }

    std::vector<TSample> get_buffer() const
    {
        return buffer_;
    }

    TSample get_sample_rate() const
    {
        return sample_rate_;
    }

    TSample get_rolloff_point() const
    {
        return rolloff_point_;
    }

    std::vector<TSample> get_precomputed_frequencies() const
    {
        return precomputed_frequencies_;
    }

    TSample get_time_descriptor(const std::string& descriptor) const
    {
        if (time_descriptors_.find(descriptor) != time_descriptors_.end())
        {
            return time_descriptors_.at(descriptor);
        }

        return static_cast<TSample>(0.0);
    }

    TSample get_frequency_descriptor(const std::string& descriptor) const
    {
        if (frequency_descriptors_.find(descriptor) != frequency_descriptors_.end())
        {
            return frequency_descriptors_.at(descriptor);
        }

        return static_cast<TSample>(0.0);
    }

    std::unordered_map<std::string, TSample> get_time_descriptors() const
    {
        return time_descriptors_;
    }

    std::unordered_map<std::string, TSample> get_frequency_descriptors() const
    {
        return frequency_descriptors_;
    }

    // It is advisable to call compute_descriptors() instead of the single wrapper functions that follow

    // Time domain descriptors
    TSample amp_peak()
    {
        time_descriptors_["peak"] = Amplitude::peak(buffer_);

        return time_descriptors_["peak"];
    }

    TSample amp_rms()
    {
        time_descriptors_["rms"] = Amplitude::rms(buffer_);

        return time_descriptors_["rms"];
    }

    TSample amp_variance()
    {
        time_descriptors_["variance"] = Amplitude::variance(buffer_);

        return time_descriptors_["variance"];
    }

    TSample amp_kurtosis()
    {
        TSample mean = std::accumulate(buffer_.begin(), buffer_.end(),
                                       static_cast<TSample>(0.0)) / static_cast<TSample>(buffer_.size());
        TSample amp_variance = Amplitude::variance(buffer_);

        time_descriptors_["kurtosis"] = Amplitude::kurtosis(buffer_, mean, amp_variance);

        return time_descriptors_["kurtosis"];
    }

    TSample amp_skewness()
    {
        TSample mean = std::accumulate(buffer_.begin(), buffer_.end(),
                                       static_cast<TSample>(0.0)) / static_cast<TSample>(buffer_.size());
        TSample amp_variance = Amplitude::variance(buffer_);

        time_descriptors_["skewness"] = Amplitude::skewness(buffer_, mean, amp_variance);

        return time_descriptors_["skewness"];
    }

    TSample amp_zerocrossing()
    {
        time_descriptors_["zerocrossing"] = Amplitude::zerocrossing(buffer_);

        return time_descriptors_["zerocrossing"];
    }

    // Frequency domain descriptors
    TSample spectral_centroid()
    {
        frequency_descriptors_["centroid"] = Frequency::centroid(magnitudes_, sample_rate_,
                                             precomputed_frequencies_);

        return frequency_descriptors_["centroid"];
    }

    TSample spectral_crestfactor()
    {
        frequency_descriptors_["crestfactor"] = Frequency::crestfactor(magnitudes_);

        return frequency_descriptors_["crestfactor"];
    }

    TSample spectral_decrease()
    {
        frequency_descriptors_["decrease"] = Frequency::decrease(magnitudes_);

        return frequency_descriptors_["decrease"];
    }

    TSample spectral_entropy()
    {
        frequency_descriptors_["entropy"] = Frequency::entropy(magnitudes_);

        return frequency_descriptors_["entropy"];
    }

    TSample spectral_flatness()
    {
        frequency_descriptors_["flatness"] = Frequency::flatness(magnitudes_);

        return frequency_descriptors_["flatness"];
    }

    TSample spectral_flux()
    {
        frequency_descriptors_["flux"] = Frequency::flux(magnitudes_, previous_magnitudes_);

        return frequency_descriptors_["flux"];
    }

    TSample spectral_irregularity()
    {
        frequency_descriptors_["irregularity"] = Frequency::irregularity(magnitudes_);

        return frequency_descriptors_["irregularity"];
    }

    TSample spectral_kurtosis()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(magnitudes_, sample_rate_, precomputed_frequencies_);
        }

        if (frequency_descriptors_.find("spread") != frequency_descriptors_.end())
        {
            Frequency::spread(magnitudes_, sample_rate_, precomputed_frequencies_,
                              frequency_descriptors_["centroid"]);
        }

        frequency_descriptors_["kurtosis"] = Frequency::kurtosis(magnitudes_, sample_rate_,
                                             precomputed_frequencies_, frequency_descriptors_["centroid"],
                                             frequency_descriptors_["spread"]);

        return frequency_descriptors_["kurtosis"];
    }

    TSample spectral_peak()
    {
        frequency_descriptors_["peak"] = Frequency::peak(magnitudes_, sample_rate_,
                                         precomputed_frequencies_);

        return frequency_descriptors_["peak"];
    }

    TSample spectral_rolloff()
    {
        frequency_descriptors_["rolloff"] = Frequency::rolloff(magnitudes_, sample_rate_,
                                            rolloff_point_, precomputed_frequencies_);

        return frequency_descriptors_["rolloff"];
    }

    TSample spectral_skweness()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(magnitudes_, sample_rate_, precomputed_frequencies_);
        }

        if (frequency_descriptors_.find("spread") != frequency_descriptors_.end())
        {
            Frequency::spread(magnitudes_, sample_rate_, precomputed_frequencies_,
                              frequency_descriptors_["centroid"]);
        }

        return Frequency::skweness(magnitudes_, sample_rate_, precomputed_frequencies_,
                                   frequency_descriptors_["centroid"], frequency_descriptors_["spread"]);
    }

    TSample spectral_slope()
    {
        frequency_descriptors_["slope"] = Frequency::slope(magnitudes_, sample_rate_,
                                          precomputed_frequencies_);

        return frequency_descriptors_["slope"];
    }

    TSample spectral_spread()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(magnitudes_, sample_rate_, precomputed_frequencies_);
        }

        frequency_descriptors_["spread"] = Frequency::spread(magnitudes_, sample_rate_,
                                           precomputed_frequencies_, frequency_descriptors_["centroid"]);

        return frequency_descriptors_["spread"];
    }

private:
    std::vector<TSample> precomputed_frequencies_ = {};
    std::vector<TSample> magnitudes_ = {};
    std::vector<TSample> previous_magnitudes_ = {};
    std::vector<TSample> buffer_ = {};
    unsigned int stft_size_ = 0u;
    TSample sample_rate_ = static_cast<TSample>(44100.0);
    TSample rolloff_point_ = static_cast<TSample>(0.85);
    std::unordered_map<std::string, TSample> time_descriptors_;
    std::unordered_map<std::string, TSample> frequency_descriptors_;
};

} // namespace Informer

#endif // INFORMER_H_
