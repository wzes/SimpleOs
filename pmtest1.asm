; ==========================================
; pmtest1.asm
<<<<<<< HEAD
; ±àÒë·½·¨£ºnasm pmtest1.asm -o pmtest1.bin
; ==========================================

%include	"pm.inc"	; ³£Á¿, ºê, ÒÔ¼°Ò»Ð©ËµÃ÷

org	07c00h
=======
; Â±Ã Ã’Ã«Â·Â½Â·Â¨Â£Âºnasm pmtest1.asm -o pmtest1.bin
; ==========================================

%include	"pm.inc"	; Â³Â£ÃÂ¿, ÂºÃª, Ã’Ã”Â¼Â°Ã’Â»ÃÂ©Ã‹ÂµÃƒÃ·

org	0100h
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
<<<<<<< HEAD
;                              ¶Î»ùÖ·,       ¶Î½çÏÞ     , ÊôÐÔ
LABEL_GDT:	   Descriptor       0,                0, 0           ; ¿ÕÃèÊö·û
LABEL_DESC_CODE32: Descriptor       0, SegCode32Len - 1, DA_C + DA_32; ·ÇÒ»ÖÂ´úÂë¶Î
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; ÏÔ´æÊ×µØÖ·
; GDT ½áÊø

GdtLen		equ	$ - LABEL_GDT	; GDT³¤¶È
GdtPtr		dw	GdtLen - 1	; GDT½çÏÞ
		dd	0		; GDT»ùµØÖ·

; GDT Ñ¡Ôñ×Ó
=======
;                              Â¶ÃŽÂ»Ã¹Ã–Â·,       Â¶ÃŽÂ½Ã§ÃÃž     , ÃŠÃ´ÃÃ”
LABEL_GDT:	   Descriptor       0,                0, 0           ; Â¿Ã•ÃƒÃ¨ÃŠÃ¶Â·Ã»
LABEL_DESC_CODE32: Descriptor       0, SegCode32Len - 1, DA_C + DA_32; Â·Ã‡Ã’Â»Ã–Ã‚Â´ÃºÃ‚Ã«Â¶ÃŽ
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; ÃÃ”Â´Ã¦ÃŠÃ—ÂµÃ˜Ã–Â·
; GDT Â½Ã¡ÃŠÃ¸

GdtLen		equ	$ - LABEL_GDT	; GDTÂ³Â¤Â¶Ãˆ
GdtPtr		dw	GdtLen - 1	; GDTÂ½Ã§ÃÃž
		dd	0		; GDTÂ»Ã¹ÂµÃ˜Ã–Â·sdsssssjjjjsafdsdfç¯‡ç»éªŒssss

; GDT Ã‘Â¡Ã”Ã±Ã—Ã“
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; END of [SECTION .gdt]

[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

<<<<<<< HEAD
	; ³õÊ¼»¯ 32 Î»´úÂë¶ÎÃèÊö·û
=======
	; Â³ÃµÃŠÂ¼Â»Â¯ 32 ÃŽÂ»Â´ÃºÃ‚Ã«Â¶ÃŽÃƒÃ¨ÃŠÃ¶Â·Ã»
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

<<<<<<< HEAD
	; Îª¼ÓÔØ GDTR ×÷×¼±¸
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt »ùµØÖ·
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt »ùµØÖ·

	; ¼ÓÔØ GDTR
	lgdt	[GdtPtr]

	; ¹ØÖÐ¶Ï
	cli

	; ´ò¿ªµØÖ·ÏßA20
=======
	; ÃŽÂªÂ¼Ã“Ã”Ã˜ GDTR Ã—Ã·Ã—Â¼Â±Â¸
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt Â»Ã¹ÂµÃ˜Ã–Â·
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt Â»Ã¹ÂµÃ˜Ã–Â·

	; Â¼Ã“Ã”Ã˜ GDTR
	lgdt	[GdtPtr]

	; Â¹Ã˜Ã–ÃÂ¶Ã
	cli

	; Â´Ã²Â¿ÂªÂµÃ˜Ã–Â·ÃÃŸA20
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
	in	al, 92h
	or	al, 00000010b
	out	92h, al

<<<<<<< HEAD
	; ×¼±¸ÇÐ»»µ½±£»¤Ä£Ê½
=======
	; Ã—Â¼Â±Â¸Ã‡ÃÂ»Â»ÂµÂ½Â±Â£Â»Â¤Ã„Â£ÃŠÂ½
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

<<<<<<< HEAD
	; ÕæÕý½øÈë±£»¤Ä£Ê½
	jmp	dword SelectorCode32:0	; Ö´ÐÐÕâÒ»¾ä»á°Ñ SelectorCode32 ×°Èë cs,
					; ²¢Ìø×ªµ½ Code32Selector:0  ´¦
; END of [SECTION .s16]


[SECTION .s32]; 32 Î»´úÂë¶Î. ÓÉÊµÄ£Ê½ÌøÈë.
=======
	; Ã•Ã¦Ã•Ã½Â½Ã¸ÃˆÃ«Â±Â£Â»Â¤Ã„Â£ÃŠÂ½
	jmp	dword SelectorCode32:0	; Ã–Â´ÃÃÃ•Ã¢Ã’Â»Â¾Ã¤Â»Ã¡Â°Ã‘ SelectorCode32 Ã—Â°ÃˆÃ« cs,
					; Â²Â¢ÃŒÃ¸Ã—ÂªÂµÂ½ Code32Selector:0  Â´Â¦
; END of [SECTION .s16]


[SECTION .s32]; 32 ÃŽÂ»Â´ÃºÃ‚Ã«Â¶ÃŽ. Ã“Ã‰ÃŠÂµÃ„Â£ÃŠÂ½ÃŒÃ¸ÃˆÃ«.
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
[BITS	32]

LABEL_SEG_CODE32:
	mov	ax, SelectorVideo
<<<<<<< HEAD
	mov	gs, ax			; ÊÓÆµ¶ÎÑ¡Ôñ×Ó(Ä¿µÄ)

	mov	edi, (80 * 11 + 79) * 2	; ÆÁÄ»µÚ 11 ÐÐ, µÚ 79 ÁÐ¡£
	mov	ah, 0Ch			; 0000: ºÚµ×    1100: ºì×Ö
	mov	al, 'P'
	mov	[gs:edi], ax

	; µ½´ËÍ£Ö¹
=======
	mov	gs, ax			; ÃŠÃ“Ã†ÂµÂ¶ÃŽÃ‘Â¡Ã”Ã±Ã—Ã“(Ã„Â¿ÂµÃ„)

	mov	edi, (80 * 11 + 79) * 2	; Ã†ÃÃ„Â»ÂµÃš 11 ÃÃ, ÂµÃš 79 ÃÃÂ¡Â£
	mov	ah, 0Ch			; 0000: ÂºÃšÂµÃ—    1100: ÂºÃ¬Ã—Ã–
	mov	al, 'P'
	mov	[gs:edi], ax

	; ÂµÂ½Â´Ã‹ÃÂ£Ã–Â¹
>>>>>>> 2ee49122d5ab24fd479852a2b1bc7bd7ac65e138
	jmp	$

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]

