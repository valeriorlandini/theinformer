/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../../../../Library/informer.h"

using namespace c74::min;

class freqdesc_tilde : public object<freqdesc_tilde>, public sample_operator<2, 1>
{
public:
	MIN_DESCRIPTION {"Compute the selected frequency domain descriptor from magnitude spectrum input and returns it as a signal"};
	MIN_TAGS {"audio, analysis, spectral descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.timedesc, iformer.freqdesc"};

	inlet<>  in_m {this, "(signal) Magntiude spectrum input"};
	inlet<>  in_b {this, "(signal) Bin"};
	outlet<> out {this, "Computed descriptor", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			informer_.set_sample_rate(args[0]);
			magnitudes_.resize(stft_size_);
			return {};
		}
	};

	enum class f_descriptors : int { centroid, crestfactor, decrease, entropy, flatness, flux, irregularity, kurtosis, peak, rolloff, skewness, slope, spread, enum_count };

    enum_map f_descriptors_range = {"centroid", "crestfactor", "decrease", "entropy", "flatness", "flux", "irregularity", "kurtosis", "peak", "rolloff", "skewness", "slope", "spread"};

	argument<symbol> descriptor_arg
	{
		this,
		"descriptor",
		"Frequency domain descriptor to output.",
        MIN_ARGUMENT_FUNCTION
		{
			if (arg == "centroid")
			{
				descriptor = f_descriptors::centroid;
			}
			if (arg == "crestfactor")
			{
				descriptor = f_descriptors::crestfactor;
			}
			if (arg == "decrease")
			{
				descriptor = f_descriptors::decrease;
			}
			if (arg == "entropy")
			{
				descriptor = f_descriptors::entropy;
			}
			if (arg == "flatness")
			{
				descriptor = f_descriptors::flatness;
			}
			if (arg == "flux")
			{
				descriptor = f_descriptors::flux;
			}
			if (arg == "irregularity")
			{
				descriptor = f_descriptors::irregularity;
			}
			if (arg == "kurtosis")
			{
				descriptor = f_descriptors::kurtosis;
			}
			if (arg == "peak")
			{
				descriptor = f_descriptors::peak;
			}
			if (arg == "rolloff")
			{
				descriptor = f_descriptors::rolloff;
			}
			if (arg == "skewness")
			{
				descriptor = f_descriptors::skewness;
			}
			if (arg == "slope")
			{
				descriptor = f_descriptors::slope;
			}
			if (arg == "spread")
			{
				descriptor = f_descriptors::spread;
			}
		}
    };

	argument<int> spectral_frame_size_arg
    {
        this,
        "frame_size",
        "Size of the spectral frame (number of bins).",
		MIN_ARGUMENT_FUNCTION
		{
            spectral_frame_size = arg;
        }
    };

	attribute<f_descriptors> descriptor
	{
        this,
        "descriptor",
        f_descriptors::centroid,
		f_descriptors_range,
        title {"Descriptor"},
		description {"Descriptor to output."},
		setter
		{
			MIN_FUNCTION
			{
				int index = static_cast<int>(args[0]);
                selected_descriptor_name_ = f_descriptors_range[index];
				return args;
			}
		}
    };

	attribute<bool> dict
	{
        this,
        "dict",
        false,
        title {"Output for dict"},
        description {"When activated, the output messages are formatted so that they can be directly sent to a dictionary."}
    };

	attribute<bool> normalize
	{
        this,
        "normalize",
        false,
        title {"Normalize descriptor"},
        description {"When activated, the computed descriptor is normalized inside the [0, 1] range. Consider that some normalizations are performed on euristic bases, use only for artistic purposes."}
    };

	attribute<int, threadsafe::no, limit::clamp> spectral_frame_size
	{
		this,
		"frame_size",
		4096,
		range { 2, 65536 },
		title {"Frame Size (bins)"},
		description {"Size of the spectral frame (number of bins)."},
		setter
		{
			MIN_FUNCTION
			{
				stft_size_ = int(args[0]);
				magnitudes_.resize(stft_size_);
				informer_.set_stft_size(stft_size_);
				return args;
			}
		}
	};

	sample operator()(sample mag, sample index)
    {
		if (index < magnitudes_.size())
		{
			magnitudes_[static_cast<size_t>(index)] = mag;
		}	

		if (index == magnitudes_.size() - 1)
		{
			
			informer_.set_magnitudes(magnitudes_, true);
			informer_.compute_descriptors(false, true);
			if (bool(normalize))
			{
				informer_.normalize_descriptors();
			}
		}

		return informer_.get_frequency_descriptor(selected_descriptor_name_);
	}

private:
	std::vector<sample> magnitudes_ = std::vector<sample>(4096, 0.0);;
	Informer::Informer<sample> informer_;
	unsigned int stft_size_ = 4096u;
	std::string selected_descriptor_name_ = "centroid";
};

MIN_EXTERNAL(freqdesc_tilde);
