/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../../../../Library/informer.h"

using namespace c74::min;

class timedesc_tilde : public object<timedesc_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Compute the selected time domain descriptor from sample input and returns it as a signal"};
	MIN_TAGS {"audio, analysis, amplitude descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.timedesc, informer.freqdesc~"};

	inlet<>  in {this, "(signal) Input"};
	outlet<> out {this, "(signal) Computed descriptor", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			informer_.set_sample_rate(args[0]);
			sample_count_ = 0u;
			return {};
		}
	};

	enum class t_descriptors : int { peak, rms, kurtosis, skewness, variance, zerocrossing, enum_count };

    enum_map t_descriptors_range = {"peak", "rms", "kurtosis", "skewness", "variance", "zerocrossing"};

	argument<symbol> descriptor_arg
	{
		this,
		"descriptor",
		"Time domain descriptor to output.",
        MIN_ARGUMENT_FUNCTION
		{
			if (arg == "peak")
			{
				descriptor = t_descriptors::peak;
			}
			if (arg == "rms")
			{
				descriptor = t_descriptors::rms;
            }
			if (arg == "kurtosis")
			{
				descriptor = t_descriptors::kurtosis;
			}
			if (arg == "skewness")
			{
				descriptor = t_descriptors::skewness;
			}
			if (arg == "variance")
			{
				descriptor = t_descriptors::variance;
			}
			if (arg == "zerocrossing")
			{
				descriptor = t_descriptors::zerocrossing;
			}
		}
    };

	argument<int> buffer_size_arg
    {
        this,
        "buffer_size",
        "Size of the buffer.",
		MIN_ARGUMENT_FUNCTION
		{
			buffer_size = arg;
		}
    };

	attribute<t_descriptors> descriptor
	{
        this,
        "descriptor",
        t_descriptors::peak,
		t_descriptors_range,
        title {"Descriptor"},
		description {"Descriptor to output."},
		setter
		{
			MIN_FUNCTION
			{
				int index = static_cast<int>(args[0]);
                selected_descriptor_name_ = t_descriptors_range[index];
				return args;
			}
		}
    };

	attribute<bool> normalize
	{
        this,
        "normalize",
        false,
        title {"Normalize descriptor"},
        description {"When activated, the computed descriptor is normalized inside the [0, 1] range. Consider that some normalizations are performed on euristic bases, use only for artistic purposes."}
    };

	attribute<int, threadsafe::no, limit::clamp> buffer_size
	{
		this,
		"buffer_size",
		4096,
		range { 2, 65536 },
		title {"Buffer Size"},
		description {"Buffer size for descriptors computation."},
		setter
		{
			MIN_FUNCTION
			{
				buffer_.resize(int(args[0]));
				sample_count_ = 0u;
				return args;
			}
		}
	};

	attribute<number, threadsafe::no, limit::clamp> overlap
	{
		this,
		"overlap",
		0.5,
		range { 0.0 , 0.75 },
		title {"Overlap Factor"},
		description {"Overlap factor between consecutive buffers."}
	};

	sample operator()(sample input)
    {
		if (sample_count_ < buffer_.size() - 1)
		{
			buffer_[sample_count_] = input;
			sample_count_++;
		}
		else if (!buffer_.empty())
		{
			informer_.set_buffer(buffer_);
			informer_.compute_descriptors(true, false);
			if (bool(normalize))
			{
				informer_.normalize_descriptors();
			}

			// Prepare next buffer with overlap
			unsigned int overlap_samples = static_cast<unsigned int>(static_cast<double>(buffer_.size()) * double(overlap));
			unsigned int last_item = buffer_.size() - overlap_samples;
			if (overlap_samples > 0)
			{
				std::move(buffer_.begin() + last_item, buffer_.end(), buffer_.begin());
			}
			sample_count_ = overlap_samples;
		}

        return informer_.get_time_descriptor(selected_descriptor_name_);
	}

private:
	std::vector<sample> buffer_ = std::vector<sample>(4096, 0.0);
	Informer::Informer<sample> informer_;
	unsigned int sample_count_ = 0u;
	std::string selected_descriptor_name_ = "peak";
};

MIN_EXTERNAL(timedesc_tilde);
