#include "plugin.hpp"


struct TheInformer : Module {
	enum ParamId {
		NORMALIZE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INLEFT_INPUT,
		INRIGHT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AMPLITUDEKURTOSIS_OUTPUT,
		AMPLITUDEPEAK_OUTPUT,
		AMPLITUDEPEAK_OUTPUT,
		AMPLITUDERMS_OUTPUT,
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
	enum LightId {
		LIGHTS_LEN
	};

	TheInformer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(NORMALIZE_PARAM, 0.f, 1.f, 0.f, "");
		configInput(INLEFT_INPUT, "");
		configInput(INRIGHT_INPUT, "");
		configOutput(AMPLITUDEKURTOSIS_OUTPUT, "");
		configOutput(AMPLITUDEPEAK_OUTPUT, "");
		configOutput(AMPLITUDEPEAK_OUTPUT, "");
		configOutput(AMPLITUDERMS_OUTPUT, "");
		configOutput(AMPLITUDEVARIANCE_OUTPUT, "");
		configOutput(AMPLITUDEZEROCROSSING_OUTPUT, "");
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

	void process(const ProcessArgs& args) override {
	}
};


struct TheInformerWidget : ModuleWidget {
	TheInformerWidget(TheInformer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TheInformer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.6, 67.872)), module, TheInformer::NORMALIZE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.6, 29.0)), module, TheInformer::INLEFT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.6, 29.0)), module, TheInformer::INRIGHT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.1, 91.102)), module, TheInformer::AMPLITUDEKURTOSIS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.1, 91.102)), module, TheInformer::AMPLITUDEPEAK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.1, 91.102)), module, TheInformer::AMPLITUDEPEAK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.1, 91.102)), module, TheInformer::AMPLITUDERMS_OUTPUT));
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