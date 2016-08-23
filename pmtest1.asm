; ==========================================
; pmtest1.asm
; ±àÒë·½·¨£ºnasm pmtest1.asm -o pmtest1.bin
; ==========================================

%include	"pm.inc"	; ³£Á¿, ºê, ÒÔ¼°Ò»Ð©ËµÃ÷

org	0100h
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
;                              ¶Î»ùÖ·,       ¶Î½çÏÞ     , ÊôÐÔ
LABEL_GDT:	   Descriptor       0,                0, 0           ; ¿ÕÃèÊö·û
LABEL_DESC_CODE32: Descriptor       0, SegCode32Len - 1, DA_C + DA_32; ·ÇÒ»ÖÂ´úÂë¶Î
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; ÏÔ´æÊ×µØÖ·
; GDT ½áÊø

GdtLen		equ	$ - LABEL_GDT	; GDT³¤¶È
GdtPtr		dw	GdtLen - 1	; GDT½çÏÞ
		dd	0		; GDT»ùµØÖ·sdsssssjjjjsafdsdf篇经验ssss

; GDT Ñ¡Ôñ×Ó
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

	; ³õÊ¼»¯ 32 Î»´úÂë¶ÎÃèÊö·û
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

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
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; ×¼±¸ÇÐ»»µ½±£»¤Ä£Ê½
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; ÕæÕý½øÈë±£»¤Ä£Ê½
	jmp	dword SelectorCode32:0	; Ö´ÐÐÕâÒ»¾ä»á°Ñ SelectorCode32 ×°Èë cs,
					; ²¢Ìø×ªµ½ Code32Selector:0  ´¦
; END of [SECTION .s16]


[SECTION .s32]; 32 Î»´úÂë¶Î. ÓÉÊµÄ£Ê½ÌøÈë.
[BITS	32]

LABEL_SEG_CODE32:
	mov	ax, SelectorVideo
	mov	gs, ax			; ÊÓÆµ¶ÎÑ¡Ôñ×Ó(Ä¿µÄ)

	mov	edi, (80 * 11 + 79) * 2	; ÆÁÄ»µÚ 11 ÐÐ, µÚ 79 ÁÐ¡£
	mov	ah, 0Ch			; 0000: ºÚµ×    1100: ºì×Ö
	mov	al, 'P'
	mov	[gs:edi], ax

	; µ½´ËÍ£Ö¹
	jmp	$

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]

