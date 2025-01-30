:: Open either a x64 or x86 Native Tools Command Prompt for VS 2022, then run buildLib.bat

cl.exe /EHsc /MDd /c -I..\include *.cpp
lib.exe *.obj /OUT:sfxObjectDump.lib