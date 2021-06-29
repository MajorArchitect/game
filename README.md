# Game Engine
This is a game engine written in C with OpenGL. I intend to cover at least the
most essential features of a game engine, and then use it challenge myself and
write a game. I hope it will be intuitive enough to get your head around
easily, but also be powerful enough to make a real game with.

## Features
### Current
- Ability to load multiple entities and display them (and remove them).
- Folder format for storing meshes and textures of a model together, and the
	appropriate functions for dealing with these.

### Planned
- As cross-platform as possible! It currently only works on Linux (maybe just
	my version of Ubuntu!) But any decision i will make about adding
	external stuff will be made with consideration of how easy it will be
	to port.
- A special entity that acts as the world/map, that can be changed frequently
	during runtime and has special properties like being able to reduce
	mesh resolution of far-away parts of it, etc.
- Ability to apply affects to objects such as colour and fade via GL shaders.
- A basic multiplayer protocol.

## How to use
At the moment, the repository itself is a test program. I have not separated
the test file ```main.c``` from the rest of the engine code yet, so you can
edit that file and change what happens when the project is built and run. To
build, just run ```build.sh``` to create an executable in ```bin/game``` (or
run ```build-g.sh``` to create an executable fit for ```gdb``` debugging
in ```bin/game-g```). The executable in the repository already contains the
test program from ```main.c```, so you can run that without compiling as soon
as you download the repository.  
The script ```build.sh``` clears the screen then compiles all the *.c files
with any linker statements that might be needed for X11 and OpenGL, etc. If
you add another file to the project, it must be added to this script to not
have linker errors. I will replace this with a Makefile once I figure out how
they work.
