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
				cout << "Frame size must be a power of two between 64 and 32768. Clamping to nearest valid value: " << size << "." << endl;
			}

			switch (size)
        	{
            	case 64:      frame_size = f_framesizes::f64; break;
           		case 128:     frame_size = f_framesizes::f128; break;
            	case 256:     frame_size = f_framesizes::f256; break;
            	case 512:     frame_size = f_framesizes::f512; break;
            	case 1024:    frame_size = f_framesizes::f1024; break;
            	case 2048:    frame_size = f_framesizes::f2048; break;
            	case 4096:    frame_size = f_framesizes::f4096; break;
            	case 8192:    frame_size = f_framesizes::f8192; break;
            	case 16384:   frame_size = f_framesizes::f16384; break;
            	case 32768:   frame_size = f_framesizes::f32768; break;
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
				window_.resize(frame_size_);
				fill_window_();
				return args;
			}
		}
	};

	message<> analyze
	{
		this,
		"analyze",
		"Analyze the current buffer.",
		setter
		{
			MIN_FUNCTION
			{
				analyze_();
				return {};
			}
		}
	};

	message<> bang
	{
		this,
		"bang",
		"Output the computed descriptors.",
		setter
		{
			MIN_FUNCTION
			{
				for (auto ch = 0; ch < t_analysis_outputs_.size(); ch++)
				{
					std::string channel = "ch" + std::to_string(ch+1);
					for (auto frame = 0; frame < t_analysis_outputs_[ch].size(); frame++)
					{
						for (const auto& descriptor : t_analysis_outputs_[ch][frame])
						{
							atoms output;
							output.reserve(3);
							output.push_back("append");
							std::string path = channel + "::" + "time" + "::" + descriptor.first;
							output.push_back(path);
							output.push_back(descriptor.second);
							out.send(output);
						}		
						for (const auto& descriptor : f_analysis_outputs_[ch][frame])
						{
							atoms output;
							output.reserve(3);
							output.push_back("append");
							std::string path = channel + "::" + "freq" + "::" + descriptor.first;
							output.push_back(path);
							output.push_back(descriptor.second);
							out.send(output);
						}			
					}
				}
				return {};
			}
		}
	};

private:
	Informer::Informer<sample> informer_;
	std::vector<sample> window_;
	unsigned int hop_size_ = 2048u;
	unsigned int frame_size_ = 4096u;

	std::vector<std::vector<std::unordered_map<std::string, sample>>> f_analysis_outputs_;
	std::vector<std::vector<std::unordered_map<std::string, sample>>> t_analysis_outputs_;

	inline void analyze_()
	{
		buffer_lock<> b(source_buffer);

		if (b.valid())
		{
			informer_.set_sample_rate(b.samplerate());
			f_analysis_outputs_.clear();
			t_analysis_outputs_.clear();

			for (auto ch = 0; ch < b.channel_count(); ++ch)
			{
				std::vector<std::unordered_map<std::string, sample>> f_analysis_channel_;
				std::vector<std::unordered_map<std::string, sample>> t_analysis_channel_;

				cout << "Analyzing channel " << ch + 1 << "..." << endl;

				for (auto f = 0; f < b.frame_count(); f += hop_size_)
				{
					// Read frame
					std::vector<sample> current_frame_(frame_size_, 0.0);
					for (auto s = 0; s < frame_size_; ++s)
					{
						auto current_sample = f + s;
						if (current_sample < b.frame_count())
						{
							current_frame_[s] = b.lookup(current_sample, ch);
						}
						else
						{
							current_frame_[s] = 0.0;
						}
					}

					// Set buffer
					informer_.set_buffer(current_frame_);

					dj::fft_arg<sample> fft_frame(frame_size_);

					// Prepare FFT frame
        			for (int s = 0; s < frame_size_; ++s)
					{
            			fft_frame[s] = std::complex<sample>(current_frame_[s] * window_[s], 0.0);
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

					f_analysis_channel_.push_back(frequency_descriptors);
					t_analysis_channel_.push_back(time_descriptors);
				}

				t_analysis_outputs_.push_back(t_analysis_channel_);
				f_analysis_outputs_.push_back(f_analysis_channel_);
			}
		}
	}

	static constexpr sample double_pi_ = 3.14159265358979323846 * 2.0;
	
	inline void fill_window_()
	{
		sample w_mul  = 1.0 / (static_cast<sample>(window_.size()) - 1.0);

		for (auto n = 0; n < window_.size(); n++)
		{
			window_[n] = 0.5 * (1.0 - cos(double_pi_ * n * w_mul));
		}
	}
};

MIN_EXTERNAL(bufferdesc);
