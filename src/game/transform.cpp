#include "transform.hpp"

#include <bx/math.h>

#include "entity.hpp"

namespace gold {
	object& transform::getPrototype() {
		static auto proto = obj(initList{
			{"pos", vec3f(0, 0, 0)},
			{"rot", quatf(0, 0, 0, 1)},
			{"scl", vec3f(1, 1, 1)},
			{"setPosition", method(&transform::setPosition)},
			{"setRotation", method(&transform::setRotation)},
			{"setScale", method(&transform::setScale)},
			{"getPosition", method(&transform::getPosition)},
			{"getRotation", method(&transform::getRotation)},
			{"getScale", method(&transform::getScale)},
			{"getMatrix", method(&transform::getMatrix)},
			{"reset", method(&transform::reset)},
			{"proto", component::getPrototype()},
		});
		return proto;
	}

	transform::transform() : component() {}

	transform::transform(object config) : component() {
		setParent(getPrototype());
		setBool("rebuild", true);
		copy(config);
	}

	var transform::setPosition(list args) {
		if (args.getType(0) == typeList) {
			setPosition(args);
		} else if (args[0].isVec3()) {
			setVar("pos", args[0]);
			setBool("rebuild", true);
		} else if (args.size() >= 3 && args.isAllNumber()) {
			setVar(
				"pos",
				vec3f(
					args[0].getFloat(),
					args[1].getFloat(),
					args[2].getFloat()));
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::setRotation(list args) {
		if (args.getType(0) == typeList) {
			setRotation(args);
		} else if (args[0].isQuat()) {
			setVar("rot", args[0]);
			setBool("rebuild", true);
		} else if (args.size() >= 4 && args.isAllNumber()) {
			setVar(
				"rot",
				quatf(
					args[0].getFloat(),
					args[1].getFloat(),
					args[2].getFloat(),
					args[3].getFloat()));
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::setScale(list args) {
		if (args.getType(0) == typeList) {
			setScale(args);
		} else if (args[0].isVec3()) {
			setVar("scl", args[0]);
			setBool("rebuild", true);
		} else if (args.size() >= 3 && args.isAllNumber()) {
			setVar(
				"scl",
				vec3f(
					args[0].getFloat(),
					args[1].getFloat(),
					args[2].getFloat()));
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::getPosition(list) { return getVar("pos"); }

	var transform::getRotation(list) { return getVar("rot"); }

	var transform::getScale(list) { return getVar("scl"); }

	var transform::getMatrix() {
		if (
			getBool("rebuild", false) && getType("mtx") != typeNull) {
			return getVar("mtx");
		}

		auto pos = getVar("pos");
		auto rot = getVar("rot");
		auto scl = getVar("scl");

		auto results = mat4x4f({});
		results = results * scl;
		results = results * rot;
		results = results + pos;

		setVar("mtx", results);
		erase("rebuild");
		return results;
	}

	var transform::getWorldMatrix() {
		auto parentObj = entity();
		returnObject<entity>("object", parentObj);
		auto parentChain = list();
		while (parentObj) {
			parentChain.pushObject(parentObj);
			parentObj = parentObj.getObject<entity>("parent");
		}
		auto results = mat4x4f({});
		for (auto it = parentChain.rbegin();
				 it != parentChain.rend();
				 ++it) {
			it->returnObject<entity>(parentObj);
			auto comps = parentObj.getList("components");
			auto parentTrans = comps.find(transform::getPrototype());
			if (parentTrans != comps.end()) {
				auto trans = parentTrans->getObject<transform>();
				auto mtx = trans.getMatrix();
				results = results * mtx;
			}
		}
		return results;
	}

	var transform::reset(list) {
		setVar("pos", vec3f(0, 0, 0));
		setVar("rot", quatf(0, 0, 0, 1));
		setVar("scl", vec3f(1, 1, 1));
		setVar("mtx", mat4x4f({}));
		erase("rebuild");
		return var();
	}
}  // namespace gold