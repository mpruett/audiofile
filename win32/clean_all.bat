nmake -f sfinfo.mak CFG="sfinfo - Win32 Debug" CLEAN
nmake -f sfinfo.mak CFG="sfinfo - Win32 Release" CLEAN
nmake -f sfinfo.mak CFG="sfinfo - Win32 Debug using DLL" CLEAN
nmake -f sfinfo.mak CFG="sfinfo - Win32 Release using DLL" CLEAN
nmake -f sfconvert.mak CFG="sfconvert - Win32 Debug" CLEAN
nmake -f sfconvert.mak CFG="sfconvert - Win32 Release" CLEAN
nmake -f sfconvert.mak CFG="sfconvert - Win32 Debug using DLL" CLEAN
nmake -f sfconvert.mak CFG="sfconvert - Win32 Release using DLL" CLEAN
nmake -f staticlib.mak CFG="staticlib - Win32 Debug" CLEAN
nmake -f staticlib.mak CFG="staticlib - Win32 Release" CLEAN
nmake -f dynamiclib.mak CFG="dynamiclib - Win32 Debug" CLEAN
nmake -f dynamiclib.mak CFG="dynamiclib - Win32 Release" CLEAN
del .\lib\*.lib
cd .\Debug
del /q /s *.obj
del /q /s *.sbr
del /q /s *.dep
del /q /s *.lib
del /q /s *.idb
del /q /s *.pdb
del /q /s *.bsc
del /q /s *.pch
cd ..\Release
del /q /s *.obj
del /q /s *.sbr
del /q /s *.dep
del /q /s *.lib
del /q /s *.idb
del /q /s *.pdb
del /q /s *.bsc
del /q /s *.pch
