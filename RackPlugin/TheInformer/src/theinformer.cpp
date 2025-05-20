#include "plugin.hpp"


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
        LIGHTS_LEN
    };

    TheInformer() : fftProcessor(BUFFER_SIZE)
    {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(NORMALIZE_PARAM, 0.f, 1.f, 0.f, "");
        configInput(IN_INPUT, "");
        configOutput(AMPLITUDEKURTOSIS_OUTPUT, "Amp Kurtosis");
        configOutput(AMPLITUDEPEAK_OUTPUT, "Amp Peak");
        configOutput(AMPLITUDERMS_OUTPUT, "Amp RMS");
        configOutput(AMPLITUDESKEWNESS_OUTPUT, "Amp Skewness");
        configOutput(AMPLITUDEVARIANCE_OUTPUT, "Amp Variance");
        configOutput(AMPLITUDEZEROCROSSING_OUTPUT, "Zero Crossing Rate");
        configOutput(CENTROID_OUTPUT, "");
        configOutput(CRESTFACTOR_OUTPUT, "");
        configOutput(DECREASE_OUTPUT, "");
        configOutput(ENTROPY_OUTPUT, "");
        configOutput(FLATNESS_OUTPUT, "");
        configOutput(FLUX_OUTPUT, "");
        configOutput(IRREGULARITY_OUTPUT, "");
        configOutput(KURTOSIS_OUTPUT, "");
        configOutput(PEAK_OUTPUT, "");
        configOutput(ROLLOFF_OUTPUT, "");
        configOutput(SKEWNESS_OUTPUT, "");
        configOutput(SLOPE_OUTPUT, "");
        configOutput(SPREAD_OUTPUT, "");
    }

    Informer::Informer<float> informer;
    static constexpr int BUFFER_SIZE = 4096;
	dsp::RealFFT fftProcessor;
    alignas(16) float buffer[BUFFER_SIZE] = {};
    unsigned int count = 0;
    float ampKurtosis;
    float ampPeak;
    float ampRms;
    float ampSkewness;
    float ampVariance;
    float ampZeroCrossing;
    float centroid;
    float crestFactor;
    float decrease;
    float entropy;
    float flatness;
    float flux;
    float irregularity;
    float kurtosis;
    float peak;
    float rolloff;
    float skewness;
    float slope;
    float spread;

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
        magBuffer.assign(BUFFER_SIZE * 2, 0.f);

        for (auto s = 0; s < BUFFER_SIZE * 2; s += 2)
        {
            magBuffer[s / 2] = fftBuffer[s];
        }

        return magBuffer;
    }

    void process(const ProcessArgs& args) override
    {
        float input = clamp(inputs[IN_INPUT].getVoltage() * 0.2f, -1.f, 1.f);

		if (args.sampleRate != informer.get_sample_rate())
		{
			informer.set_sample_rate(args.sampleRate);
			count = 0;
		}

        if (count < BUFFER_SIZE)
        {
            buffer[count++] = input;
        }
        else
        {
            alignas(16) float freqBuffer[BUFFER_SIZE * 2];
			fftProcessor.rfft(buffer, freqBuffer);
            std::vector<float> audioBuffer;
            for (auto s = 0; s < BUFFER_SIZE; s++)
            {
                audioBuffer.push_back(buffer[s]);
            }
            informer.set_buffer(audioBuffer);
            informer.set_stft(createMagBuffer(freqBuffer));
            informer.compute_descriptors(true, true);

            ampKurtosis = informer.get_time_descriptor("kurtosis");
            ampPeak = informer.get_time_descriptor("peak");
            ampRms = informer.get_time_descriptor("rms");
            ampSkewness = informer.get_time_descriptor("skewness");
            ampVariance = informer.get_time_descriptor("variance");
            ampZeroCrossing = informer.get_time_descriptor("zerocrossing");
			
            centroid = informer.get_frequency_descriptor("centroid");
            crestFactor = informer.get_frequency_descriptor("crestfactor");
            decrease = informer.get_frequency_descriptor("decrease");
            entropy = informer.get_frequency_descriptor("entropy");
            flatness = informer.get_frequency_descriptor("flatness");
            flux = informer.get_frequency_descriptor("flux");
            irregularity = informer.get_frequency_descriptor("irregularity");
            kurtosis = informer.get_frequency_descriptor("kurtosis");
            peak = informer.get_frequency_descriptor("peak");
            rolloff = informer.get_frequency_descriptor("rolloff");
            skewness = informer.get_frequency_descriptor("skewness");
            slope = informer.get_frequency_descriptor("slope");
            spread = informer.get_frequency_descriptor("spread");

            count = 0;
            buffer[count++] = input;
        }

        if (params[NORMALIZE_PARAM].getValue() >= 0.5f)
        {
            normalize(args.sampleRate);
        }

        /*** SEND OSC MESSAGES ***/

        // If normalize is not set (so the OSC messages were sent not normalized)
        // we need to normalize the values before sending them to the module outputs
        if (params[NORMALIZE_PARAM].getValue() < 0.5f)
        {
            normalize(args.sampleRate);
        }

		// Send the values to the module outputs
        outputs[AMPLITUDEKURTOSIS_OUTPUT].setVoltage(ampKurtosis * 5.f);
        outputs[AMPLITUDEPEAK_OUTPUT].setVoltage(ampPeak * 5.f);
        outputs[AMPLITUDERMS_OUTPUT].setVoltage(ampRms * 5.f);
		outputs[AMPLITUDESKEWNESS_OUTPUT].setVoltage(ampSkewness * 5.f);
        outputs[AMPLITUDEVARIANCE_OUTPUT].setVoltage(ampVariance * 5.f);
        outputs[AMPLITUDEZEROCROSSING_OUTPUT].setVoltage(ampZeroCrossing * 5.f);
		
        outputs[CENTROID_OUTPUT].setVoltage(centroid * 5.f);
        outputs[CRESTFACTOR_OUTPUT].setVoltage(crestFactor * 5.f);
        outputs[DECREASE_OUTPUT].setVoltage(decrease * 5.f);
        outputs[ENTROPY_OUTPUT].setVoltage(entropy * 5.f);
        outputs[FLATNESS_OUTPUT].setVoltage(flatness * 5.f);
        outputs[FLUX_OUTPUT].setVoltage(flux * 5.f);
        outputs[IRREGULARITY_OUTPUT].setVoltage(irregularity * 5.f);
        outputs[KURTOSIS_OUTPUT].setVoltage(kurtosis * 5.f);
        outputs[PEAK_OUTPUT].setVoltage(peak * 5.f);
        outputs[ROLLOFF_OUTPUT].setVoltage(rolloff * 5.f);
        outputs[SKEWNESS_OUTPUT].setVoltage(skewness * 5.f);
        outputs[SLOPE_OUTPUT].setVoltage(slope * 5.f);
        outputs[SPREAD_OUTPUT].setVoltage(spread * 5.f);
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

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.6, 67.872)), module, TheInformer::NORMALIZE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.1, 29.0)), module, TheInformer::IN_INPUT));

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
    }
};


Model* modelTheInformer = createModel<TheInformer, TheInformerWidget>("TheInformer");