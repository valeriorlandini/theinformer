#include "plugin.hpp"

#include "oscpkt.hh"
#include "udp.hh"

struct TheInformer : Module
{
    enum ParamId
    {
        NORMALIZE_PARAM,
        PARAMS_LEN
    };
    enum InputId
    {
        IN_INPUT,
        INPUTS_LEN
    };
    enum OutputId
    {
        AMPLITUDEKURTOSIS_OUTPUT,
        AMPLITUDEPEAK_OUTPUT,
        AMPLITUDERMS_OUTPUT,
        AMPLITUDESKEWNESS_OUTPUT,
        AMPLITUDEVARIANCE_OUTPUT,
        AMPLITUDEZEROCROSSING_OUTPUT,
        CENTROID_OUTPUT,
        CRESTFACTOR_OUTPUT,
        DECREASE_OUTPUT,
        ENTROPY_OUTPUT,
        FLATNESS_OUTPUT,
        FLUX_OUTPUT,
        IRREGULARITY_OUTPUT,
        KURTOSIS_OUTPUT,
        PEAK_OUTPUT,
        ROLLOFF_OUTPUT,
        SKEWNESS_OUTPUT,
        SLOPE_OUTPUT,
        SPREAD_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId
    {   
        NORMALIZE_LIGHT,
        LIGHTS_LEN
    };

    TheInformer() : fftProcessor(BUFFER_SIZE)
    {
        std::cout << "TheInformer module initialized." << std::endl;
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(NORMALIZE_PARAM, 0.f, 1.f, 0.f, "Normalize OSC Values");
        configInput(IN_INPUT, "Audio In");
        configOutput(AMPLITUDEKURTOSIS_OUTPUT, "Amp Kurtosis");
        configOutput(AMPLITUDEPEAK_OUTPUT, "Amp Peak");
        configOutput(AMPLITUDERMS_OUTPUT, "Amp RMS");
        configOutput(AMPLITUDESKEWNESS_OUTPUT, "Amp Skewness");
        configOutput(AMPLITUDEVARIANCE_OUTPUT, "Amp Variance");
        configOutput(AMPLITUDEZEROCROSSING_OUTPUT, "Zero Crossing Rate");
        configOutput(CENTROID_OUTPUT, "Centroid");
        configOutput(CRESTFACTOR_OUTPUT, "Crest Factor");
        configOutput(DECREASE_OUTPUT, "Decrease");
        configOutput(ENTROPY_OUTPUT, "Entropy");
        configOutput(FLATNESS_OUTPUT, "Flatness");
        configOutput(FLUX_OUTPUT, "Flux");
        configOutput(IRREGULARITY_OUTPUT, "Irregularity");
        configOutput(KURTOSIS_OUTPUT, "Kurtosis");
        configOutput(PEAK_OUTPUT, "Peak");
        configOutput(ROLLOFF_OUTPUT, "Rolloff");
        configOutput(SKEWNESS_OUTPUT, "Skewness");
        configOutput(SLOPE_OUTPUT, "Slope");
        configOutput(SPREAD_OUTPUT, "Spread");

        informer = new Informer::Informer<float>();
        buffer = new std::vector<float>(BUFFER_SIZE, 0.f);

        informer->set_sample_rate(44100.f);
        informer->set_stft_size(BUFFER_SIZE);
        buffer->assign(BUFFER_SIZE, 0.f);

        // Initialize the OSC socket 
        socket = new oscpkt::UdpSocket();
        socket->connectTo(ip.c_str(), port);
        if (!socket->isOk())
        {
            std::cerr << "Error connecting to port " << port << ": " << socket->errorMessage() << std::endl;
        }
    }

    ~TheInformer()
    {
        socket->close();
        delete socket;
        delete informer;
        delete buffer;
    }

    Informer::Informer<float> *informer = nullptr;
    std::vector<float> *buffer = nullptr;
    oscpkt::UdpSocket *socket = nullptr;
    std::string ip = "127.0.0.1";
    int port = 8000;
    std::string oscRoot = "/theinformer";
    static constexpr int BUFFER_SIZE = 8192;
    dsp::RealFFT fftProcessor;

    unsigned int count = 0;
    float ampKurtosis = 0.0f;
    float ampPeak = 0.0f;
    float ampRms = 0.0f;
    float ampSkewness = 0.0f;
    float ampVariance = 0.0f;
    float ampZeroCrossing = 0.0f;
    float centroid = 0.0f;
    float crestFactor = 0.0f;
    float decrease = 0.0f;
    float entropy = 0.0f;
    float flatness = 0.0f;
    float flux = 0.0f;
    float irregularity = 0.0f;
    float kurtosis = 0.0f;
    float peak = 0.0f;
    float rolloff = 0.0f;
    float skewness = 0.0f;
    float slope = 0.0f;
    float spread = 0.0f;
    bool dirty = false;

	void onReset() override
    {
		ip = "127.0.0.1";
        oscRoot = "/theinformer";
        port = 8000;
		dirty = true;
	}

	void fromJson(json_t* rootJ) override 
    {
		Module::fromJson(rootJ);
		json_t* ipJ = json_object_get(rootJ, "ip");
		if (ipJ)
        {
            ip = json_string_value(ipJ);
        }
        json_t* oscRootJ = json_object_get(rootJ, "oscRoot");
        if (oscRootJ)
        {
            oscRoot = json_string_value(oscRootJ);
        }
		dirty = true;
	}

	json_t* dataToJson() override 
    {
		json_t* rootJ = json_object();
        json_object_set_new(rootJ, "ip", json_stringn(ip.c_str(), ip.size()));
        json_object_set_new(rootJ, "oscRoot", json_stringn(oscRoot.c_str(), oscRoot.size()));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override
    {
        json_t* ipJ = json_object_get(rootJ, "ip");
        if (ipJ)
        {
            ip = json_string_value(ipJ);
        }
        json_t* oscRootJ = json_object_get(rootJ, "oscRoot");
        if (oscRootJ)
        {
            oscRoot = json_string_value(oscRootJ);
        }
		dirty = true;
	}

    inline void normalize(float sampleRate)
    {
        if (sampleRate <= 0.f)
        {
            return;
        }
        float invNyquist = 1.f / (sampleRate * 0.5f);

        ampKurtosis = clamp(ampKurtosis + 2.f, 0.f, 4.f) * 0.25f;
        ampSkewness = clamp(ampSkewness + 2.f, 0.f, 4.f) * 0.25f;
        centroid *= invNyquist;
        decrease = clamp((decrease + 0.05f) * 10.f, 0.f, 1.f);
        flux = clamp(flux / 2048.f, 0.f, 1.f);
        kurtosis = clamp(kurtosis + 2.f, 0.f, 4.f) * 0.25f;
        irregularity = clamp(irregularity * 0.5f, 0.f, 1.f);
        peak *= invNyquist;
        rolloff *= invNyquist;
        skewness = clamp(skewness + 2.f, 0.f, 4.f) * 0.25f;
        slope = clamp((slope + 1.f) * 0.5f, 0.f, 1.f);
        spread *= invNyquist * 0.5f;
    }

    inline std::vector<float> createMagBuffer(float *fftBuffer)
    {
        std::vector<float> magBuffer;
        magBuffer.assign(BUFFER_SIZE / 2 + 1, 0.f);

        magBuffer[0] = fabs(fftBuffer[0]) / (float)(BUFFER_SIZE);

        for (auto s = 2; s < BUFFER_SIZE; s += 2)
        {
            float real = fftBuffer[s];
            float imag = fftBuffer[s + 1];
            magBuffer[s / 2] = sqrt(real * real + imag * imag) / (float)(BUFFER_SIZE);
        }

        magBuffer[BUFFER_SIZE / 2] = fabs(fftBuffer[1]) / (float)(BUFFER_SIZE);

        return magBuffer;
    }

    inline void connectTo()
    {
        if (socket->isOk())
        {
            socket->connectTo(ip.c_str(), port);
            if (!socket->isOk())
            {
                std::cerr << "Error connecting to port " << port << ": " << socket->errorMessage() << std::endl;
            }
        }
        else
        {
            std::cerr << "Socket is not ok, cannot connect." << std::endl;
        }
    }

    void process(const ProcessArgs& args) override
    {
        float input = clamp(inputs[IN_INPUT].getVoltage() * 0.2f, -1.f, 1.f);

        if (args.sampleRate != informer->get_sample_rate())
        {
            informer->set_sample_rate(args.sampleRate);
            count = 0;
        }

        if (inputs[IN_INPUT].isConnected())
        {
            if (count < BUFFER_SIZE)
            {
                buffer->at(count++) = input;
            }
            else
            {
                std::vector<float> windowedBuffer = Informer::Frequency::window(*buffer);
                alignas(16) float *windowedBufferPtr = windowedBuffer.data();
                alignas(16) float freqBuffer[BUFFER_SIZE * 2];
                fftProcessor.rfft(windowedBufferPtr, freqBuffer);
                std::vector<float> audioBuffer;
                for (auto s = 0; s < BUFFER_SIZE; s++)
                {
                    audioBuffer.push_back(buffer->at(s));
                }
                informer->set_buffer(audioBuffer);
                informer->set_magnitudes(createMagBuffer(freqBuffer));
                auto freqs = informer->get_magnitudes();
                auto prec = informer->get_precomputed_frequencies();
                informer->compute_descriptors(true, true);

                ampKurtosis = informer->get_time_descriptor("kurtosis");
                ampPeak = informer->get_time_descriptor("peak");
                ampRms = informer->get_time_descriptor("rms");
                ampSkewness = informer->get_time_descriptor("skewness");
                ampVariance = informer->get_time_descriptor("variance");
                ampZeroCrossing = informer->get_time_descriptor("zerocrossing");

                centroid = informer->get_frequency_descriptor("centroid");
                crestFactor = informer->get_frequency_descriptor("crestfactor");
                decrease = informer->get_frequency_descriptor("decrease");
                entropy = informer->get_frequency_descriptor("entropy");
                flatness = informer->get_frequency_descriptor("flatness");
                flux = informer->get_frequency_descriptor("flux");
                irregularity = informer->get_frequency_descriptor("irregularity");
                kurtosis = informer->get_frequency_descriptor("kurtosis");
                peak = informer->get_frequency_descriptor("peak");
                rolloff = informer->get_frequency_descriptor("rolloff");
                skewness = informer->get_frequency_descriptor("skewness");
                slope = informer->get_frequency_descriptor("slope");
                spread = informer->get_frequency_descriptor("spread");

                count = BUFFER_SIZE / 2;
                std::move(buffer->begin() + BUFFER_SIZE / 2, buffer->end(), buffer->begin());
                std::fill(buffer->begin() + BUFFER_SIZE / 2, buffer->end(), 0.f);
                buffer->at(count++) = input;

                if (params[NORMALIZE_PARAM].getValue() >= 0.5f)
                {
                    normalize(args.sampleRate);
                }

                // Send OSC messages with the computed values
                if (socket->isOk())
                {
                    oscpkt::PacketWriter pw;
                    pw.startBundle().startBundle();

                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_kurtosis").pushFloat(ampKurtosis));
                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_peak").pushFloat(ampPeak));
                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_rms").pushFloat(ampRms));
                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_skewness").pushFloat(ampSkewness));
                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_variance").pushFloat(ampVariance));
                    pw.addMessage(oscpkt::Message(oscRoot + "/time/amp_zero_crossing").pushFloat(ampZeroCrossing));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/centroid").pushFloat(centroid));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/crestfactor").pushFloat(crestFactor));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/decrease").pushFloat(decrease));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/entropy").pushFloat(entropy));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/flatness").pushFloat(flatness));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/flux").pushFloat(flux));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/irregularity").pushFloat(irregularity));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/kurtosis").pushFloat(kurtosis));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/peak").pushFloat(peak));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/rolloff").pushFloat(rolloff));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/skewness").pushFloat(skewness));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/slope").pushFloat(slope));
                    pw.addMessage(oscpkt::Message(oscRoot + "/freq/spread").pushFloat(spread));

                    pw.endBundle().endBundle();
                    socket->sendPacket(pw.packetData(), pw.packetSize());
                }

                // If normalize is not set (so the OSC messages were sent not normalized)
                // we need to normalize the values before sending them to the module outputs
                if (params[NORMALIZE_PARAM].getValue() < 0.5f)
                {
                    normalize(args.sampleRate);
                }

                // Send the values to the module outputs
                outputs[AMPLITUDEKURTOSIS_OUTPUT].setVoltage(ampKurtosis * 10.f);
                outputs[AMPLITUDEPEAK_OUTPUT].setVoltage(ampPeak * 10.f);
                outputs[AMPLITUDERMS_OUTPUT].setVoltage(ampRms * 10.f);
                outputs[AMPLITUDESKEWNESS_OUTPUT].setVoltage(ampSkewness * 10.f);
                outputs[AMPLITUDEVARIANCE_OUTPUT].setVoltage(ampVariance * 10.f);
                outputs[AMPLITUDEZEROCROSSING_OUTPUT].setVoltage(ampZeroCrossing * 10.f);
                outputs[CENTROID_OUTPUT].setVoltage(centroid * 10.f);
                outputs[CRESTFACTOR_OUTPUT].setVoltage(crestFactor * 10.f);
                outputs[DECREASE_OUTPUT].setVoltage(decrease * 10.f);
                outputs[ENTROPY_OUTPUT].setVoltage(entropy * 10.f);
                outputs[FLATNESS_OUTPUT].setVoltage(flatness * 10.f);
                outputs[FLUX_OUTPUT].setVoltage(flux * 10.f);
                outputs[IRREGULARITY_OUTPUT].setVoltage(irregularity * 10.f);
                outputs[KURTOSIS_OUTPUT].setVoltage(kurtosis * 10.f);
                outputs[PEAK_OUTPUT].setVoltage(peak * 10.f);
                outputs[ROLLOFF_OUTPUT].setVoltage(rolloff * 10.f);
                outputs[SKEWNESS_OUTPUT].setVoltage(skewness * 10.f);
                outputs[SLOPE_OUTPUT].setVoltage(slope * 10.f);
                outputs[SPREAD_OUTPUT].setVoltage(spread * 10.f);
            }
        }
        else
        {
            // If the input is not connected, reset the count and buffer
            count = 0;
            std::fill(buffer->begin(), buffer->end(), 0.f);

            // Reset all outputs to 0
            outputs[AMPLITUDEKURTOSIS_OUTPUT].setVoltage(0.f);
            outputs[AMPLITUDEPEAK_OUTPUT].setVoltage(0.f);
            outputs[AMPLITUDERMS_OUTPUT].setVoltage(0.f);
            outputs[AMPLITUDESKEWNESS_OUTPUT].setVoltage(0.f);
            outputs[AMPLITUDEVARIANCE_OUTPUT].setVoltage(0.f);
            outputs[AMPLITUDEZEROCROSSING_OUTPUT].setVoltage(0.f);
            outputs[CENTROID_OUTPUT].setVoltage(0.f);
            outputs[CRESTFACTOR_OUTPUT].setVoltage(0.f);
            outputs[DECREASE_OUTPUT].setVoltage(0.f);
            outputs[ENTROPY_OUTPUT].setVoltage(0.f);
            outputs[FLATNESS_OUTPUT].setVoltage(0.f);
            outputs[FLUX_OUTPUT].setVoltage(0.f);
            outputs[IRREGULARITY_OUTPUT].setVoltage(0.f);
            outputs[KURTOSIS_OUTPUT].setVoltage(0.f);
            outputs[PEAK_OUTPUT].setVoltage(0.f);
            outputs[ROLLOFF_OUTPUT].setVoltage(0.f);
            outputs[SKEWNESS_OUTPUT].setVoltage(0.f);
            outputs[SLOPE_OUTPUT].setVoltage(0.f);
            outputs[SPREAD_OUTPUT].setVoltage(0.f);
        }

        lights[NORMALIZE_LIGHT].setBrightness(params[NORMALIZE_PARAM].getValue());
    }
};

struct IpTextField : LedDisplayTextField
{
	TheInformer* module;

	void step() override
    {
		LedDisplayTextField::step();
		if (module && module->dirty)
        {
			setText(module->ip);
			module->dirty = false;
		}
	}

	void onChange(const ChangeEvent& e) override
    {
		if (module)
        {
			auto newIp = getText();
            if (!newIp.empty())
            {
                module->ip = newIp;
                module->dirty = true;
            }
        }
	}
};

struct IpDisplay : LedDisplay
{
	void setModule(TheInformer* module)
    {
		IpTextField* textField = createWidget<IpTextField>(Vec(0, 0));
		textField->box.size = box.size;
		textField->multiline = false;
		textField->module = module;
		addChild(textField);
	}
};

struct PortTextField : LedDisplayTextField
{
	TheInformer* module;

	void step() override
    {
		LedDisplayTextField::step();
		if (module && module->dirty)
        {
			setText(std::to_string(module->port));
			module->dirty = false;
		}
	}

	void onChange(const ChangeEvent& e) override
    {
		if (module)
        {
            auto newPort = getText();
            if (!newPort.empty() && std::all_of(newPort.begin(), newPort.end(), ::isdigit))
            {
                module->port = std::max(0, std::min(std::stoi(newPort), 65535));
                module->connectTo();
                module->dirty = true;
            }
        }
	}
};

struct PortDisplay : LedDisplay
{
	void setModule(TheInformer* module)
    {
		PortTextField* textField = createWidget<PortTextField>(Vec(0, 0));
		textField->box.size = box.size;
		textField->multiline = false;
		textField->module = module;
		addChild(textField);
	}
};

struct RootTextField : LedDisplayTextField
{
	TheInformer* module;

	void step() override
    {
		LedDisplayTextField::step();
		if (module && module->dirty)
        {
			setText(module->oscRoot);
			module->dirty = false;
		}
	}

	void onChange(const ChangeEvent& e) override
    {
		if (module)
        {
            auto newRoot = getText();
            if (!newRoot.empty())
            {
                if (newRoot[0] != '/')
                {
                    newRoot = "/" + newRoot; // Ensure the root starts with a slash
                }
                module->oscRoot = newRoot;
                module->connectTo();
                module->dirty = true;
            }
        }
	}
};

struct RootDisplay : LedDisplay
{
	void setModule(TheInformer* module)
    {
		RootTextField* textField = createWidget<RootTextField>(Vec(0, 0));
		textField->box.size = box.size;
		textField->multiline = false;
		textField->module = module;
		addChild(textField);
	}
};

struct TheInformerWidget : ModuleWidget
{
    TheInformerWidget(TheInformer* module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/theinformer.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(30.6, 67.872)), module, TheInformer::NORMALIZE_PARAM, TheInformer::NORMALIZE_LIGHT));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.5, 19.65)), module, TheInformer::IN_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.1, 91.102)), module, TheInformer::AMPLITUDEKURTOSIS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.1, 91.102)), module, TheInformer::AMPLITUDEPEAK_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.1, 91.102)), module, TheInformer::AMPLITUDERMS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.1, 91.102)), module, TheInformer::AMPLITUDESKEWNESS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.1, 91.102)), module, TheInformer::AMPLITUDEVARIANCE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.1, 100.627)), module, TheInformer::AMPLITUDEZEROCROSSING_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.1, 100.627)), module, TheInformer::CENTROID_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.1, 100.627)), module, TheInformer::CRESTFACTOR_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.1, 100.627)), module, TheInformer::DECREASE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.1, 100.627)), module, TheInformer::ENTROPY_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.1, 110.152)), module, TheInformer::FLATNESS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.1, 110.152)), module, TheInformer::FLUX_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.1, 110.152)), module, TheInformer::IRREGULARITY_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.1, 110.152)), module, TheInformer::KURTOSIS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.1, 110.152)), module, TheInformer::PEAK_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.1, 119.677)), module, TheInformer::ROLLOFF_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.1, 119.677)), module, TheInformer::SKEWNESS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.1, 119.677)), module, TheInformer::SLOPE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.1, 119.677)), module, TheInformer::SPREAD_OUTPUT));

        IpDisplay* ipDisplay = createWidget<IpDisplay>(mm2px(Vec(26, 26.938)));
		ipDisplay->box.size = mm2px(Vec(40, 10));
		ipDisplay->setModule(module);
		addChild(ipDisplay);

        PortDisplay* portDisplay = createWidget<PortDisplay>(mm2px(Vec(26, 38.916)));
		portDisplay->box.size = mm2px(Vec(40, 10));
		portDisplay->setModule(module);
		addChild(portDisplay);

        RootDisplay* rootDisplay = createWidget<RootDisplay>(mm2px(Vec(26, 50.895)));
		rootDisplay->box.size = mm2px(Vec(40, 10));
		rootDisplay->setModule(module);
		addChild(rootDisplay);
        
    }
};


Model* modelTheInformer = createModel<TheInformer, TheInformerWidget>("TheInformer");