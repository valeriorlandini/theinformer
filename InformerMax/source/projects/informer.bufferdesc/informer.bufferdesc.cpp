/// @file
///	@ingroup 	informer.*
///	@copyright	Copyright 2024-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../../../../Library/informer.h"
#define DJ_FFT_IMPLEMENTATION
#include "dj_fft.h"
#include <string>
#include <vector>

using namespace c74::min;

class bufferdesc : public object<bufferdesc>
{
public:
	MIN_DESCRIPTION {"Compute frequency domain descriptors from magnitude spectrum input"};
	MIN_TAGS {"audio, analysis, spectral descriptors"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"informer.timedesc"};

	inlet<>  in_m {this, "(signal) Magntiude spectrum input"};
	inlet<>  in_b {this, "(signal) Bin"};
	outlet<> out {this, "Computed descriptors"};

	enum class f_framesizes : int { f64, f128, f256, f512, f1024, f2048, f4096, f8192, f16384, f32768, enum_count };

    enum_map f_framesizes_range = {"64", "128", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768"};


	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			informer_.set_sample_rate(args[0]);
			window_.resize(frame_size_ / 2 + 1);
			fill_window_();
			return {};
		}
	};    
	
	buffer_reference source_buffer
    {
        this,
        MIN_FUNCTION
        {
            return {};
        }
    };

	argument<symbol> buffer_arg
    {
        this,
        "buffer",
        "Name of the buffer to operate on.",
		MIN_ARGUMENT_FUNCTION
		{
			source_buffer.set(arg);
		}
    };

	argument<int> frame_size_arg
    {
        this,
        "frame_size",
        "Size of the analysis frame, must be a power of two between 64 and 32768.",
		MIN_ARGUMENT_FUNCTION
		{
			int size = arg;
			if (size < 64 || size > 32768 || (size & (size - 1)) != 0)
			{			
				// Clamp to nearest power of two
				if (size < 64)
				{
					size = 64;
				}
				else if (size > 32768)
				{
					size = 32768;
				}
				else
				{
					// Find nearest power of two
					int power = 1;
					while (power < size)
					{
						power *= 2;
					}
					int lower_power = power / 2;
					if ((size - lower_power) < (power - size))
					{
						size = lower_power;
					}
					else
					{
						size = power;
					}
				}
				cout << "Frame size must be a power of two between 64 and 32768. Clamping to nearest valid value: " << size << ".";
			}

			switch (size)
        	{
            	case 64:      frame_size = f_framesizes::f64;
           		case 128:     frame_size = f_framesizes::f128;
            	case 256:     frame_size = f_framesizes::f256;
            	case 512:     frame_size = f_framesizes::f512;
            	case 1024:    frame_size = f_framesizes::f1024;
            	case 2048:    frame_size = f_framesizes::f2048;
            	case 4096:    frame_size = f_framesizes::f4096;
            	case 8192:    frame_size = f_framesizes::f8192;
            	case 16384:   frame_size = f_framesizes::f16384;
            	case 32768:   frame_size = f_framesizes::f32768;
            	default:      frame_size = f_framesizes::f4096;
        	}
		}
    };

	attribute<f_framesizes> frame_size
	{
		this,
		"frame_size",
		f_framesizes::f4096,
		f_framesizes_range,
		title {"Frame Size"},
		description {"Size of the analysis frame."},
		setter
		{
			MIN_FUNCTION
			{
				frame_size_ = std::stoi(f_framesizes_range[static_cast<int>(args[0])]);
				hop_size_ = frame_size_ / 2;
				informer_.set_stft_size(frame_size_ / 2 + 1);
				window_.resize(frame_size_ / 2 + 1);
				fill_window_();
				return args;
			}
		}
	};

	message<> bang
	{
		this,
		"bang",
		"Analyze the specified buffer and output the computed descriptors.",
		setter
		{
			MIN_FUNCTION
			{
				analyze_();
				return {};
			}
		}
	};
/*
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
*/
private:
	Informer::Informer<sample> informer_;
	std::vector<sample> window_;
	//std::vector<std::array<std::vector<sample>, descriptors_>> analysis_outputs_;
	unsigned int hop_size_ = 2048u;
	unsigned int frame_size_ = 4096u;

	inline void analyze_()
	{
		buffer_lock<> b(source_buffer);

		if (b.valid())
		{
			for (auto ch = 0; ch < b.channel_count(); ++ch)
			{				
				for (auto f = 0; f < b.frame_count(); f += hop_size_)
				{
					// Read frame
					std::vector<sample> current_frame_(frame_size_, 0.0);
					for (auto s = 0; s < frame_size_; ++s)
					{
						if (s < b.frame_count())
						{
							current_frame_[s] = b.lookup(f + s, ch);
						}
						else
						{
							current_frame_[s] = 0.0;
						}
					}

					// Set buffer
					informer_.set_buffer(current_frame_);

					dj::fft_arg<sample> fft_frame;

					// Prepare FFT frame
        			for (int s = 0; s < frame_size_; ++s)
					{
            			fft_frame.push_back(std::complex<sample>(current_frame_[s] * window_[s], 0.0));
					}

					// Compute magnitude spectrum
					auto fftData = dj::fft1d(fft_frame, dj::fft_dir::DIR_FWD);
					std::vector<sample> magnitudes_(frame_size_ / 2 + 1, 0.0);
					for (auto k = 0; k < frame_size_ / 2 + 1; ++k)
					{
						magnitudes_[k] = std::abs(fftData[k]);
					}

					// Set magnitudes
					informer_.set_magnitudes(magnitudes_, true);
					
					// Compute descriptors
					informer_.compute_descriptors(true, true);

					// Get and output descriptors
					auto time_descriptors = informer_.get_time_descriptors();
					auto frequency_descriptors = informer_.get_frequency_descriptors();
				}
			}
		}
	}

	static constexpr sample pi_2_ = 3.14159265358979323846 * 2.0;
	
	inline void fill_window_()
	{
		sample w_mul  = 1.0 / (static_cast<sample>(window_.size()) - 1.0);

		for (auto n = 0; n < window_.size(); n++)
		{
			window_[n] = 0.5 * (1.0 - cos(pi_2_ * n * w_mul));
		}
	}
};

MIN_EXTERNAL(bufferdesc);
