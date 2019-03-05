# Lips
## Lua Image Processing System
Lua Image Processing System is mainly intended to be used with Embarcadero C++/Delphi.
It consists of: 
1. lua engine - lipsEngine.dll (Embarcadero 10.1. Berlin C++ project).    
2. OpenCV wrapper - ocvWrapper.dll (VS2017 project).
3. Host application - proLips.exe (Embarcadero 10.1. Berlin C++ project).
### Lua engine - lipsEngine.dll
This is the core of the project. It's written in Embarcadero C++, but it can be easily modified for other compilers. It uses LuaJit  engine and ffi to pass images structure to lua scripts. Also, it calls ocvWrapper.dll to apply a limited set of OpenCV filters from lua script.
