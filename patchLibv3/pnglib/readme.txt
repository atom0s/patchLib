-------------------------------------------------------------------
 PNGlib version 1.0
 Beta release
 (C) 2002 by Thomas Bleeker
 PNGlib@MadWizard.org
 http://www.MadWizard.org
-------------------------------------------------------------------

This is the beta releas of PNGlib, a free library written in 
assembly to read and display PNG images in your programs. 
Includes and headers for use in MASM32 as well as (visual) C
are included. 

Directory listing:

[bin]
   Normal and safe build of the PNGlib, in binary .lib format
   Include file for use with MASM32
   Header file for use in visual C
[C samples]
   Visual C examples of using the PNGlib
[MASM samples]
   MASM32 samples of using the PNGlib
[docs]
   The PNGlib documentation, in HTMLhelp format
[PNGview]
   Executable (\Release) and source of a 29KB PNG viewer, 
   written by me using the PNGlib
[source]
   The full source of PNGlib



Thomas, PNGlib@MadWizard.org




-------------------------------------------------------------------
 Custom Compile Note - by atom0s

 This copy of pnglib has cdebug.asm and CRC.asm removed
 from the lib. These files cause linker warnings with
 Visual Studio 2010 because the files contain no exported
 symbols based on compile settings.

 These two files are only needed in DEBUG and SAFE versions
 of pnglib which patchLib does not use.

 ~atom0s
-------------------------------------------------------------------