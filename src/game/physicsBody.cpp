#include "physicsBody.hpp"

#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

#include "entity.hpp"
#include "shape.hpp"

namespace gold {
	object& physicsBody::getPrototype() {
		static auto proto = obj{
			{"mass", float(0)},
			{"proto", component::getPrototype()},
		};
		return proto;
	}

	physicsBody::physicsBody() : component() {}

	physicsBody::physicsBody(object config) : component(config) {
		setParent(getPrototype());
	}

	var physicsBody::initialize(list args) {
		float mass = getFloat("mass");
		auto parentObject = getObject<entity>("object");
		auto parTrans = parentObject.getTransform();
		auto posList = parTrans.getPosition().getList();
		auto rotList = parTrans.getPosition().getList();
		float pos[3];
		float rot[4];
		posList.assign(typeFloat, pos, 3);
		rotList.assign(typeFloat, rot, 4);
		auto btParTrans = btTransform(
			btQuaternion(rot[0], rot[1], rot[2], rot[3]),
			btVector3(pos[0], pos[1], pos[2]));
		auto shapes =
			parentObject
				.getComponentsRecursive({shape::getPrototype()})
				.getList();
		btCompoundShape* bodyShape = new btCompoundShape();
		for (auto it = shapes.begin(); it != shapes.end(); ++it) {
			auto shapeComp = it->getObject<shape>();
			auto shapeObj = shapeComp.getObject<entity>("object");
			auto trans = shapeObj.getTransform();
			auto posList = trans.getPosition().getList();
			auto rotList = trans.getPosition().getList();
			float pos[3];
			float rot[4];
			posList.assign(typeFloat, pos, 3);
			rotList.assign(typeFloat, rot, 4);
			auto btTrans = btTransform(
				btQuaternion(rot[0], rot[1], rot[2], rot[3]),
				btVector3(pos[0], pos[1], pos[2]));
			auto shape = (btCollisionShape*)shapeComp.getPtr("shape");
			bodyShape->addChildShape(btTrans, shape);
		}
		bodyShape->createAabbTreeFromChildren();
		btAssert(
			(!bodyShape ||
			 bodyShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			bodyShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState =
			new btDefaultMotionState(btParTrans);

		btRigidBody::btRigidBodyConstructionInfo cInfo(
			mass, myMotionState, bodyShape, localInertia);

		btRigidBody* body = new btRigidBody(cInfo);

		body->setUserIndex(-1);
		setPtr("body", body);
		return var();
	}

	var physicsBody::destroy(list args) {}
}  // namespace gold