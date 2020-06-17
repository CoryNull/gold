#include "light.hpp"

namespace gold {
	obj& light::getPrototype() {
		static auto proto = obj({
			{"priority", priorityEnum::genericPriority},
			{"color", 0xffffffff},
			{"intensity", 1.0f},
			{"cone", vec2f(1, 1)},
			{"type", "point"},
			{"setColor", method(&light::setColor)},
			{"getColor", method(&light::getColor)},
			{"setIntensity", method(&light::setIntensity)},
			{"getIntensity", method(&light::getIntensity)},
			{"setCone", method(&light::setCone)},
			{"getCone", method(&light::getCone)},
			{"setType", method(&light::setType)},
			{"getType", method(&light::getType)},
			{"getArea", method(&light::getArea)},
			{"drawScene", method(&light::drawScene)},
			{"proto", component::getPrototype()},
		});
		return proto;
	}

	var light::setColor(list args) {
		auto color = list::getColor(args);
		setUInt32("color", color);
		return color;
	}

	var light::getColor() { return getUInt32("color"); }

	var light::setIntensity(list args) {
		float in = args[0].getFloat();
		setFloat("intensity", in);
		return in;
	}

	var light::getIntensity() { return getFloat("intensity"); }

	var light::setCone(list args) {
		auto val = list::getVec2f(args);
		setVar("cone", val);
		return val;
	}

	var light::getCone() { return getVar("cone"); }

	var light::setType(list args) {
		auto val = args.getString(0);
		setString("type", val);
		return val;
	}

	var light::getType() { return getString("type"); }

	var light::getArea() { return var(); }

	var light::drawScene(list comps, uint16_t scene) {
		return var();
	}

	light::light() : component() { setParent(getPrototype()); }

	light::light(obj config) : component() {
		copy(config);
		setParent(getPrototype());
	}
}  // namespace gold