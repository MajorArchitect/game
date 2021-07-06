# Game
This is a game written in C with OpenGL. I think it will be a clone of Wurm
Online.

## Features
### Current
- Ability to load multiple entities and display them (and remove them).
- Folder format for storing meshes and textures of a model together, and the
	appropriate functions for dealing with these.
- Basic physics.

### Planned
- Terraforming!
- Crafting!
- Combat!
- Magic!
- As cross-platform as possible! It currently only works on Linux (maybe just
	my version of Ubuntu!) But any decision I will make about adding
	external stuff will be made with consideration of how easy it will be
	to port.
- A special entity that acts as the world/map, that can be changed frequently
	during runtime and has special properties like being able to reduce
	mesh resolution of far-away parts of it, etc.
- Ability to apply affects to objects such as colour and fade via GL shaders.
- A multiplayer protocol.

## How to use
The binary in ```bin/game``` is the result of compiling the game with all the
code seen in ```src/``` on my machine. ```bin/game-g``` might also contain a
version of the game compiled to be compatible with GDB debugging, and it can be
remade by running ```make -f makefile-g```.
