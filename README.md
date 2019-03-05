# Lips
## Lua Image Processing System
Lua Image Processing System is mainly intended to be used with Embarcadero C++/Delphi.
It consists of: 
1. lua engine - lipsEngine.dll (Embarcadero 10.1. Berlin C++ project).    
2. OpenCV wrapper - ocvWrapper.dll (VS2017 project).
3. Host application - proLips.exe (Embarcadero 10.1. Berlin C++ project).

### Lua engine - lipsEngine.dll
This is the core of the project. It's written in Embarcadero C++, but it can be easily modified for other compilers. It uses LuaJit  engine and ffi to pass images structure to Lua scripts. Also, it calls ocvWrapper.dll to apply a limited set of OpenCV filters from Lua script. To use this DLL from Delphi DLL entries described in lipsEngineDLL.h muste be converted to pas file, as well as structure and typedefs in lipsGlobals.h. Lua engine currently supports contiguous memory RGB24 images (TBitmap) and TIEBItmap images (commercial product) available on www.imageen.com. Also, it supports RGB24 images with separated alpha channel. Support for RGBA32 can be added as well, but currently it's not supported.

### OpenCV wrapper - ocvWrapper.dll
This dll is written in VS2017, because bcc compiler is not supported by OpenCV. It contains API calls for setting source/target image, for setting region of interesets and a limited set of OpenCV filters and image processing routines. As this dll is called from lipsEngine, after compiling and linking ocvWrapper.dll, import library must be created:

- implib -a ocvWrapper.lib ocvWrapper.dll (for 32-bit dll)
- mkexp -p ocvWrapper.a ocvWrapper.dll (for 64-bit dll)

Import libraries must be copied in OpenCV folder under lipsEngine directory.

### Host application - projLips.exe
Host (demo) application is written in Embarcadero C++ and it contains some commercial components. The source code can be modified to use standard components. From the source code, you can see how to use lipsEngine APIs and how to parse parameters requested from Lua script.
