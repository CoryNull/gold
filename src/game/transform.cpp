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

	void transform::getMatrix(float* results) {
		if (
			getBool("rebuild", false) && getType("mtx") == typeList) {
			auto cMtx = getVar("mtx");
			for (auto i = 0; i < 16; ++i)
				results[i] = cMtx.getFloat(i);
			return;
		}

		auto pos = getVar("pos");
		auto rot = getVar("rot");
		auto scl = getVar("scl");

		auto fRot =
			bx::Quaternion({rot.getFloat(0), rot.getFloat(1),
											rot.getFloat(2), rot.getFloat(3)});
		bx::mtxIdentity(results);
		bx::mtxScale(
			results,
			scl.getFloat(0),
			scl.getFloat(1),
			scl.getFloat(2));
		bx::mtxQuat(results, fRot);
		bx::mtxTranslate(
			results,
			pos.getFloat(0),
			pos.getFloat(1),
			pos.getFloat(2));

		auto cMtx = mat4x4f({});
		for (auto i = 0; i < 16; ++i) cMtx.setFloat(i, results[i]);
		setVar("mtx", cMtx);
		erase("rebuild");
	}

	void transform::getWorldMatrix(float* results) {
		auto parentObj = entity();
		returnObject<entity>("object", parentObj);
		auto parentChain = list();
		while (parentObj) {
			parentChain.pushObject(parentObj);
			parentObj = parentObj.getObject<entity>("parent");
		}
		bx::mtxIdentity(results);
		for (auto it = parentChain.rbegin();
				 it != parentChain.rend();
				 ++it) {
			it->returnObject<entity>(parentObj);
			auto comps = parentObj.getList("components");
			auto parentTrans = comps.find(transform::getPrototype());
			if (parentTrans != comps.end()) {
				auto trans = parentTrans->getObject<transform>();
				auto mtx = mat4x4f({});
				trans.getMatrix((float*)mtx.getPtr());
				bx::mtxMul(results, results, (float*)mtx.getPtr());
			}
		}
	}

	var transform::reset(list) {
		setVar("pos", vec3f(0, 0, 0));
		setVar("rot", quatf(0, 0, 0, 1));
		setVar("scl", vec3f(1, 1, 1));
		auto mtx = mat4x4f({});
		bx::mtxIdentity((float*)mtx.getPtr());
		setVar("mtx", mtx);
		erase("rebuild");
		return var();
	}
}  // namespace gold