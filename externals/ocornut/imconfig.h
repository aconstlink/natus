
#pragma once

#if defined( NATUS_BUILD_DLL )
#if defined( NATUS_IMGUI_EXPORT )
#define IMGUI_API __declspec( dllexport )
#else
#define IMGUI_API __declspec( dllimport )
#endif
#else
#endif