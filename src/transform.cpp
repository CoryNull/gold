#include "transform.hpp"

#include <bx/math.h>

#include "entity.hpp"

namespace gold {
	object& transform::getPrototype() {
		static auto proto = obj(initList{
			{"pos", var(list{0, 0, 0})},
			{"rot", var(list{0, 0, 0, 1})},
			{"scl", var(list{1, 1, 1})},
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

	transform::transform() : component() {
	}

	transform::transform(object config) : component() {
		setParent(getPrototype());
		setBool("rebuild", true);
		copy(config);
	}

	var transform::setPosition(list args) {
		if (args.getType(0) == typeList) {
			auto pos = args[0].getList();
			if (pos.size() >= 3) {
				pos.resize(3);
				setList("pos", pos);
				setBool("rebuild", true);
			}
		} else if (args.size() >= 3) {
			auto x = args[0].getFloat();
			auto y = args[1].getFloat();
			auto z = args[2].getFloat();
			setList("pos", {x, y, z});
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::setRotation(list args) {
		if (args.getType(0) == typeList) {
			auto rot = args[0].getList();
			if (rot.size() >= 4) {
				rot.resize(4);
				setList("rot", rot);
				setBool("rebuild", true);
			}
		} else if (args.size() >= 4) {
			auto x = args[0].getFloat();
			auto y = args[1].getFloat();
			auto z = args[2].getFloat();
			auto w = args[3].getFloat();
			setList("rot", {x, y, z, w});
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::setScale(list args) {
		if (args.getType(0) == typeList) {
			auto scl = args[0].getList();
			if (scl.size() >= 3) {
				scl.resize(3);
				setList("scl", scl);
				setBool("rebuild", true);
			}
		} else if (args.size() >= 3) {
			auto x = args[0].getFloat();
			auto y = args[1].getFloat();
			auto z = args[2].getFloat();
			setList("scl", {x, y, z});
			setBool("rebuild", true);
		}
		return var();
	}

	var transform::getPosition(list) {
		auto pos = list({0, 0, 0});
		return var(getList("pos", pos));
	}

	var transform::getRotation(list) {
		auto rot = list({0, 0, 0, 1});
		return var(getList("rot", rot));
	}

	var transform::getScale(list) {
		auto scl = list({1, 1, 1});
		return var(getList("scl", scl));
	}

	void transform::getMatrix(float* results) {
		if (
			getBool("rebuild", false) && getType("mtx") == typeList) {
			auto cMtx = list();
			getList("mtx", cMtx);
			for (auto i = 0; i < 16; ++i)
				results[i] = cMtx.getFloat(i);
			return;
		}

		list pos = {0, 0, 0};
		list rot = {0, 0, 0, 1};
		list scl = {1, 1, 1};
		getList("pos", pos);
		getList("rot", rot);
		getList("scl", scl);
		auto fRot =
			bx::Quaternion({rot[0], rot[1], rot[2], rot[3]});
		bx::mtxIdentity(results);
		bx::mtxScale(results, scl[0], scl[1], scl[2]);
		bx::mtxQuat(results, fRot);
		bx::mtxTranslate(results, pos[0], pos[1], pos[2]);

		list cMtx = list({});
		for (auto i = 0; i < 16; ++i) cMtx.pushFloat(results[i]);
		setList("mtx", cMtx);
		erase("rebuild");
	}

	void transform::getWorldMatrix(float* results) {
		auto parentObj = entity();
		returnObject<entity>("object",parentObj);
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
				float transMtx[16];
				trans.getMatrix(transMtx);
				bx::mtxMul(results, results, transMtx);
			}
		}
	}

	var transform::reset(list) {
		setList("pos", {0, 0, 0});
		setList("rot", {0, 0, 0, 1});
		setList("scl", {1, 1, 1});
		float mtx[16];
		bx::mtxIdentity(mtx);
		list results = list({});
		for (auto i = 0; i < 16; ++i) results.pushFloat(mtx[i]);
		setList("mtx", results);
		erase("rebuild");
		return var();
	}
}  // namespace gold