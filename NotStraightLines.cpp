#include <rack.hpp>

using namespace rack;

Plugin* pluginInstance;

static const int NUM_LINES = 60;
static const int NUM_POINTS = 128;

struct NotStraightLines : Module {
	enum Params {
		NUM_PARAMS
	};
	enum Inputs {
		X_INPUT,
		Y_INPUT,
		TRIG_INPUT,
		NUM_INPUTS
	};
	enum Outputs {
		NUM_OUTPUTS
	};
	enum Lights {
		NUM_LIGHTS
	};

	float x[NUM_LINES * NUM_POINTS] {};
	float y[NUM_LINES * NUM_POINTS] {};
	int index {};

	NotStraightLines() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(X_INPUT, "X");
		configInput(Y_INPUT, "Y");
		configInput(TRIG_INPUT, "Trigger");
	}

	void onReset() override {
		memset(x, 0, sizeof(x));
		memset(y, 0, sizeof(y));
		index = 0;
	}

	void process(const ProcessArgs& args) override {
		if (index < NUM_LINES * NUM_POINTS) {
			x[index] = inputs[X_INPUT].getVoltage() / 5.f;
			y[index] = inputs[Y_INPUT].getVoltage() / 5.f;
			index++;
		}
		else if (!inputs[TRIG_INPUT].isConnected() || inputs[TRIG_INPUT].getVoltage() >= 2.f) {
			index = 0;
		}
	}
};


struct ScopeWidget : TransparentWidget {
	NotStraightLines *module;

	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer != 1) return;

		nvgBeginPath(args.vg);
		for (int j = 0; j < NUM_LINES; j++) {
			for (int i = 0; i < NUM_POINTS; i++) {
				Vec p = Vec(i, j + 0.5f).div(Vec(NUM_POINTS - 1, NUM_LINES));
				int index = j * NUM_POINTS + i;
				p = p.plus(Vec(module->x[index], module->y[index]).div(Vec(NUM_LINES, NUM_LINES)));
				p = p.mult(box.size);
				if (i == 0)
					nvgMoveTo(args.vg, p.x, p.y);
				else
					nvgLineTo(args.vg, p.x, p.y);
			}
		}
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStrokeWidth(args.vg, 0.5);
		// nvgStrokeColor(args.vg, nvgRGBA(80, 80, 80, 0xff));
		nvgStrokeColor(args.vg, SCHEME_BLACK);
		nvgStroke(args.vg);

		Widget::drawLayer(args, layer);
	}
};

struct NotStraightLinesWidget : ModuleWidget {
	NotStraightLinesWidget(NotStraightLines* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/NotStraightLines.svg")));

		float x = 7.f, y = 123.f;
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(x, y)), module, NotStraightLines::X_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(x + 19.5f, y)), module, NotStraightLines::Y_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(x + 39.f, y)), module, NotStraightLines::TRIG_INPUT));

		ScopeWidget* scope = createWidget<ScopeWidget>(mm2px(Vec(13.5f, 10.5f)));
		scope->box.size = mm2px(Vec(100.f, 100.f));
		scope->module = module;
		addChild(scope);
	}
};


void init(Plugin *p) {
	pluginInstance = p;
	p->addModel(createModel<NotStraightLines, NotStraightLinesWidget>("NotStraightLines"));
}

