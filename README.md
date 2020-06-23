[gold](https://github.com/CoryNull/gold) - Generic Object Linked Development - A high level app framework
============================================================================
[![License](https://img.shields.io/badge/license-Apache%202-blue)](https://github.com/CoryNull/gold/LICENSE)

What is this?

It's a high level abstraction app framework, written in C++. It uses simple objects to carry out various tasks in a simple data driven development. Making fast video games or HTTP web services can be organically and quickly developed, and both sub systems are split accordingly, and they'll both share the same base. You can make a game server that uses the same code base as the game. Or a website that talks to a game, or vice-versa. More additions and ideas coming down the road.

What's in the box?
* RAII Objects/Lists
* Object inheritance (prototypes)
* Type agnosticism with abstracted run-time type information
* Universal generic RAII value container; "var" <-- type name
* First class Vector, Quaternion, Matrix3x3, Matrix4x4 var types
* JSON/BSON/CBOR/MsgPack/UBJSON/URLForm serialization
* Express.JS "like" HTTP(S)/WebSocket server
* HTML5 rendering (with form handling/pragmatic templating)
* MongoDB driver and basic MVC system 
* Server-side image loading (not connected to game).
* Object & component based game engine
* Basic window handling
* 3D matrix transformation hierarchies
* Texture loading(2D/3D/Cube)
* Auto shader compilation, with inlining
* CMake utilities
* Still experimental threading stuff (workers/promises)
* 3rdParty dependencies are sub modules to other GitHub projects
* Hard parts of C++ have been abstracted to JS/Python difficulty
* Works with GCC and Clang (MSVC is untested).

Where it falls short?
* Memory overhead (maybe not ideal for very low spec hardware (Arduino/IOT))
* Lacks some in-depth error handling (see genericError)
* Threading is subsystem limited and experimental (off by default)
* Has heavy 3rdParty dependencies that have it's own dependencies
* Needs the latest bleeding edge compiler and STL library
* Uses the C++17/20 standard

What it's lacking?
* Documentation
* Comments
* Bug/error testing
* Tests
* A website

What's planned?
* Documentation
* More bug testing
* In-depth complex examples
* Expanded game engine
* Expanded web services
* Asset handling
* Game editor
* GUI handling
* Audio handling
* Controller handling
* Async event handling
* Asset packing
* Error reporting
* Scripting language interface
* Compile to WebAssembly/ASM.JS?

# Getting Started

You can copy everything from the examples directory to get started with a basic web app. It's better to make this project a submodule in git instead of cloning/copying the project. I've found that using all cores on my machine causes throttling because there's a lot of code that needs to be compiled which is a great stress, use half or almost all instead. Any subsequent compiles will be easier because of CMake's object caching system though. I have yet to make a game example but it's not hard to get started though. Here's an example using gold's game subsystem.

```C++
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "camera.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "mesh.hpp"
#include "player.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "transform.hpp"

using namespace gold;

int main() {
	engine main = engine("MountainAndValley", "MyGame");
	auto cam = main.getPrimaryCamera().getObject<camera>();
	if (cam) {
		auto camTrans =
			cam.getComponent({transform::getPrototype()})
				.getObject<gold::transform>();
		if (camTrans) {
			camTrans.setPosition({vec3i32(0, 0, 0)});
		}
	}

	//auto cubeModel = mesh("./assets/sceneTest.gltf");

	auto playerEntity = entity({{"name", "player"}});
	auto player0 = playerComp(playerEntity, obj{});
	auto playerTrans = playerEntity.getTransform();
	playerTrans.setPosition({0, 0, -5});
	main += {playerEntity};

	main.start();

	return 0;
}
```

All code not in 3rdParty or explicitly stated otherwise are Apache version 2.