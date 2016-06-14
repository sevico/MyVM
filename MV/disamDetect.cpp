#include "disamDetect.h"
unsigned int OutDisam=0;

void IO_Write_0900(DWORD Detect)
{
	OutDisam = Detect;

}
void disamInit()
{
	OutTable[0x0900] = (unsigned int)IO_Write_0900;

}