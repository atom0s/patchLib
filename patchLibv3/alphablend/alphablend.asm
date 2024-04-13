.586P
.MMX
.MODEL FLAT,STDCALL 
OPTION CASEMAP:NONE 

;=======================================================================================
COMMENT $

CREDITS:
-----------------------------------------------------

- MMX CODE BY BitRAKE
- REST OF THE CODE BY UFO-PUSSY
- C++ Compile Fix By atom0s

The project file can be opened with WinAsm Studio, availabe at: www.winasm.net

$
;=======================================================================================



;=======================================================================================
; Standard Headers
;=======================================================================================

INCLUDE \MASM32\INCLUDE\WINDOWS.INC 
INCLUDE \MASM32\INCLUDE\USER32.INC
INCLUDE \MASM32\INCLUDE\KERNEL32.INC
INCLUDE \MASM32\INCLUDE\GDI32.INC 
INCLUDE \MASM32\MACROS\MACROS.ASM 

;=======================================================================================
; Function Prototypes
;=======================================================================================

AlphaBlend2 				PROTO C :ULONG,:HBITMAP,:ULONG,:ULONG,:ULONG,:ULONG,:HBITMAP,:ULONG,:ULONG,:BOOL
AlphaBlendScroller 			PROTO C :ULONG,:HBITMAP,:ULONG,:ULONG,:ULONG,:ULONG,:HBITMAP,:ULONG,:ULONG,:BOOL,:ULONG,:ULONG,:ULONG
PrintText					PROTO C :HDC,:HBITMAP,:HDC,:HDC,:LPCSTR,:ULONG,:ULONG


;=======================================================================================
; Initialized Data
;=======================================================================================

.DATA
ALIGN 16   
qwAlphaMask				QWORD 	0FF000000FF000000h
qwRGBMask				QWORD 	000FFFFFF00FFFFFFh
qwFadingMask				QWORD 	000FFFFFF00FFFFFFh

;=======================================================================================
; Uninitialized Data
;=======================================================================================
.DATA?
ALIGN 16   
dwSine					DWORD 	?
r4Angle					REAL4 	?
r4AngleConst				REAL4	?


.CODE

ALIGN 16   
AlphaBlend2 PROC C USES EBX ESI EDI ulSrcWndWidth:ULONG, hSrcBMP:HBITMAP,ulSrcX:ULONG,ulSrcY:ULONG,ulSrcWidth:ULONG,ulSrcHeight:ULONG,hDestBMP:HBITMAP,ulDestX:ULONG,ulDestY:ULONG,bFade:BOOL 

         LOCAL BMPSource		:BITMAP
         LOCAL BMPDestination	:BITMAP
         LOCAL srci			:DWORD
         LOCAL dsti			:DWORD
         LOCAL ScrWidth		:DWORD
	LOCAL dwScreenWidth	:DWORD
	
 .CODE    
 
         INVOKE GetObject,[hSrcBMP],SIZEOF BMPSource,ADDR BMPSource
         INVOKE GetObject,[hDestBMP],SIZEOF BMPDestination,ADDR BMPDestination
         MOV EAX,DWORD PTR [BMPSource.bmWidth]
         SUB EAX,DWORD PTR [ulSrcWidth]
         SHL EAX,2
         MOV DWORD PTR [srci],EAX
         MOV EAX,DWORD PTR [BMPDestination.bmWidth]
         SUB EAX,DWORD PTR [ulSrcWidth]
         SHL EAX,2
         MOV DWORD PTR [dsti],EAX
         MOV EAX,DWORD PTR [BMPDestination.bmWidth]
         MOV EDX,DWORD PTR [BMPDestination.bmHeight]
         SUB EDX,DWORD PTR [ulDestY]
         SUB EDX,DWORD PTR [ulSrcHeight]
         MUL EDX
         ADD EAX,DWORD PTR [ulDestX]
         SHL EAX,2
         ADD EAX,DWORD PTR [BMPDestination.bmBits]
         MOV EDX,EAX
         PUSH EDX
         MOV EAX,DWORD PTR [BMPSource.bmWidth]
         MOV EDX,DWORD PTR [BMPSource.bmHeight]
         SUB EDX,DWORD PTR [ulSrcY]
         SUB EDX,DWORD PTR [ulSrcHeight]
         MUL EDX
         ADD EAX,DWORD PTR [ulSrcX]
         SHL EAX,2
         ADD EAX,DWORD PTR [BMPSource.bmBits]
         POP EDX
         SHR DWORD PTR [ulSrcWidth],1
         
@@AlphaBlendLoop:

         MOV ECX,DWORD PTR [ulSrcWidth]
         DEC ECX 
         PXOR MM7,MM7 
         MOVQ MM0,QWORD PTR [EAX+ECX*8] 
         MOVQ MM1,QWORD PTR [EDX+ECX*8]
     @@:
	.IF (bFade == TRUE)
		m2m [dwScreenWidth],[ulSrcWndWidth]
		SHR DWORD PTR [dwScreenWidth], 1
		SUB DWORD PTR [dwScreenWidth], 12
		.IF( ECX >= SDWORD PTR [dwScreenWidth] )
			PAND MM0,qwFadingMask
			ADD BYTE PTR [qwFadingMask+3],011h
			ADD BYTE PTR [qwFadingMask+7],011h
		.ELSEIF (ECX < 12)
			SUB BYTE PTR [qwFadingMask+3],011h
			SUB BYTE PTR [qwFadingMask+7],011h
			PAND MM0,QWORD PTR [qwFadingMask]
            	.ELSE
                 		PAND MM0,QWORD PTR [qwFadingMask]
             	.ENDIF
         .ENDIF
         MOVQ MM4,MM0
         MOVQ MM6,MM0
        ;PSRLW MM6,1; OPTIONAL EFFECT
         PADDUSB MM6,MM1
         PAND MM6,QWORD PTR [qwAlphaMask]
         MOVQ MM2,MM0 
         PSRLW MM4,1 
         MOVQ MM3,MM1 
         MOVQ MM5,MM4 
         PUNPCKLBW MM0,MM7 
         PUNPCKLBW MM1,MM7 
         PUNPCKHBW MM2,MM7 
         PUNPCKHBW MM3,MM7 
         PSUBSW MM0,MM1 
         PSUBSW MM2,MM3 
         PUNPCKLWD MM4,MM4 
         PUNPCKHWD MM5,MM5 
         PUNPCKHDQ MM4,MM4 
         PUNPCKHDQ MM5,MM5
         PSLLW MM0,1 
         PSLLW MM2,1 
         PMULHW MM0,MM4 
         PMULHW MM2,MM5
         PADDSW MM0,MM1 
         PADDSW MM2, MM3 
         PACKUSWB MM0, MM2
         PAND MM0,QWORD PTR [qwRGBMask]
         POR MM0,MM6
        ; stored alpha = (-dA * sA)/256 + dA (better!?)
         MOVQ QWORD PTR [EDX+ECX*8],MM0
         DEC ECX 
         JS @F 
         
         MOVQ MM0,QWORD PTR [EAX+ECX*8] 
         MOVQ MM1,QWORD PTR [EDX+ECX*8] 
         JNS @B
         
     @@:
         MOV ECX, DWORD PTR [ulSrcWidth]
         SHL ECX,3
         ADD EAX,ECX
         ADD EDX,ECX    
         ADD EAX,DWORD PTR [srci]
         ADD EDX,DWORD PTR [dsti]
         DEC DWORD PTR [ulSrcHeight]
         
         JNZ @@AlphaBlendLoop
         
         RET
     
AlphaBlend2 ENDP

ALIGN 16   
AlphaBlendScroller PROC C USES EBX ESI EDI ulSrcWndWidth:ULONG,hSrcBMP:HBITMAP,ulSrcX:ULONG,ulSrcY:ULONG,ulSrcWidth:ULONG,ulSrcHeight:ULONG,hDestBMP:HBITMAP,ulDestX:ULONG,ulDestY:ULONG,bFade:BOOL,Frequency:ULONG,Sine_Speed:ULONG,Amplitude:ULONG

	LOCAL BMPSource		:BITMAP	
	LOCAL BMPDestination	:BITMAP
	LOCAL srci			:DWORD
	LOCAL dsti			:DWORD
	LOCAL ScrWidth		:DWORD
	LOCAL dwScreenWidth	:DWORD

.CODE

	MOV ESI, DWORD PTR [Sine_Speed]
	MOV EDI, DWORD PTR [Frequency]

	FINIT
	FLD DWORD PTR [r4AngleConst]
;	FADD Sine_Speed
	FADD DWORD PTR [ESI]
	FST DWORD PTR [r4AngleConst]
	FSTP DWORD PTR [r4Angle]
	FLD DWORD PTR [r4Angle]
	FSIN
	FIMUL DWORD PTR [Amplitude]
	FISTP DWORD PTR [dwSine]
	FWAIT

	INVOKE GetObject,hSrcBMP,SIZEOF BMPSource,ADDR BMPSource
	INVOKE GetObject,hDestBMP,SIZEOF BMPDestination,ADDR BMPDestination
	MOV EAX,DWORD PTR [BMPSource.bmWidth]
	SUB EAX,DWORD PTR [ulSrcWidth]
	SHL EAX,2
	MOV DWORD PTR [srci],EAX
	MOV EAX,DWORD PTR [BMPDestination.bmWidth]
	SUB EAX,DWORD PTR [ulSrcWidth]
	SHL EAX,2
	MOV DWORD PTR [dsti],EAX
	MOV EAX,DWORD PTR [BMPDestination.bmWidth]
	MOV EDX,DWORD PTR [BMPDestination.bmHeight]
	SUB EDX,DWORD PTR [ulDestY]
	SUB EDX,DWORD PTR [ulSrcHeight]
	MUL EDX
	ADD EAX,DWORD PTR [ulDestX]
	SHL EAX,2
	ADD EAX,DWORD PTR [BMPDestination.bmBits]
	MOV EDX,EAX
	PUSH EDX
	MOV EAX,DWORD PTR [BMPSource.bmWidth]
	MOV EDX,DWORD PTR [BMPSource.bmHeight]
	SUB EDX,DWORD PTR [ulSrcY]
	SUB EDX,DWORD PTR [ulSrcHeight]
	MUL EDX
	ADD EAX,DWORD PTR [ulSrcX]
	SHL EAX,2
	ADD EAX,DWORD PTR [BMPSource.bmBits]
	POP EDX
	SHR DWORD PTR [ulSrcWidth],1
	
@@AlphaBlendLoop: 

	MOV ECX,DWORD PTR [ulSrcWidth] ; number of pixels/2 
	DEC ECX 
	PXOR MM7,MM7 
	MOVQ MM0,QWORD PTR [EAX+ECX*8] 
	MOVQ MM1,QWORD PTR [EDX+ECX*8]
@@:
	PXOR MM7,MM7 
	.IF (bFade == TRUE)
		m2m [dwScreenWidth], [ulSrcWndWidth]
		SHR DWORD PTR [dwScreenWidth], 1
		SUB DWORD PTR [dwScreenWidth], 12
		.IF( ECX >= SDWORD PTR [dwScreenWidth] )
			PAND MM0,QWORD PTR [qwFadingMask]
			ADD BYTE PTR [qwFadingMask+3],011h
			ADD BYTE PTR [qwFadingMask+7],011h
		.ELSEIF (ECX < 12)
			SUB BYTE PTR [qwFadingMask+3],011h
			SUB BYTE PTR [qwFadingMask+7],011h
			PAND MM0,QWORD PTR [qwFadingMask]
		.ELSE
			PAND MM0,QWORD PTR [qwFadingMask]
		.ENDIF
	.ENDIF
	
	MOVQ MM4,MM0
	MOVQ MM6,MM0
	;PSRLW MM6,1 ; OPTIONAL EFFECT
	PADDUSB MM6,MM1
	PAND MM6,QWORD PTR [qwAlphaMask]
	MOVQ MM2,MM0 
	PSRLW MM4,1 
	MOVQ MM3,MM1 
	MOVQ MM5,MM4 
	PUNPCKLBW MM0,MM7 
	PUNPCKLBW MM1,MM7 
	PUNPCKHBW MM2,MM7 
	PUNPCKHBW MM3,MM7 
	PSUBSW MM0,MM1 
	PSUBSW MM2,MM3 
	PUNPCKLWD MM4,MM4 
	PUNPCKHWD MM5,MM5 
	PUNPCKHDQ MM4,MM4 
	PUNPCKHDQ MM5,MM5
	PSLLW MM0,1 
	PSLLW MM2,1 
	PMULHW MM0,MM4 
	PMULHW MM2,MM5
	PADDSW MM0,MM1 
	PADDSW MM2,MM3 
	PACKUSWB MM0,MM2
	PAND MM0,QWORD PTR [qwRGBMask]
	POR MM0,MM6
	; STORED ALPHA = (-DA * SA)/256 + DA (BETTER!?)
	MOVQ [EDX+ECX*8],MM0
	EMMS

	FINIT
	FLD [r4Angle]
;	FADD Frequency
	FADD DWORD PTR[EDI]
	FSTP [r4Angle]
	FLD [r4Angle]
	FSIN
	FIMUL DWORD PTR [Amplitude]
	FISTP DWORD PTR [dwSine]
	FWAIT
	PUSH EAX
	MOV EAX,DWORD PTR [BMPDestination.bmWidth]
	MOV EDX,DWORD PTR [BMPDestination.bmHeight]
	SUB EDX,DWORD PTR [ulDestY]
	SUB EDX,DWORD PTR [ulSrcHeight]
	ADD EDX,DWORD PTR [dwSine]
	MUL EDX
	ADD EAX,DWORD PTR [ulDestX]
	SHL EAX,2
	ADD EAX,DWORD PTR [BMPDestination.bmBits]
	ADD EDX,EAX
	POP EAX
	DEC ECX 
	
	JS @F
	
	MOVQ MM0,QWORD PTR [EAX+ECX*8] 
	MOVQ MM1,QWORD PTR [EDX+ECX*8] 
	
	JNS @B
	
@@:
	MOV ECX,DWORD PTR [ulSrcWidth]
	SHL ECX,3
	ADD EAX,ECX
	ADD EDX,ECX    
	ADD EAX,DWORD PTR [srci]
	ADD EDX,DWORD PTR [dsti]
	FINIT
	FLD [r4AngleConst]
	FSTP [r4Angle]
	FWAIT
	DEC DWORD PTR [ulSrcHeight]
	
	JNZ @@AlphaBlendLoop
    
	RET

AlphaBlendScroller endp

ALIGN 16
PaintText PROC C USES EBX ESI EDI hTmpDC:HDC,hTmpBMP:HBITMAP,hOutDC:HDC,hOutBMP:HDC,lpString:LPCSTR,ulX:ULONG,ulY:ULONG
	
	LOCAL iStringLength	:DWORD
	LOCAL TxtSize[2]		:DWORD
	LOCAL NextLine		:DWORD
	LOCAL StartOfs		:DWORD
	LOCAL pSrc 			:DWORD
	LOCAL pDest			:DWORD
	LOCAL bminfo			:BITMAP

.CODE

	INVOKE lstrlen, DWORD PTR [lpString]
	.IF (EAX==0)
		RET
	.ELSE
		MOV DWORD PTR [iStringLength],EAX
	.ENDIF
	
	INVOKE GetTextExtentPoint,[hTmpDC],[lpString],[iStringLength],ADDR TxtSize
;
	INVOKE GetObject,[hTmpBMP],SIZEOF bminfo,ADDR bminfo
	MOV ESI,DWORD PTR [bminfo.bmBits]
	MOV DWORD PTR [pSrc],ESI
	INVOKE GetObject,[hOutBMP],SIZEOF bminfo,ADDR bminfo
	MOV EDI,DWORD PTR [bminfo.bmBits]
	MOV DWORD PTR [pDest],EDI
	MOV EAX,DWORD PTR [bminfo.bmWidth]
	SUB EAX,TxtSize
	SHL EAX,2
	MOV NextLine,EAX
	MOV EAX,DWORD PTR [bminfo.bmWidth]
	MOV EDX,DWORD PTR [bminfo.bmHeight]
	SUB EDX,ulY
	SUB EDX,TxtSize+4
	MUL EDX
	ADD EAX,ulX
	SHL EAX,2
	MOV StartOfs,EAX

; Correct the alpha values in the destination DC
	INVOKE SetBkMode,[hTmpDC],OPAQUE
	INVOKE TextOut,[hTmpDC],[ulX],[ulY],[lpString],[iStringLength]
	ADD ESI,StartOfs
	ADD EDI,StartOfs
	ADD EDI,3
	MOV EBX,TxtSize+4
	
@@yLoop:

	MOV ECX,TxtSize
	
@@xLoop:

	MOV EAX,DWORD PTR [ESI]
	MOV EDX,EAX
	ROL EDX,16
	ROR EAX,8
	AND EAX,0FFFF00FFh
	ADD DX,AX
	SHR EDX,24
	ADD AX,DX
	AND EAX,0FFFFh
	XOR EDX,EDX
	PUSH EBX
	MOV EBX,3
	DIV EBX
	POP EBX
	NOT AL
	.IF (AL>BYTE PTR [EDI])
		MOV BYTE PTR [EDI],AL
	.ENDIF
	ADD ESI,4
	ADD EDI,4
	DEC ECX
	
	JNZ @@xLoop
	
	ADD ESI,NextLine
	ADD EDI,NextLine
	DEC EBX
	
	JNZ @@yLoop
		
	INVOKE BitBlt,[hTmpDC],[ulX],[ulY],TxtSize,TxtSize+4,[hOutDC],[ulX],[ulY],SRCCOPY
	INVOKE SetBkMode,[hTmpDC],TRANSPARENT
	;INVOKE SetTextColor,[hTmpDC],0443333h
	INVOKE TextOut,[hTmpDC],[ulX],[ulY],[lpString],iStringLength
	MOV ESI,DWORD PTR [pSrc]
	MOV EDI,DWORD PTR [pDest]
	ADD ESI,DWORD PTR [StartOfs]
	ADD EDI,DWORD PTR [StartOfs]
	MOV EBX,TxtSize+4
	
@@yLoop2:

	MOV ECX,TxtSize
	
@@xLoop2:

	MOV EAX,[ESI]
	MOV EDX,[EDI]
	AND EDX,0FF000000h
	OR EAX,EDX
	MOV DWORD PTR [EDI],EAX
	ADD ESI,4
	ADD EDI,4
	DEC ECX
	
	JNZ @@xLoop2
	
	ADD ESI,DWORD PTR [NextLine]
	ADD EDI,DWORD PTR [NextLine]
	DEC EBX
	
	JNZ @@yLoop2	
	
	RET

PaintText ENDP
ALIGN 16  
  
END