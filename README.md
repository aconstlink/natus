# natus 

natus is a software framework for real-time interactive audio/visual application. Ok, ok. It will be :art: :construction:

![Travis (.org)](https://img.shields.io/travis/aconstlink/natus?label=Travis%40master) 
[![Build Status](https://dev.azure.com/aconstlink/natus/_apis/build/status/aconstlink.natus?branchName=master)](https://dev.azure.com/aconstlink/natus/_build/latest?definitionId=1&branchName=master) :construction:  

## Prerequisites

Windows prerequisites:  
 - Visual Studio 2019 Community
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

## Contributing  
Please see CONTRIBUTING.md

## License
[MIT](https://choosealicense.com/licenses/mit/)
