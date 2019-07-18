rem VISUALTEXT Generated KB batch file.
pushd d:\apps\noop\kb
del /q kb.dep
nmake /f kb.mak CFG="kb - Win32 Debug"
popd
