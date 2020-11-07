# OrcThief
Ogre3d-based game project. An experiment with Constructive Solid Geometry (CSG)

This project aims to reproduce some of the tools provided by CSG, inspiried by the work of Sander van Rossen (@logicalerror). Some references include the Realtime CSG series on http://sandervanrossen.blogspot.com/, http://realtimecsg.com/, and Chisel. 

The project aims to be as portable as possible, at least in terms of output. For a portable development environment (ie: CMake), contributions are welcome.

Fate has not yet decided what the game is going to be about.

# Build

## Generator

Currently, only a Visual Studio 2019 solution is offered, in vs_build. Contributions in the form of a CMake version are welcome.

## Required Dependencies

We don't provide binaries on this repo, therefore the user will be required to install some dependencies on their own to build the project.

### Ogre3d

The project depends Ogre3d 2.2 (aka Ogre Next). 

The location of the installation must be under "/ext/Ogre" - symbolic links supported. 

The required components of the Ogre3d installation are:
#### Headers
- /OgreMain/include/...
- /Components/Hlms/Unlit/include/...
- /Components/Hlms/Pbs/include/...
- /Components/Hlms/Common/include/...
- /Components/Overlay/include/...

Those should all be available by cloning Ogre3d's Git repository.

#### Binaries and build headers
The user will also need to prepare manually a "/build" folder. This folder consists of three things:
- /build/include/...
- /build/lib/...
- /build/bin/...

Those folders contain the build configuration header, the built import libraries (for Windows), and the built dynamic libraries, respectively. Those can all be acquired by building Ogre3d through the CMake. Some information on how to acquire all of Ogre3d's dependencies can be found here: http://wiki.ogre3d.org/Building+Dependencies

### SDL2
The project depends on SDL2. 

The location of the installation must be under "/ext/SDL2" - symbolic links supported.

The required components of the SDL2 installation are:
#### Headers
- /include/...

This should already be available when downloading an SDL2 source repository.

#### Binaries
- /bin/x64/debug
- /bin/x64/release

Where "x64" is for 64-bits Windows binaries. Debug contains non-optimized binaries, and Release contains optimized ones. Those folders must contain the following binaries:
- SDL2.dll (same name in Debug)
- SDL2.lib (on Windows, same name in Debug)
- SDL2main.lib (same name in Debug)

Adding PDB files too can't hurt.

### Boost

Yeah, Boost got in there. It's only Boost.QVM for now, it's just too good to pass on.

The location of the installation must be under "/ext/boost" - symbolic links supported.

#### Headers
- /boost/qvm/...

### fmt
The project depends on fmt. 

The location of the installation must be under "/ext/fmt" - symbolic links supported.

The required components of the installation are:

#### Headers
- /include/*.h

#### Binaries
- /build/Debug/fmtd.lib
- /build/Release/fmt.lib

### FreeType

To get better results on Dear ImGui fonts, there is a dependency on FreeType.

A "binary package" of FreeType is sufficient (ex: freetype-windows-binaries)

#### Headers
- /include/*.h

#### Binaries
- /win64/freetype.lib
- /win64/freetype.dll

### TL;DR
```
mklink /D %ORC_THIEF%\ext\Ogre %MY_LIRARIES%\Ogre
mklink /D %ORC_THIEF%\ext\SDL2 %MY_LIRARIES%\SDL2
mklink /D %ORC_THIEF%\ext\boost %MY_LIBRARIES%\boost
mklink /D %ORC_THIEF%\ext\fmt %MY_LIBRARIES%\fmt
mklink /D %ORC_THIEF%\ext\FreeType %MY_LIBRARIES%\FreeType
```

## Project

Now, time to build the project itself. The repository layout consists of the following layout:
- /ext, which contains the external dependencies
- /lib, which contains the local submodules of the project
- /res, which contains the non-code assets (ie: resources) of the project
- /src, which contains the code of the main executable module of the project
- /test, which contains the unit test executable module
- /vs_build, which contains the provided Visual Studio 2019 generator

For folders containing other code modules (/ext, /lib, /test, etc...), this general layout can be found recursively:
- /ext, again containing the external dependencies of the module
- /include, for the public headers of the module
- /src, for the private headers and source of the module
- /vs_build, which contains a provided Visual Studio 2019 generator

To get started building the project itself, open /vs_build/OrcThief.sln on your favorite Windows distribution. Select your desired Configuration. Build the OrcThief project.

Voilà.

# Running the project

Find the output directory of the executable (ex: /vs_build/x64/Debug). If you ran the steps in the Building section, you should see OrcThief.exe. Now, there's a few requirements before actually running the executable.

## Runtime Dependencies

Time to find those DLLs. TODO

## Configuration

TODO

## Required resources

TODO

