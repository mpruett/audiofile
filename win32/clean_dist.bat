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
rmdir /s /q .\lib
del *.ncb
del *.opt
del *.plg
rmdir /s /q .\Debug
rmdir /s /q .\Release
