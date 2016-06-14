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
//ȫ�ֱ���
unsigned int MemoryStart;			//�����ڴ����ʼ��ַ
unsigned int MemorySize;			//�����ڴ�Ĵ�С
unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
unsigned int InsTable[0x100];		//ָ���
FILE *fDebug;						//������Ϣ����ļ�

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

	//*(unsigned short *)(MemoryStart+0x413)=MemorySize*1024;				//������BIOS��������������ڴ��С��ʵ���ϣ�INT 12���Ƿ���639KB
	HANDLE hBiosData = CreateFile("BIOS\\BIOSData.bin",	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long BiosdSize;
	ReadFile(hBiosData,(void *)(MemoryStart+0x400),0x100,&BiosdSize,NULL);
	CloseHandle(hBiosData);
	*(unsigned short *)(MemoryStart+0x413)=0x27f;				//��֪Ϊ�Σ�������Bochs������64MB�����λ��Ҳ�Ƿŵ�0x27F

	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\MyVM.ini");
	GetPrivateProfileString("Boot","boot","A",t,sizeof(t),t);
	switch(t[0])
	{	
	case 'A':
	case 'a':		
		*(char *)(MemoryStart+0x0400+0x0100)=0;				//�������������0040:0100���ֽڶ���Ϊϵͳ������
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
// ����	��VMShutDown
// ����	��MyVM�����ػ�ʱ���ã����Ը����������ͷ���Դ
int VMShutDown()
{
	CPUTerminate();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// ����	��VMRun
// ����	��MyVM�����к���
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



