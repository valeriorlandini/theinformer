#include "../Library/informer.h"
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;
using namespace Informer;

// Helper function to convert JavaScript arrays to std::vector
template<typename T>
std::vector<T> vecFromJSArray(const val& jsArray) {
    unsigned int length = jsArray["length"].as<unsigned int>();
    std::vector<T> vec(length);
    for (unsigned int i = 0; i < length; ++i) {
        vec[i] = jsArray[i].as<T>();
    }
    return vec;
}

// Helper function to convert std::vector to JavaScript arrays
template<typename T>
val vecToJSArray(const std::vector<T>& vec) {
    val jsArray = val::array();
    for (size_t i = 0; i < vec.size(); ++i) {
        jsArray.set(i, vec[i]);
    }
    return jsArray;
}

// Wrapper functions for Amplitude namespace
namespace AmplitudeWrapper {
    double peak(const val& jsBuffer) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::peak(buffer);
    }
    
    double rms(const val& jsBuffer) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::rms(buffer);
    }
    
    double variance(const val& jsBuffer) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::variance(buffer);
    }
    
    double kurtosis(const val& jsBuffer, double mean, double variance) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::kurtosis(buffer, mean, variance);
    }
    
    double skewness(const val& jsBuffer, double mean, double variance) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::skewness(buffer, mean, variance);
    }
    
    double zerocrossing(const val& jsBuffer) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        return Amplitude::zerocrossing(buffer);
    }
}

// Wrapper functions for Frequency namespace
namespace FrequencyWrapper {
    val precompute_frequencies(int stft_size, double sample_rate) {
        auto freqs = Frequency::precompute_frequencies(stft_size, sample_rate);
        return vecToJSArray(freqs);
    }
    
    val magnitudes(const val& jsStft) {
        auto stft = vecFromJSArray<double>(jsStft);
        auto mags = Frequency::magnitudes(stft);
        return vecToJSArray(mags);
    }
    
    val window(const val& jsBuffer, bool hann_window) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        auto windowed = Frequency::window(buffer, hann_window);
        return vecToJSArray(windowed);
    }
    
    double centroid(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::centroid(magnitudes, sample_rate, precomp_freqs);
    }
    
    double spread(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::spread(magnitudes, sample_rate, precomp_freqs);
    }
    
    double crestfactor(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        return Frequency::crestfactor(magnitudes);
    }
    
    double decrease(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        return Frequency::decrease(magnitudes);
    }
    
    double entropy(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        return Frequency::entropy(magnitudes);
    }
    
    double flatness(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        return Frequency::flatness(magnitudes);
    }
    
    double flux(const val& jsMagnitudes, const val& jsPrevMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        auto prevMagnitudes = vecFromJSArray<double>(jsPrevMagnitudes);
        return Frequency::flux(magnitudes, prevMagnitudes);
    }
    
    double irregularity(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        return Frequency::irregularity(magnitudes);
    }
    
    double kurtosis(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::kurtosis(magnitudes, sample_rate, precomp_freqs);
    }
    
    double peak(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::peak(magnitudes, sample_rate, precomp_freqs);
    }
    
    double rolloff(const val& jsMagnitudes, double sample_rate, double rolloff_point) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::rolloff(magnitudes, sample_rate, rolloff_point, precomp_freqs);
    }
    
    double skewness(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::skewness(magnitudes, sample_rate, precomp_freqs);
    }
    
    double slope(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::slope(magnitudes, sample_rate, precomp_freqs);
    }
    
    double f0_hps(const val& jsMagnitudes, double sample_rate) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        std::vector<double> precomp_freqs;
        return Frequency::f0_hps(magnitudes, 5u, sample_rate, precomp_freqs);
    }

}

// Wrapper class for Informer
class InformerWrapper {
public:
    InformerWrapper(double sample_rate = 44100.0, double rolloff_point = 0.85, unsigned int stft_size = 4096)
        : informer_(std::vector<double>(), std::vector<double>(), sample_rate, rolloff_point, 
                   std::vector<double>(), stft_size, false) {}
    
    void setBuffer(const val& jsBuffer) {
        auto buffer = vecFromJSArray<double>(jsBuffer);
        informer_.set_buffer(buffer);
    }
    
    void setMagnitudes(const val& jsMagnitudes) {
        auto magnitudes = vecFromJSArray<double>(jsMagnitudes);
        informer_.set_magnitudes(magnitudes, true);
    }
    
    void setMagnitudesFromStft(const val& jsStft) {
        auto stft = vecFromJSArray<double>(jsStft);
        informer_.set_magnitudes_from_stft(stft);
    }
    
    void setPreviousMagnitudes(const val& jsPrevMagnitudes) {
        auto prevMagnitudes = vecFromJSArray<double>(jsPrevMagnitudes);
        informer_.set_previous_magnitudes(prevMagnitudes);
    }
    
    void setSampleRate(double sample_rate) {
        informer_.set_sample_rate(sample_rate);
    }
    
    void setRolloffPoint(double rolloff) {
        informer_.set_rolloff_point(rolloff);
    }
    
    void setStftSize(unsigned int stft_size) {
        informer_.set_stft_size(stft_size);
    }
    
    void computeDescriptors(bool compute_time = true, bool compute_freq = true) {
        informer_.compute_descriptors(compute_time, compute_freq);
    }
    
    double getTimeDescriptor(const std::string& descriptor) const {
        return informer_.get_time_descriptor(descriptor);
    }
    
    double getFrequencyDescriptor(const std::string& descriptor) const {
        return informer_.get_frequency_descriptor(descriptor);
    }
    
    val getBuffer() const {
        return vecToJSArray(informer_.get_buffer());
    }
    
    val getMagnitudes() const {
        return vecToJSArray(informer_.get_magnitudes());
    }
    
    val getPreviousMagnitudes() const {
        return vecToJSArray(informer_.get_previous_magnitudes());
    }
    
    val getPrecomputedFrequencies() const {
        return vecToJSArray(informer_.get_precomputed_frequencies());
    }
    
    double getSampleRate() const {
        return informer_.get_sample_rate();
    }
    
    double getRolloffPoint() const {
        return informer_.get_rolloff_point();
    }
    
    unsigned int getStftSize() const {
        return informer_.get_stft_size();
    }
    
    val getTimeDescriptors() const {
        auto descriptors = informer_.get_time_descriptors();
        val obj = val::object();
        for (const auto& pair : descriptors) {
            obj.set(pair.first, pair.second);
        }
        return obj;
    }
    
    val getFrequencyDescriptors() const {
        auto descriptors = informer_.get_frequency_descriptors();
        val obj = val::object();
        for (const auto& pair : descriptors) {
            obj.set(pair.first, pair.second);
        }
        return obj;
    }

private:
    ::Informer::Informer<double> informer_;
};

// Embind declarations
EMSCRIPTEN_BINDINGS(informer_module) {
    // Amplitude namespace functions
    function("amplitude_peak", &AmplitudeWrapper::peak);
    function("amplitude_rms", &AmplitudeWrapper::rms);
    function("amplitude_variance", &AmplitudeWrapper::variance);
    function("amplitude_kurtosis", &AmplitudeWrapper::kurtosis);
    function("amplitude_skewness", &AmplitudeWrapper::skewness);
    function("amplitude_zerocrossing", &AmplitudeWrapper::zerocrossing);
    
    // Frequency namespace functions
    function("frequency_precompute_frequencies", &FrequencyWrapper::precompute_frequencies);
    function("frequency_magnitudes", &FrequencyWrapper::magnitudes);
    function("frequency_window", &FrequencyWrapper::window);
    function("frequency_centroid", &FrequencyWrapper::centroid);
    function("frequency_spread", &FrequencyWrapper::spread);
    function("frequency_crestfactor", &FrequencyWrapper::crestfactor);
    function("frequency_decrease", &FrequencyWrapper::decrease);
    function("frequency_entropy", &FrequencyWrapper::entropy);
    function("frequency_flatness", &FrequencyWrapper::flatness);
    function("frequency_flux", &FrequencyWrapper::flux);
    function("frequency_irregularity", &FrequencyWrapper::irregularity);
    function("frequency_kurtosis", &FrequencyWrapper::kurtosis);
    function("frequency_peak", &FrequencyWrapper::peak);
    function("frequency_rolloff", &FrequencyWrapper::rolloff);
    function("frequency_skewness", &FrequencyWrapper::skewness);
    function("frequency_slope", &FrequencyWrapper::slope);
    function("frequency_f0", &FrequencyWrapper::f0_hps);
    
    // Informer class
    class_<InformerWrapper>("Informer")
        .constructor<double, double, unsigned int>()
        .constructor<>()
        .function("setBuffer", &InformerWrapper::setBuffer)
        .function("setMagnitudes", &InformerWrapper::setMagnitudes)
        .function("setMagnitudesFromStft", &InformerWrapper::setMagnitudesFromStft)
        .function("setPreviousMagnitudes", &InformerWrapper::setPreviousMagnitudes)
        .function("setSampleRate", &InformerWrapper::setSampleRate)
        .function("setRolloffPoint", &InformerWrapper::setRolloffPoint)
        .function("setStftSize", &InformerWrapper::setStftSize)
        .function("computeDescriptors", &InformerWrapper::computeDescriptors)
        .function("getTimeDescriptor", &InformerWrapper::getTimeDescriptor)
        .function("getFrequencyDescriptor", &InformerWrapper::getFrequencyDescriptor)
        .function("getBuffer", &InformerWrapper::getBuffer)
        .function("getMagnitudes", &InformerWrapper::getMagnitudes)
        .function("getPreviousMagnitudes", &InformerWrapper::getPreviousMagnitudes)
        .function("getPrecomputedFrequencies", &InformerWrapper::getPrecomputedFrequencies)
        .function("getSampleRate", &InformerWrapper::getSampleRate)
        .function("getRolloffPoint", &InformerWrapper::getRolloffPoint)
        .function("getStftSize", &InformerWrapper::getStftSize)
        .function("getTimeDescriptors", &InformerWrapper::getTimeDescriptors)
        .function("getFrequencyDescriptors", &InformerWrapper::getFrequencyDescriptors);
}