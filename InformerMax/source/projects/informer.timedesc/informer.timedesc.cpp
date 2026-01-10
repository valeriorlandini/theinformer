/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../../../../Library/informer.h"

using namespace c74::min;

class timedesc : public object<timedesc>, public sample_operator<1, 0>
{
public:
	MIN_DESCRIPTION {"Compute time domain descriptors from sample input"};
	MIN_TAGS {"audio, analysis, amplitude descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.freqdesc, informer.timedesc~"};

	inlet<>  in_s {this, "(signal) Input"};
	outlet<> out {this, "Computed descriptors"};

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
        title {"Normalize descriptors"},
        description {"When activated, the computed descriptors are normalized inside the [0, 1] range. Consider that some normalizations are performed on euristic bases, use only for artistic purposes."}
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

	samples<0> operator()(sample input)
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
			auto amp_descriptors = informer_.get_time_descriptors();
			for (const auto& descriptor : amp_descriptors)
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

			// Prepare next buffer with overlap
			unsigned int overlap_samples = static_cast<unsigned int>(static_cast<double>(buffer_.size()) * double(overlap));
			unsigned int last_item = buffer_.size() - overlap_samples;
			if (overlap_samples > 0)
			{
				std::move(buffer_.begin() + last_item, buffer_.end(), buffer_.begin());
			}
			sample_count_ = overlap_samples;
		}

		return { };
	}

private:
	std::vector<sample> buffer_ = std::vector<sample>(4096, 0.0);
	Informer::Informer<sample> informer_;
	unsigned int sample_count_ = 0u;
};

MIN_EXTERNAL(timedesc);
