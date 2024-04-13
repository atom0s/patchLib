del *.obj
del *.lib

c:\masm32\bin\ml /c /ID:\masm32\includeex\ /coff *.asm
c:\masm32\bin\lib /subsystem:windows /NODEFAULTLIB /out:alphablend.lib *.obj

pause

del *.obj