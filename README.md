# natus 

natus is a software framework for real-time interactive audio/visual applications. Ok, ok. Not yet but it will be. :art: :construction:



![AppVeyor](https://ci.appveyor.com/api/projects/status/d8h6pknsm29r2yyk?svg=true)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<!--
[![AppVeyor](https://img.shields.io/appveyor/build/aconstlink/natus?label=Vs2019%3AWin10%40master)](https://ci.appveyor.com/project/aconstlink/natus)
[![Azure DevOps builds](https://img.shields.io/azure-devops/build/aconstlink/cfc6b414-d551-4b4f-8dd7-ef0788dc1680/1?label=Azure%40master)](https://dev.azure.com/aconstlink/natus/_build?definitionId=1)
-->


Examples of using the engine already exists here in the [test project repository](https://github.com/aconstlink/natus_tests).

## Prerequisites

Windows prerequisites:  
 - Windows 10 SDK
 - Visual Studio 2019 Community **or**
 - msys/mingw :construction:
 - CMake


Linux prerequisites:  
For development:
```bash
libX11-devel mesa-libGLES-devel mesa-libGL-devel mesa-libEGL-devel alsa-lib-devel make cmake gcc gcc-c++
```

For execution only
```bash
libX11 mesa-libGLES mesa-libGL mesa-libEGL alsa-lib
```

For Raspberry Pi Os:
```bash
libx11-dev libglx-dev libgl-dev libgles-dev git cmake 
```

## Clone and Build

**Linux Make:**  
```
cd repos
git clone https://github.com/aconstlink/natus.git
mkdir _build/natus.platform
cd _build/natus.platform
cmake ../../natus
```

**Windows Visual Studio:**  
1. Open CMake Gui
2. Choose 64 bit Visual Studio 2019
3. Generate (out of source)
4. Open START_HERE.bat (required for finding dlls)(not in engine only builds)

## Usage  
:construction:

## Roadmap
The [Projects Tab](https://github.com/aconstlink/natus/projects) provides some sort of roadmap of the project. There is no particular priority, but if you really need anything, just leave a comment or just ask to [contribute](CONTRIBUTING.md) something you would like to do.

## Contributing  
Please see [CONTRIBUTING](CONTRIBUTING.md)

## License
[MIT](https://choosealicense.com/licenses/mit/)
