#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../../Library/informer.h"

namespace py = pybind11;

PYBIND11_MODULE(pyinformer, m)
{
    m.doc() = "Python bindings for Informer audio analysis library";

    // Bind the Amplitude namespace functions
    py::module_ amplitude = m.def_submodule("amplitude", "Time domain amplitude descriptors");

    amplitude.def("peak", &Informer::Amplitude::peak<std::vector<float>>,
                  "Calculate peak amplitude", py::arg("buffer"));
    amplitude.def("peak", &Informer::Amplitude::peak<std::vector<double>>,
                  "Calculate peak amplitude", py::arg("buffer"));

    amplitude.def("rms", &Informer::Amplitude::rms<std::vector<float>>,
                  "Calculate RMS amplitude", py::arg("buffer"));
    amplitude.def("rms", &Informer::Amplitude::rms<std::vector<double>>,
                  "Calculate RMS amplitude", py::arg("buffer"));

    amplitude.def("variance", &Informer::Amplitude::variance<std::vector<float>>,
                  "Calculate amplitude variance", py::arg("buffer"));
    amplitude.def("variance", &Informer::Amplitude::variance<std::vector<double>>,
                  "Calculate amplitude variance", py::arg("buffer"));

    amplitude.def("kurtosis", &Informer::Amplitude::kurtosis<std::vector<float>>,
                  "Calculate amplitude kurtosis",
                  py::arg("buffer"), py::arg("mean") = -10000.0f, py::arg("variance") = -10000.0f);
    amplitude.def("kurtosis", &Informer::Amplitude::kurtosis<std::vector<double>>,
                  "Calculate amplitude kurtosis",
                  py::arg("buffer"), py::arg("mean") = -10000.0, py::arg("variance") = -10000.0);

    amplitude.def("skewness", &Informer::Amplitude::skewness<std::vector<float>>,
                  "Calculate amplitude skewness",
                  py::arg("buffer"), py::arg("mean") = -10000.0f, py::arg("variance") = -10000.0f);
    amplitude.def("skewness", &Informer::Amplitude::skewness<std::vector<double>>,
                  "Calculate amplitude skewness",
                  py::arg("buffer"), py::arg("mean") = -10000.0, py::arg("variance") = -10000.0);

    amplitude.def("zerocrossing", &Informer::Amplitude::zerocrossing<std::vector<float>>,
                  "Calculate zero crossing rate", py::arg("buffer"));
    amplitude.def("zerocrossing", &Informer::Amplitude::zerocrossing<std::vector<double>>,
                  "Calculate zero crossing rate", py::arg("buffer"));

    // Bind the Frequency namespace functions
    py::module_ frequency = m.def_submodule("frequency", "Frequency domain spectral descriptors");

    frequency.def("precompute_frequencies",
                  py::overload_cast<int, float>(
                      &Informer::Frequency::precompute_frequencies<int, float>),
                  "Precompute frequency bins",
                  py::arg("stft_size") = 4096, py::arg("sample_rate") = 44100.0f);
    frequency.def("precompute_frequencies",
                  py::overload_cast<int, double>(
                      &Informer::Frequency::precompute_frequencies<int, double>),
                  "Precompute frequency bins",
                  py::arg("stft_size") = 4096, py::arg("sample_rate") = 44100.0);

    frequency.def("magnitudes", &Informer::Frequency::magnitudes<std::vector<float>>,
                  "Calculate magnitudes from FFT", py::arg("stft"));
    frequency.def("magnitudes", &Informer::Frequency::magnitudes<std::vector<double>>,
                  "Calculate magnitudes from FFT", py::arg("stft"));

    frequency.def("window", &Informer::Frequency::window<std::vector<float>>,
                  "Apply window function", py::arg("buffer"), py::arg("hann_window") = true);
    frequency.def("window", &Informer::Frequency::window<std::vector<double>>,
                  "Apply window function", py::arg("buffer"), py::arg("hann_window") = true);

    frequency.def("centroid", [](const std::vector<float>& magnitudes,
                                 float sample_rate,
                                 std::vector<float> precomputed_frequencies,
                                 unsigned int stft_size)
    {
        return Informer::Frequency::centroid(magnitudes, sample_rate, precomputed_frequencies, stft_size);
    }, "Calculate spectral centroid",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
    py::arg("precomputed_frequencies") = std::vector<float> {}, py::arg("stft_size") = 0u);

    frequency.def("centroid", [](const std::vector<double>& magnitudes,
                                 double sample_rate,
                                 std::vector<double> precomputed_frequencies,
                                 unsigned int stft_size)
    {
        return Informer::Frequency::centroid(magnitudes, sample_rate, precomputed_frequencies, stft_size);
    }, "Calculate spectral centroid",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
    py::arg("precomputed_frequencies") = std::vector<double> {}, py::arg("stft_size") = 0u);

    frequency.def("spread", [](const std::vector<float>& magnitudes,
                               float sample_rate,
                               std::vector<float> precomputed_frequencies,
                               float centroid = -10000.0f,
                               unsigned int stft_size = 0u)
    {
        return Informer::Frequency::spread(magnitudes, sample_rate, precomputed_frequencies, centroid, stft_size);
    }, "Calculate spectral spread",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
    py::arg("precomputed_frequencies") = std::vector<float> {},
    py::arg("centroid") = -10000.0f, py::arg("stft_size") = 0u);
    frequency.def("spread", [](const std::vector<double>& magnitudes,
                               double sample_rate,
                               std::vector<double> precomputed_frequencies,
                               double centroid = -10000.0,
                               unsigned int stft_size = 0u)
    {
        return Informer::Frequency::spread(magnitudes, sample_rate, precomputed_frequencies, centroid, stft_size);
    }, "Calculate spectral spread",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
    py::arg("precomputed_frequencies") = std::vector<double> {},
    py::arg("centroid") = -10000.0, py::arg("stft_size") = 0u);

    frequency.def("crestfactor", &Informer::Frequency::crestfactor<std::vector<float>>,
                  "Calculate spectral crest factor", py::arg("magnitudes"));
    frequency.def("crestfactor", &Informer::Frequency::crestfactor<std::vector<double>>,
                  "Calculate spectral crest factor", py::arg("magnitudes"));
    frequency.def("skewness", &Informer::Frequency::skewness<std::vector<float>>,
                  "Calculate spectral skewness",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
                  py::arg("precomputed_frequencies") = std::vector<float> {},
                  py::arg("centroid") = -10000.0f, py::arg("spread") = -10000.0f,
                  py::arg("stft_size") = 0u);
    frequency.def("skewness", &Informer::Frequency::skewness<std::vector<double>>,
                  "Calculate spectral skewness",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
                  py::arg("precomputed_frequencies") = std::vector<double> {},
                  py::arg("centroid") = -10000.0, py::arg("spread") = -10000.0,
                  py::arg("stft_size") = 0u);
    frequency.def("slope", &Informer::Frequency::slope<std::vector<float>>,
                  "Calculate spectral slope",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
                  py::arg("precomputed_frequencies") = std::vector<float> {},
                  py::arg("stft_size") = 0u);
    frequency.def("slope", &Informer::Frequency::slope<std::vector<double>>,
                  "Calculate spectral slope",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
                  py::arg("precomputed_frequencies") = std::vector<double> {},
                  py::arg("stft_size") = 0u);
    frequency.def("decrease", &Informer::Frequency::decrease<std::vector<float>>,
                  "Calculate spectral decrease", py::arg("magnitudes"));
    frequency.def("decrease", &Informer::Frequency::decrease<std::vector<double>>,
                  "Calculate spectral decrease", py::arg("magnitudes"));
    frequency.def("entropy", &Informer::Frequency::entropy<std::vector<float>>,
                  "Calculate spectral entropy", py::arg("magnitudes"));
    frequency.def("entropy", &Informer::Frequency::entropy<std::vector<double>>,
                  "Calculate spectral entropy", py::arg("magnitudes"));
    frequency.def("flatness", &Informer::Frequency::flatness<std::vector<float>>,
                  "Calculate spectral flatness", py::arg("magnitudes"));
    frequency.def("flatness", &Informer::Frequency::flatness<std::vector<double>>,
                  "Calculate spectral flatness", py::arg("magnitudes"));
    frequency.def("flux", &Informer::Frequency::flux<std::vector<float>>,
                  "Calculate spectral flux", py::arg("magnitudes"), py::arg("previous_magnitudes"));
    frequency.def("flux", &Informer::Frequency::flux<std::vector<double>>,
                  "Calculate spectral flux", py::arg("magnitudes"), py::arg("previous_magnitudes"));
    frequency.def("irregularity", &Informer::Frequency::irregularity<std::vector<float>>,
                  "Calculate spectral irregularity", py::arg("magnitudes"));
    frequency.def("irregularity", &Informer::Frequency::irregularity<std::vector<double>>,
                  "Calculate spectral irregularity", py::arg("magnitudes"));
    frequency.def("kurtosis", &Informer::Frequency::kurtosis<std::vector<float>>,
                  "Calculate spectral kurtosis",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
                  py::arg("precomputed_frequencies") = std::vector<float> {},
                  py::arg("centroid") = -10000.0f, py::arg("spread") = -10000.0f,
                  py::arg("stft_size") = 0u);
    frequency.def("kurtosis", &Informer::Frequency::kurtosis<std::vector<double>>,
                  "Calculate spectral kurtosis",
                  py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
                  py::arg("precomputed_frequencies") = std::vector<double> {},
                  py::arg("centroid") = -10000.0, py::arg("spread") = -10000.0,
                  py::arg("stft_size") = 0u);

    frequency.def("peak", [](const std::vector<float>& magnitudes,
                             float sample_rate,
                             std::vector<float> precomputed_frequencies,
                             unsigned int stft_size)
    {
        return Informer::Frequency::peak(magnitudes, sample_rate, precomputed_frequencies, stft_size);
    }, "Find spectral peak frequency",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f,
    py::arg("precomputed_frequencies") = std::vector<float> {}, py::arg("stft_size") = 0u);
    frequency.def("peak", [](const std::vector<double>& magnitudes,
                             double sample_rate,
                             std::vector<double> precomputed_frequencies,
                             unsigned int stft_size)
    {
        return Informer::Frequency::peak(magnitudes, sample_rate, precomputed_frequencies, stft_size);
    }, "Find spectral peak frequency",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0,
    py::arg("precomputed_frequencies") = std::vector<double> {}, py::arg("stft_size") = 0u);

    frequency.def("rolloff", [](const std::vector<float>& magnitudes,
                                float sample_rate,
                                float rolloff_point,
                                std::vector<float> precomputed_frequencies,
                                unsigned int stft_size)
    {
        return Informer::Frequency::rolloff(magnitudes, sample_rate, rolloff_point,
                                            precomputed_frequencies, stft_size);
    }, "Calculate spectral rolloff",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0f, py::arg("rolloff_point") = 0.85f,
    py::arg("precomputed_frequencies") = std::vector<float> {}, py::arg("stft_size") = 0u);
    frequency.def("rolloff", [](const std::vector<double>& magnitudes,
                                double sample_rate,
                                double rolloff_point,
                                std::vector<double> precomputed_frequencies,
                                unsigned int stft_size)
    {
        return Informer::Frequency::rolloff(magnitudes, sample_rate, rolloff_point,
                                            precomputed_frequencies, stft_size);
    }, "Calculate spectral rolloff",
    py::arg("magnitudes"), py::arg("sample_rate") = 44100.0, py::arg("rolloff_point") = 0.85,
    py::arg("precomputed_frequencies") = std::vector<double> {}, py::arg("stft_size") = 0u);


    // Bind the main Informer class for float and double
    py::class_<Informer::Informer<float>>(m, "InformerFloat")
    .def(py::init<const std::vector<float>&, const std::vector<float>&, const float&,
         const float&, const std::vector<float>&, const unsigned int&, const bool&>(),
         py::arg("buffer") = std::vector<float> {},
         py::arg("magnitudes") = std::vector<float> {},
         py::arg("sample_rate") = 44100.0f,
         py::arg("rolloff_point") = 0.85f,
         py::arg("previous_magnitudes") = std::vector<float> {},
         py::arg("stft_size") = 0u,
         py::arg("compute") = true)
    .def("set_sample_rate", &Informer::Informer<float>::set_sample_rate)
    .def("set_rolloff_point", &Informer::Informer<float>::set_rolloff_point)
    .def("set_stft_size", &Informer::Informer<float>::set_stft_size)
    .def("set_buffer", &Informer::Informer<float>::set_buffer)
    .def("set_magnitudes", &Informer::Informer<float>::set_magnitudes,
         py::arg("magnitudes"), py::arg("update_stft_size") = false)
    .def("set_magnitudes_from_stft", &Informer::Informer<float>::set_magnitudes_from_stft)
    .def("compute_descriptors", &Informer::Informer<float>::compute_descriptors,
         py::arg("compute_time") = true, py::arg("compute_freq") = true)
    .def("get_buffer", &Informer::Informer<float>::get_buffer)
    .def("get_magnitudes", &Informer::Informer<float>::get_magnitudes)
    .def("get_sample_rate", &Informer::Informer<float>::get_sample_rate)
    .def("get_stft_size", &Informer::Informer<float>::get_stft_size)
    .def("get_time_descriptor", &Informer::Informer<float>::get_time_descriptor)
    .def("get_frequency_descriptor", &Informer::Informer<float>::get_frequency_descriptor)
    .def("get_time_descriptors", &Informer::Informer<float>::get_time_descriptors)
    .def("get_frequency_descriptors", &Informer::Informer<float>::get_frequency_descriptors)
    // Individual descriptor methods
    .def("amp_peak", &Informer::Informer<float>::amp_peak)
    .def("amp_rms", &Informer::Informer<float>::amp_rms)
    .def("amp_variance", &Informer::Informer<float>::amp_variance)
    .def("spectral_centroid", &Informer::Informer<float>::spectral_centroid)
    .def("spectral_flatness", &Informer::Informer<float>::spectral_flatness)
    .def("spectral_peak", &Informer::Informer<float>::spectral_peak);

    py::class_<Informer::Informer<double>>(m, "InformerDouble")
    .def(py::init<const std::vector<double>&, const std::vector<double>&, const double&,
         const double&, const std::vector<double>&, const unsigned int&, const bool&>(),
         py::arg("buffer") = std::vector<double> {},
         py::arg("magnitudes") = std::vector<double> {},
         py::arg("sample_rate") = 44100.0,
         py::arg("rolloff_point") = 0.85,
         py::arg("previous_magnitudes") = std::vector<double> {},
         py::arg("stft_size") = 0u,
         py::arg("compute") = true)
    .def("set_sample_rate", &Informer::Informer<double>::set_sample_rate)
    .def("set_rolloff_point", &Informer::Informer<double>::set_rolloff_point)
    .def("set_stft_size", &Informer::Informer<double>::set_stft_size)
    .def("set_buffer", &Informer::Informer<double>::set_buffer)
    .def("set_magnitudes", &Informer::Informer<double>::set_magnitudes,
         py::arg("magnitudes"), py::arg("update_stft_size") = false)
    .def("set_magnitudes_from_stft", &Informer::Informer<double>::set_magnitudes_from_stft)
    .def("compute_descriptors", &Informer::Informer<double>::compute_descriptors,
         py::arg("compute_time") = true, py::arg("compute_freq") = true)
    .def("get_buffer", &Informer::Informer<double>::get_buffer)
    .def("get_magnitudes", &Informer::Informer<double>::get_magnitudes)
    .def("get_sample_rate", &Informer::Informer<double>::get_sample_rate)
    .def("get_stft_size", &Informer::Informer<double>::get_stft_size)
    .def("get_time_descriptor", &Informer::Informer<double>::get_time_descriptor)
    .def("get_frequency_descriptor", &Informer::Informer<double>::get_frequency_descriptor)
    .def("get_time_descriptors", &Informer::Informer<double>::get_time_descriptors)
    .def("get_frequency_descriptors", &Informer::Informer<double>::get_frequency_descriptors)
    // Individual descriptor methods
    .def("amp_peak", &Informer::Informer<double>::amp_peak)
    .def("amp_rms", &Informer::Informer<double>::amp_rms)
    .def("amp_variance", &Informer::Informer<double>::amp_variance)
    .def("spectral_centroid", &Informer::Informer<double>::spectral_centroid)
    .def("spectral_flatness", &Informer::Informer<double>::spectral_flatness)
    .def("spectral_peak", &Informer::Informer<double>::spectral_peak);

    // Create aliases for convenience
    m.attr("Informer") = m.attr("InformerFloat");  // Default to float version
}