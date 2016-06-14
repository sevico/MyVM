BITS 16
CodeSeg	equ	0f000h
org	0f0000h	
start:
jmp	FuncBIOSBoot


;***********************************************************************
;内部使用的函数

	
;***********************************************************************
;中断服务程序


INT_00h:
	out 50h,al
	iret
	
	
INT_01h:
	pushf
	pop	ax
	and	ax,0ffffh-020h
	push	ax
	popf
	out 51h,al
	iret
	
INT_02h:
	out 52h,al
	iret
	

INT_03h:
	out 53h,al
	iret
	
	
INT_04h:
	out 54h,al
	iret
	
	
INT_05h:
	out 55h,al
	iret
	
		
INT_06h:
	out 56h,al
	iret
	
	
INT_07h:
	out 57h,al
	iret
	
	
INT_08h:
	out 58h,al
	iret
	
	
INT_09h:
	out 59h,al
	iret
	

INT_0Ah:
	out 5Ah,al
	iret


INT_0Bh:
	out 5Bh,al
	iret


INT_0Ch:
	out 5Ch,al
	iret


INT_0Dh:
	out 5Dh,al
	iret


INT_0Eh:
	out 5Eh,al
	iret


INT_0Fh:
	out 5Fh,al
	iret

	
INT_10h:
	out	0b0h,al			;这里用了一条IO指令来代替整个INT 10H，INT10H的具体程序在Display.cpp里实现
	iret


;INT 11h，设备检测
INT_11h:
	push	ds
	push	bx
	mov	ax,0040h
	mov	ds,ax
	mov	bx,0010h	;BIOS数据区，这里存放着设备信息
	mov	ax,word  [bx]
	pop	bx
	pop	ds
	iret



INT_12h:
	push	ds
	push	bx
	mov	ax,0040h
	mov	ds,ax
	mov	bx,0013h	;BIOS数据区，在0040h:0013h的位置，大小两字节，存放着内存的大小，以KB为单位
	mov	ax,word  [bx]
	pop	bx
	pop	ds
	iret


INT_13h:
	cmp	ah,00h
	je	DiskReset
		
;	cmp	ah,02h
;	je	ReadSector
	
	;如果是其它的功能号，则在DMA.cpp里已经实现了，直接调用
	out	3,al
	iret
	
	
DiskReset:	
	clc
	mov	ah,0
	iret



INT_14h:
	out	14h,al
	iret



INT_15h:
	cmp	ah,0c0h
	je	INT_15h_0C0
	out	0b1h,al
	iret
INT_15h_0C0:
	mov	ax,CodeSeg
	mov	es,ax
	mov	bx, INT_15h_Environment -  start
	mov	ah,0
	iret
INT_15h_Environment:
	db	08h,00h,0fch,00h, 01h,0b4h,40h,00h,00h,00h
	iret



INT_16h:
	out	16h,al
	iret


INT_17h:
	out	17h,al
	iret


INT_18h:
	iret


INT_19h:
	iret


INT_1Ah:
	out	73h,al				;这里用一条IO指令调用对应的模拟程序来实现中断1Ah
	iret


INT_1Bh:
	iret


INT_1Ch:
	iret


INT_1Dh:
	iret


INT_1Eh:
	iret


INT_1Fh:
	iret
	
	
INT_NOP:
	out	0cdh,al
	iret


	

;*******************************************************************************
;BIOSBoot的编写尽量只调用BIOS中断
DataBIOSBoot:




FuncBIOSBoot:

	;初始化，置各段寄存器的值，置栈空间，复制地址表
	mov	ax,cs	
	mov	ds,ax
	mov	es,ax
	sub	ax,1000h
	mov	ss,ax	
	mov	ax,0ff00h
	mov	sp,ax
	mov	bp,ax
	mov	cx, IntTableEnd-  IntTable
	mov	di,0
	xor	ax,ax
	mov	es,ax
	mov	si, IntTable -  start
	rep movsb
	
	;把中断表其余部分也填满
	mov	cx,0d0h
OtherInt:
	mov	ax, INT_NOP -  start
	stosw
	mov	ax,CodeSeg
	stosw
	loop	OtherInt


	
	mov	ax,0040h
	mov	ds,ax
	mov	bx,0100h	;这个位置放置着启动盘盘符	
	mov	dl,byte  [bx];读取启动盘盘符	00 - A:	80 - C:
	
	mov	ax,0
	mov	es,ax
	mov	ah,02h		;读引导扇区
	mov	al,1
	mov	cx,1
	mov	dh,0
	mov	bx,7c00h
	int	13h
	
	;寄存器的初始化
	mov	ax,0
	mov	bx,0100h
	mov	bl,byte  [bx]
	xor	bh,bh
	mov	cx,ax
	inc	cx
	mov	dx,bx
	mov	ss,ax
	mov	ds,ax
	mov	es,ax
	mov	sp,ax
	sub	sp,2
	mov	bp,ax
	mov	ax,0aa55h
	
	
	;跳到引导扇区
	db	0eah,00,7ch,00,00
	
	
	
;*************************************************************************
;SYSTEM DATA - DISKETTE PARAMETERS
DisketteParameters:
db	0afh,002h,025h,002h,012h,01bh,0ffh,06ch
db	0f6h,00fh,008h,04fh,000h,004h,000h,000h
DisketteParametersEnd:
	
;*************************************************************************
;中断地址表，要复制到0x0:0x0去
DataSeg	equ	0f000h
IntTable:
dw	 INT_00h -  start,	DataSeg,  INT_01h -  start,	DataSeg,  INT_02h -  start,	DataSeg,  INT_03h -  start,	DataSeg
dw	 INT_04h -  start,	DataSeg,  INT_05h -  start,	DataSeg,  INT_06h -  start,	DataSeg,  INT_07h -  start,	DataSeg
dw	 INT_08h -  start,	DataSeg,  INT_09h -  start,	DataSeg,  INT_0Ah -  start,	DataSeg,  INT_0Bh -  start,	DataSeg
dw	 INT_0Ch -  start,	DataSeg,  INT_0Dh -  start,	DataSeg,  INT_0Eh -  start,	DataSeg,  INT_0Fh -  start,	DataSeg
dw	 INT_10h -  start,	DataSeg,  INT_11h -  start,	DataSeg,  INT_12h -  start,	DataSeg,  INT_13h -  start,	DataSeg
dw	 INT_14h -  start,	DataSeg,  INT_15h -  start,	DataSeg,  INT_16h -  start,	DataSeg,  INT_17h -  start,	DataSeg
dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_1Ah -  start,	DataSeg,  INT_NOP -  start,	DataSeg
dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  DisketteParameters -  start,DataSeg,  INT_NOP -  start, DataSeg

dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg
dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg
dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg
dw	 INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg,  INT_NOP -  start,	DataSeg

IntTableEnd:
times 65520-($-$$) db 0

;org	0ffff0h
	jmp	 FuncBIOSBoot
	
