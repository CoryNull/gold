# [gold/myWebProject](https://github.com/CoryNull/gold/examples/myWebProject)
### Simple Full-Stack Web Example

[![License](https://img.shields.io/badge/license-Apache%202-blue)](https://github.com/CoryNull/gold/LICENSE)

What is this?

This is an example and a copy of the full stack web service I'm developing in house for my business. This uses the gold framework pretty much exclusively. It uses [JQuery](https://jquery.com/), [JQueryUI](https://jqueryui.com/), [Bootstrap](https://getbootstrap.com/), [Bootstrap-Table](https://bootstrap-table.com/), [Quill](https://quilljs.com/), [Croppie](https://foliotek.github.io/Croppie/), [Luxon](https://moment.github.io/luxon/), and [FontAwesome](https://fontawesome.com/). You clone this and use which ever frontend system you like (React or even Angular) and gold's C++ web framework will interface nicely. It will use currently supported ECMA and WWW standards, HTML5, CSS, and ECMAScript v10. This uses REST over HTTP for right now but with a little work it could use WebSockets to send information. It still needs some work though and I'm open to pull requests to improve the system. Parts from this might become standard in gold, perhaps as utility, templates, or generated. One Idea I had was creating something "like" ReactJS/PreactJS and compiling to WASM/asmJS. Haven't tried porting gold to EMScripten, yet... But I really want to.

I don't think you will be disappointed, if you do run into some issues, fixing and modifying gold is easy. The page load time for localhost has been faster than some other NodeJS/Python/Java/C# frameworks I've used in my opinion. The executable compiled with debug symbols ends up just being 21.5mb~. The server used only 12.2mb of shared memory and 9.4mb of ram. This is faster and lighter than any high level language can deliver. Loading in less than 5ms (debugging) for static pages, and adding a little overhead to call the MongoDB database, usually totalling 15ms~ on my Ubuntu AMD Ryzen 3700X DDR4 32GB high-end gaming machine (dual booting Windows 10). FireFox and Chrome work great... haven't actually tested Microsoft's IE or Edge, yet.

All code not in 3rdParty or explicitly stated otherwise (like files in js or css) are Apache version 2.