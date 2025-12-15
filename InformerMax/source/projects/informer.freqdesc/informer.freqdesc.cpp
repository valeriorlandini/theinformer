/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <vector>
#include "../../../../Library/informer.h"

using namespace c74::min;

class freqdesc : public object<freqdesc>, public sample_operator<2, 0>
{
public:
	MIN_DESCRIPTION {"Compute frequency domain descriptors from magnitude spectrum input"};
	MIN_TAGS {"audio, analysis, spectral descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.timedesc"};

	inlet<>  in_m {this, "(signal) Magntiude spectrum input"};
	inlet<>  in_b {this, "(signal) Bin"};
	outlet<> out {this, "Computed descriptors"};

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

	argument<int> spectral_frame_size_arg
    {
        this,
        "frame_size",
        "Size of the spectral frame (number of bins)."
    };

	freqdesc(const atoms& args = {})
    {
        if (!args.empty())
        {
            spectral_frame_size = std::max(2, int(args[0]));
        }
    }

	attribute<bool> dict
	{
        this,
        "dict",
        false,
        title {"Output for dict"},
        description {"When activated, the output messages are formatted so that they can be directly sent to a dictionary."}
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

	samples<0> operator()(sample mag, sample index)
    {
		if (index < magnitudes_.size())
		{
			magnitudes_[static_cast<size_t>(index)] = mag;
		}	

		if (index == magnitudes_.size() - 1)
		{
			
			informer_.set_magnitudes(magnitudes_, true);
			informer_.compute_descriptors(false, true);
			auto spectral_descriptors = informer_.get_frequency_descriptors();
			for (const auto& descriptor : spectral_descriptors)
			{
				atoms output;
				if (bool(dict))
				{
					output.reserve(3);
					output.push_back("set");
				}
				else
				{
					output.reserve(2);
				}
				// Descriptor name
				output.push_back(descriptor.first);
				// Descriptor value
				output.push_back(descriptor.second);
				out.send(output);
			}
		}

		return { };
	}

private:
	std::vector<sample> magnitudes_;
	Informer::Informer<sample> informer_;
	unsigned int stft_size_ = 4096u;

	constexpr static const size_t num_descriptors_ = 13;
};

MIN_EXTERNAL(freqdesc);
