#include "world.hpp"

#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <btBulletCollisionCommon.h>

#include "engine.hpp"
#include "entity.hpp"
#include "physicsBody.hpp"
#include "transform.hpp"

namespace gold {

	object& world::getPrototype() {
		static object proto = obj({
			{"bodies", list()},
			{"gravity", list({0, -9.8, 0})},
			{"maxSubSteps", 1},
			{"fixedTimeStep", 1.0 / 60.0},
		});
		return proto;
	}

	world::world() {}

	world::world(object config) : object(config) {
		setParent(getPrototype());
	}

	var world::step(list args) {
		auto fixedTimeStep = getFloat("fixedTimeStep");
		auto timeStep = args.getFloat(0, fixedTimeStep);
		auto maxSubSteps = getInt32("maxSubSteps");
		auto bodies = getList("bodies");
		auto dWorld =
			(btDiscreteDynamicsWorld*)getPtr("dynamicsWorld");
		dWorld->stepSimulation(
			timeStep, maxSubSteps, fixedTimeStep);
		auto comp = physicsBody();
		auto body = (btRigidBody*)nullptr;
		auto ent = entity();
		auto trans = transform();
		for (auto it = bodies.begin(); it != bodies.end(); ++it) {
			if (
				(comp = it->getObject<physicsBody>()) &&
				(body = (btRigidBody*)comp.getPtr("body")) &&
				(ent = comp.getObject<entity>("object")) &&
				(trans = ent.getComponent({transform::getPrototype()})
									 .getObject<transform>())) {
				auto wTrans = body->getWorldTransform();
				auto rot = wTrans.getRotation();
				auto pos = wTrans.getOrigin();
				trans.setList(
					"rot", {rot.x(), rot.y(), rot.z(), rot.w()});
				trans.setList("pos", {pos.x(), pos.y(), pos.z()});
				trans.setBool("rebuild", true);
			} else {
				it = bodies.erase(it);
			}
		}

		return var();
	}

	var world::setGravity(list args) {
		auto dWorld =
			(btDiscreteDynamicsWorld*)getPtr("dynamicsWorld");
		float gravity[3];
		if (args.isAllNumber() && args.size() >= 3)
			args.assign(typeFloat, gravity, 3);
		setList("gravity", args);
		dWorld->setGravity(
			btVector3(gravity[0], gravity[1], gravity[2]));
		return var();
	}

	var world::raytrace(list args) {
		float from[3];
		float to[3];
		args[0].getList().assign(typeFloat, from, 3);
		args[1].getList().assign(typeFloat, to, 3);
		return var();
	}

	var world::initialize(list args) {
		auto engIt = args.find(engine::getPrototype());
		if (engIt != args.end()) {
			auto eng = engIt->getObject<engine>();
			eng.addRegisterCompnentCallback(
				{physicsBody::getPrototype(),
				 func([this](const list& args) {
					 // This gets called when a physicsBody gets added to
					 // the parent engine
					 auto ar =
						 (list&)args;  // Container is const, the data isn't
					 auto comp = physicsBody();
					 auto body = (btRigidBody*)nullptr;
					 auto dWorld = (btDiscreteDynamicsWorld*)nullptr;
					 auto compIt = ar.find(physicsBody::getPrototype());
					 if (
						 compIt != ar.end() &&
						 (comp = compIt->getObject<physicsBody>()) &&
						 (body = (btRigidBody*)comp.getPtr("body")) &&
						 (dWorld = (btDiscreteDynamicsWorld*)getPtr(
								"dynamicsWorld"))) {
						 dWorld->addRigidBody(body);
						 comp.setObject("world", *this);
						 auto bodies = getList("bodies");
						 bodies.pushObject(comp);
					 }
					 return var();
				 })});
			setObject("engine", eng);
		} else {
			return genericError("Expected engine to be first arg");
		}
		auto gravityList = getList("gravity");
		float gravity[3];
		gravityList.assign(typeFloat, gravity, 3);
		auto collisionConfiguration =
			new btDefaultCollisionConfiguration();
		setPtr("collisionConfiguration", collisionConfiguration);

		auto dispatcher =
			new btCollisionDispatcher(collisionConfiguration);
		setPtr("dispatcher", dispatcher);

		auto broadphase = new btDbvtBroadphase();
		setPtr("broadphase", broadphase);

		auto solver = new btSequentialImpulseConstraintSolver;
		setPtr("solver", solver);

		auto dynamicsWorld = new btDiscreteDynamicsWorld(
			dispatcher, broadphase, solver, collisionConfiguration);
		setPtr("dynamicsWorld", dynamicsWorld);

		dynamicsWorld->setGravity(
			btVector3(gravity[0], gravity[1], gravity[2]));
		return var();
	}

	var world::destroy(list) {
		auto dynamicsWorld =
			(btDiscreteDynamicsWorld*)getPtr("dynamicsWorld");
		if (dynamicsWorld) {
			delete dynamicsWorld;
			erase("dynamicsWorld");
		}

		auto solver =
			(btSequentialImpulseConstraintSolver*)getPtr("solver");
		if (solver) {
			delete solver;
			erase("solver");
		}

		auto broadphase = (btDbvtBroadphase*)getPtr("broadphase");
		if (broadphase) {
			delete broadphase;
			erase("broadphase");
		}

		auto dispatcher =
			(btCollisionDispatcher*)getPtr("dispatcher");
		if (dispatcher) {
			delete dispatcher;
			erase("dispatcher");
		}

		auto collisionConfiguration =
			(btDefaultCollisionConfiguration*)getPtr(
				"collisionConfiguration");
		if (collisionConfiguration) {
			delete collisionConfiguration;
			erase("collisionConfiguration");
		}

		empty();
		return var();
	}

}  // namespace gold