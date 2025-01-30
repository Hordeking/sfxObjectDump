cl.exe /EHsc /DUNICODE /D_UNICODE /c /MDd SuperFXViewer.cpp /I..\include
link.exe sfxObjectDump.lib Gdi32.lib OpenGL32.lib Kernel32.lib User32.lib Shell32.lib /SUBSYSTEM:WINDOWS SuperFXViewer.obj