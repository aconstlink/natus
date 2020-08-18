# natus 

natus is a software framework for real-time interactive audio/visual application. Ok, ok. Not yet but it will be. :art: :construction:

[![Travis (.org)](https://img.shields.io/travis/aconstlink/natus?label=Travis%40master)](https://travis-ci.org/github/aconstlink/natus/builds)
[![Azure DevOps builds](https://img.shields.io/azure-devops/build/aconstlink/cfc6b414-d551-4b4f-8dd7-ef0788dc1680/1?label=Azure%40master)](https://dev.azure.com/aconstlink/natus/_build?definitionId=1)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Prerequisites

Windows prerequisites:  
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
4. Open START_HERE.bat (required for finding dlls)

## Usage  
:construction:

## Roadmap
The [Projects Tab](https://github.com/aconstlink/natus/projects) provides some sort of roadmap of the project. There is no particular priority, but if you really need anything, just leave a comment or just ask to [contribute](CONTRIBUTING.md) something you would like to do.

## Contributing  
Please see [CONTRIBUTING](CONTRIBUTING.md)

## License
[MIT](https://choosealicense.com/licenses/mit/)
