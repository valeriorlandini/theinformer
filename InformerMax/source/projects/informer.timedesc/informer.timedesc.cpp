/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <vector>
#include "../../../../Library/informer.h"

using namespace c74::min;

class timedesc : public object<timedesc>, public sample_operator<1, 0>
{
public:
	MIN_DESCRIPTION {"Compute time domain descriptors from sample input"};
	MIN_TAGS {"audio, analysis, amplitude descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.freqdesc"};

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
        "Size of the buffer."
    };

	timedesc(const atoms& args = {})
    {
        if (!args.empty())
        {
            buffer_.resize(std::max(2, int(args[0])));
			sample_count_ = 0u;
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

	attribute<int> buffer_size
	{
		this,
		"buffer_size",
		4096,
		title {"Buffer Size"},
		description {"Buffer size for descriptors computation."},
		setter
		{
			MIN_FUNCTION
			{
				buffer_.resize(std::max(2, int(args[0])));
				sample_count_ = 0u;
				return args;
			}
		}
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
			sample_count_ = 0u;
			informer_.set_buffer(buffer_);
			informer_.compute_descriptors(true, false);
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
		}

		return { };
	}

private:
	std::vector<sample> buffer_;
	Informer::Informer<sample> informer_;
	unsigned int sample_count_ = 0u;

	constexpr static const size_t num_descriptors_ = 6;
};

MIN_EXTERNAL(timedesc);
