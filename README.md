[gold](https://github.com/CoryNull/gold) - High level app framework
============================================================================
[![License](https://img.shields.io/badge/license-Apache%202-blue)](https://github.com/CoryNull/gold/LICENSE)

What is this?

It's a high level abstraction app framework, written in C++. It uses simple objects to carry out various tasks in a simple data driven development. Making really fast video games or HTTP web services can be organically and quickly developed, and both sub systems are split accordingly, and they'll both share the same base. You can make a game server that uses the same code base as the game. Or a website that talks to a game, or vice-versa. What this framework can do right now is still basic but it can already do great things. With more additions and ideas coming down the road.

What's in the box?
* RAII Objects/Lists
* Object inheritance (prototypes)
* Type agnosticism with abstracted run-time type information
* Universal generic RAII value container; "var" <-- type name
* First class Vector, Quaternion, Matrix3x3, Matrix4x4 var types
* JSON/BSON/CBOR/MsgPack/UBJSON/URLForm serialization
* Express.JS like HTTP(S)/WebSocket server
* HTML5 rendering (with form handling/pragmatic templating)
* MongoDB driver and basic MVC system 
* Object & component based game engine
* Basic window handling
* 3D matrix transformation hierarchies
* Texture loading(2D/3D/Cube)
* Auto shader compilation, with inlining
* CMake utilities
* Still experimental threading stuff (workers/promises)
* 3rdParty dependencies are sub modules to other GitHub projects
* Hard parts of C++ have been abstracted to JS/Python difficulty

Where it falls short?
* Memory overhead (not ideal for very low spec hardware (Arduino/IOT))
* Lacks in depth error handling (see genericError)
* Threading is subsystem limited and experimental (off by default)
* Has heavy 3rdParty dependencies that have it's own dependencies
* Needs the latest bleeding edge compiler and STL library
* Uses the C++17/20 standard

What it's lacking?
* Documentation
* Comments
* Bug/error testing
* Examples
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
* 2D/3D Physics
* Controller handling
* Async event handling
* Asset packing
* Error reporting
* Scripting language interface
* Compile to WebAssembly/ASM.JS?
