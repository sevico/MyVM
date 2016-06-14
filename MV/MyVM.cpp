#include "stdafx.h"
#include "cpu/cpu.h"
#include "io/Display.h"
#include "io/PIC.h"
#include "io/SystemTimer.h"
#include "io/RTC.h"
#include "io/Floppy.h"
#include "io/HardDisk.h"
#include "io/DMA.h"
#include "io/Printer.h"
#include "io/Serial.h"
#include "io/KeyBoard.h"
#include "disamDetect.h"
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//全局变量
unsigned int MemoryStart;			//虚拟内存的起始地址
unsigned int MemorySize;			//虚拟内存的大小
unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表
unsigned int InsTable[0x100];		//指令表
FILE *fDebug;						//调试信息输出文件

//////////////////////////////////////////////////////////////////////////
int VMInit()
{
	CPUInit();

//	fDebug=fopen("Debug.txt","w");

	fclose(fopen("out.txt", "w"));


	char t[80];
	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\MyVM.ini");
	GetPrivateProfileString("Memory","Size","1",t,sizeof(t),t);
	MemorySize=atoi(t);
	MemoryStart=(unsigned int)malloc(MemorySize*1024*1024);	
	memset((void *)MemoryStart,0,MemorySize*1024*1024);

	//*(unsigned short *)(MemoryStart+0x413)=MemorySize*1024;				//这里是BIOS数据区，存放着内存大小，实际上，INT 12总是返回639KB
	HANDLE hBiosData = CreateFile("BIOS\\BIOSData.bin",	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long BiosdSize;
	ReadFile(hBiosData,(void *)(MemoryStart+0x400),0x100,&BiosdSize,NULL);
	CloseHandle(hBiosData);
	*(unsigned short *)(MemoryStart+0x413)=0x27f;				//不知为何，就算是Bochs里设了64MB，这个位置也是放的0x27F

	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\MyVM.ini");
	GetPrivateProfileString("Boot","boot","A",t,sizeof(t),t);
	switch(t[0])
	{	
	case 'A':
	case 'a':		
		*(char *)(MemoryStart+0x0400+0x0100)=0;				//设成软盘启动，0040:0100该字节定义为系统启动盘
		break;
	case 'B':
	case 'b':
		*(char *)(MemoryStart+0x0400+0x0100)=1;
		break;
	default:
		*(char *)(MemoryStart+0x0400+0x0100)=toupper(t[0])-'C'+0x80;
		break;
	}

	HANDLE hBIOS = CreateFile("BIOS\\B.bin",	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long BiosSize;
	ReadFile(hBIOS,(void *)(MemoryStart+0xf0000),0x100000-0xf0000,&BiosSize,NULL);
	CloseHandle(hBIOS);

	DispInit();
	PICInit();
	RTCInit();
	FloppyInit();
	HardDiskInit();
	DMAInit();
	PrinterInit();
	SerialInit();
	disamInit();
	KeyBoardInit();
	SystemTimerInit();

	
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// 名称	：VMShutDown
// 功能	：MyVM正常关机时调用，可以给各个部件释放资源
int VMShutDown()
{
	CPUTerminate();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 名称	：VMRun
// 功能	：MyVM的运行函数
int VMRun()
{
	CPURun();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void eVMerror(char *err)
{
	MessageBox(NULL,err,"MyVM",MB_OK|MB_APPLMODAL|MB_TOPMOST);
	VMShutDown();
	exit(0);
}

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	VMInit();
	
	VMRun();

	VMShutDown();
	return 0;
}



