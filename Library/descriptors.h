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
#include <string>
#include <unordered_map>
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

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename TSample>
#endif
std::vector<TSample> precompute_frequencies_(const unsigned int& stft_size = 4096u, const TSample& sample_rate = static_cast<TSample>(44100.0))
{
    std::vector<TSample> precomputed_frequencies = {};

    if (stft_size > 2u)
    {
        TSample fft_bandwidth = static_cast<TSample>(sample_rate) / static_cast<TSample>(stft_size);
        precomputed_frequencies.resize(stft_size / 2u);
        precomputed_frequencies.fill(static_cast<TSample>(0.0));
        for (auto b = 0u; b < stft_size / 2u; b++)
        {
            precomputed_frequencies.at(b) = static_cast<TSample>(b) * fft_bandwidth;
        }
    }

    return precomputed_frequencies;
}

// SPECTRAL CENTROID
template <typename Container, typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<typename Container::value_type> &&
std::floating_point<TSample> &&
std::same_as<typename Container::value_type, TSample>
#endif
typename Container::value_type centroid(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {})
{
    TSample centroid = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return centroid;
    }

    unsigned int fft_size = stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
            h += p * std::log2(p);
        }
    }

    h /= std::log2(static_cast<TSample>(fft_size));
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
typename Container::value_type kurtosis(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {}, const TSample& spectral_centroid = static_cast<TSample>(-1.0), const TSample& spectral_spread = static_cast<TSample>(-1.0))
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
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
typename Container::value_type peak(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {})
{
    TSample peak = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return peak;
    }

    unsigned int fft_size = stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
typename Container::value_type rolloff(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), const TSample& rolloff_point = static_cast<TSample>(0.85), std::vector<TSample>& precomputed_frequencies = {})
{
    if (stft.size() < 2)
    {
        return static_cast<TSample>(0.0);
    }

    unsigned int fft_size = stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
typename Container::value_type skweness(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {}, const TSample& spectral_centroid = static_cast<TSample>(-1.0), const TSample& spectral_spread = static_cast<TSample>(-1.0))
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
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
typename Container::value_type slope(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {})
{
    TSample sslope = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return sslope;
    }

    unsigned int fft_size = stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
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
typename Container::value_type spread(const Container& stft, const TSample& sample_rate = static_cast<TSample>(44100.0), std::vector<TSample>& precomputed_frequencies = {}, TSample centroid = static_cast<TSample>(-1.0))
{
    TSample sspread = static_cast<TSample>(0.0);

    if (stft.size() < 2)
    {
        return sspread;
    }

    unsigned int fft_size = stft.size() / 2u;

    if (precomputed_frequencies.size() < fft_size)
    {
        precomputed_frequencies = precompute_frequencies_(stft.size(), sample_rate);
    }

    if (centroid < static_cast<TSample>(0.0))
    {
        centroid = centroid(stft, sample_rate, precomputed_frequencies);
    }

    TSample numerator = static_cast<TSample>(0.0);
    TSample magn_sum = static_cast<TSample>(0.0);

    for (unsigned int k = 0u; k < fft_size; k++)
    {
        numerator += (precomputed_frequencies.at(k) - centroid) * (precomputed_frequencies.at(k) - centroid) * std::abs(stft.at(k));
        magn_sum += std::abs(stft.at(k));
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
             const std::vector<TSample>& stft = {},
             const TSample& sample_rate = static_cast<TSample>(44100.0),
             const TSample& rolloff_point = static_cast<TSample>(0.85),
             const std::vector<TSample>& previous_stft = {},
             const bool& compute = true)
    {
        set_buffer(buffer);
        set_sample_rate(sample_rate);
        set_stft(stft);
        if (!previous_stft.empty())
        {
            set_previous_stft(previous_stft);
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

            if (stft_.size() > 2)
            {
                precompute_frequencies_();
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

    bool set_stft(const std::vector<TSample>& stft)
    {
        if (stft.size() > 2)
        {
            if (!stft_.empty())
            {
                previous_stft_ = stft_;
            }
            else
            {
                previous_stft_.resize(stft.size(), static_cast<TSample>(0.0));
            }

            if (stft.size() != stft_.size() || precomputed_frequencies_.empty())
            {
                stft_ = stft;
                precompute_frequencies_();
            }
            else
            {
                stft_ = stft;
            }

            return true;
        }

        return false;
    }

    bool set_previous_stft(const std::vector<TSample>& previous_stft)
    {
        if (previous_stft.size() == stft_.size())
        {
            previous_stft_ = previous_stft;

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
        if (stft_.size() > 2 && compute_freq)
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

    std::vector<TSample> get_stft() const
    {
        return stft_;
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
        frequency_descriptors_["centroid"] = Frequency::centroid(stft_, sample_rate_,
                                             precomputed_frequencies_);

        return frequency_descriptors_["centroid"];
    }

    TSample spectral_crestfactor()
    {
        frequency_descriptors_["crestfactor"] = Frequency::crestfactor(stft_);

        return frequency_descriptors_["crestfactor"];
    }

    TSample spectral_decrease()
    {
        frequency_descriptors_["decrease"] = Frequency::decrease(stft_);

        return frequency_descriptors_["decrease"];
    }

    TSample spectral_entropy()
    {
        frequency_descriptors_["entropy"] = Frequency::entropy(stft_);

        return frequency_descriptors_["entropy"];
    }

    TSample spectral_flatness()
    {
        frequency_descriptors_["flatness"] = Frequency::flatness(stft_);

        return frequency_descriptors_["flatness"];
    }

    TSample spectral_flux()
    {
        frequency_descriptors_["flux"] = Frequency::flux(stft_, previous_stft_);

        return frequency_descriptors_["flux"];
    }

    TSample spectral_irregularity()
    {
        frequency_descriptors_["irregularity"] = Frequency::irregularity(stft_);

        return frequency_descriptors_["irregularity"];
    }

    TSample spectral_kurtosis()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(stft_, sample_rate_, precomputed_frequencies_);
        }

        if (frequency_descriptors_.find("spread") != frequency_descriptors_.end())
        {
            Frequency::spread(stft_, sample_rate_, precomputed_frequencies_,
                              frequency_descriptors_["centroid"]);
        }

        frequency_descriptors_["kurtosis"] = Frequency::kurtosis(stft_, sample_rate_,
                                             precomputed_frequencies_, frequency_descriptors_["centroid"],
                                             frequency_descriptors_["spread"]);

        return frequency_descriptors_["kurtosis"];
    }

    TSample spectral_peak()
    {
        frequency_descriptors_["peak"] = Frequency::peak(stft_, sample_rate_,
                                         precomputed_frequencies_);

        return frequency_descriptors_["peak"];
    }

    TSample spectral_rolloff()
    {
        frequency_descriptors_["rolloff"] = Frequency::rolloff(stft_, sample_rate_,
                                            rolloff_point_, precomputed_frequencies_);

        return frequency_descriptors_["rolloff"];
    }

    TSample spectral_skweness()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(stft_, sample_rate_, precomputed_frequencies_);
        }

        if (frequency_descriptors_.find("spread") != frequency_descriptors_.end())
        {
            Frequency::spread(stft_, sample_rate_, precomputed_frequencies_,
                              frequency_descriptors_["centroid"]);
        }

        return Frequency::skweness(stft_, sample_rate_, precomputed_frequencies_,
                                   frequency_descriptors_["centroid"], frequency_descriptors_["spread"]);
    }

    TSample spectral_slope()
    {
        frequency_descriptors_["slope"] = Frequency::slope(stft_, sample_rate_,
                                          precomputed_frequencies_);

        return frequency_descriptors_["slope"];
    }

    TSample spectral_spread()
    {
        if (frequency_descriptors_.find("centroid") != frequency_descriptors_.end())
        {
            Frequency::centroid(stft_, sample_rate_, precomputed_frequencies_);
        }

        frequency_descriptors_["spread"] = Frequency::spread(stft_, sample_rate_,
                                           precomputed_frequencies_, frequency_descriptors_["centroid"]);

        return frequency_descriptors_["spread"];
    }

private:
    std::vector<TSample> precomputed_frequencies_ = {};
    std::vector<TSample> stft_ = {};
    std::vector<TSample> previous_stft_ = {};
    std::vector<TSample> buffer_ = {};
    TSample sample_rate_ = static_cast<TSample>(44100.0);
    TSample rolloff_point_ = static_cast<TSample>(0.85);
    std::unordered_map<std::string, TSample> time_descriptors_;
    std::unordered_map<std::string, TSample> frequency_descriptors_;

    inline void precompute_frequencies_()
    {
        if (stft_.size() < 2)
        {
            return;
        }

        TSample fft_bandwidth = static_cast<TSample>(sample_rate_)
                                / static_cast<TSample>(stft_.size());
        precomputed_frequencies_.resize(stft_.size());
        precomputed_frequencies_.fill(static_cast<TSample>(0.0));
        for (auto b = 0u; b < stft_.size() / 2u; b++)
        {
            precomputed_frequencies_.at(b) = static_cast<TSample>(b) * fft_bandwidth;
        }
    }
};

} // namespace Informer

#endif // DESCRIPTORS_H_
