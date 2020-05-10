[gold](https://github.com/CoryNull/gold) - High level app framework
============================================================================
[![License](https://img.shields.io/badge/license-Apache%202-blue)](https://github.com/CoryNull/gold/LICENSE)

What is this?

It's a high level abstraction app framework, written in C++. It uses simple objects to carry out various tasks in a simple data driven development. Making really fast video games or HTTP web services can be organically and quickly developed, and both sub systems are split accordingly, and they'll both share the same base. You can make a game server that uses the same code base as the game. Or a website that talks to a game, or vice-versa. What this framework can do right now is still basic but it can already do great things. With more additions and ideas coming down the road.

What's in the box?
* RAII Objects/Lists
* Object inheritance (prototypes)
* Type agnosticism with abstracted run-time type information
* Universal RAII value container; var
* JSON/BSON/CBOR/MsgPack/UBJSON/URLForm serialization
* Express.JS like HTTP(S)/WebSocket server
* HTML5 rendering (with form handling/pragmatic templating)
* MongoDB driver and basic MVC system 
* Object & component based game engine
* Basic window handling
* 3D matrix transformation hierarchies
* Texture loading(2D/3D/Cube)
* Automatic multi-platform shader compilation, compiled to be inlined
* CMake utilities
* Still experimental threading stuff (workers/promises)
* Quick dev time, fast compilation times, creating fast code
* All 3rdParty dependencies are sub modules to other GitHub projects

Where it falls short?
* Memory overhead (not ideal for very low spec hardware (Arduino/IOT))
* Lacks in depth error handling (see genericError)
* Threading is limited and experimental (off by default)
* Has a couple heavy 3rdParty dependencies that have it's own dependencies
* If used incorrectly produces undefined behaviors and doesn't stop you
* It feels like it's still in alpha but functional enough for business
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
* Compile to WebAssembly/ASM.JS
