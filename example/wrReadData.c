//******************************************************************************
// ���������� ��������� ��� ����� ������ � �� �� ������ USB3000,
// ��������� ��� ���� ����������-�������� wrRtusbapi.dll
//******************************************************************************
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "wrRtusbapi.h"

// ����� �� ���������
void TerminateApplication(char *ErrorString, BOOL TerminationFlag);
// �������� ���������� ���������� ������� �� ���� ������
BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv);
// ����������� ������ ���������� ���������
void ShowThreadErrorMessage(void);

// ������� ������ ����� ������ � ������
DWORD 	WINAPI ServiceReadThread(PVOID /*Context*/);
// ������������� ������ �����
HANDLE 	hReadThread;
DWORD 	ReadTid;

// ������������� ����� ��� ������ ���������� ������
HANDLE hFile;

// ������� ������ ���������� Rtusbapi.dll
DWORD DllVersion;
// ��������� �� ��������� ������
LPVOID pModule;
// ���������� ����������
HANDLE hModule;
// ����� ������
HANDLE ModuleHandle;
// �������� ������
char ModuleName[10];
// �������� ������ ���� USB
BYTE UsbSpeed;
// �������� ����� ������
char ModuleSerialNumber[9];
// ������ �������� AVR
char AvrVersion[5];
// ���������, ���������� ���������� � ������ �������� DSP
DSP_INFO_USB3000 di;
// ��������� ���������� � ���� ������
FLASH_USB3000 fi;
// ��������� ���������� ������ ���
INPUT_PARS_USB3000 ip;

// ����������� ��������� ���-�� ������������ ����������� ������
const WORD MaxVirtualSoltsQuantity = 127;
// �������  ����� ������
const double ReadRate = 3000.0;

//max ��������� ���-�� ������������ �������� (������� 32) ��� �. ReadData � WriteData()
DWORD DataStep = 1024*1024;
// ������� ������ �� DataStep �������� ����� ������� � ����
const WORD NBlockRead = 20;
// ��������� �� ����� ��� �������� ������
SHORT *ReadBuffer;

// �������� �������-���������
DWORD Counter = 0x0, OldCounter = 0xFFFFFFFF;

// ����� ������ ��� ���������� ������ ����� ������
WORD ThreadErrorNumber;
// ������ ���������� ������� ����� ������
BOOL IsThreadComplete = FALSE;

SHORT AdcSample;

//------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------
void main(void)
{
	char String[128];
	WORD i;
	DWORD FileBytesWritten;

	// �������� ����� ��������	
	system("cls");

	printf(" ***********************************************************************\n");
	printf(" Data Reading Console Example for USB3000 unit using apper Rtusbapi \n");
	printf(" ***********************************************************************\n\n");

	// �������� ������ ������������ ���������� Rtusbapi.dll
	if((DllVersion = GetDllVersion_Rtusbapi()) != CURRENT_VERSION_RTUSBAPI) 
	{	
		sprintf(String, " Rtusbapi.dll Version Error!!!\n   Current: %1u.%1u. Required: %1u.%1u",
											DllVersion >> 0x10, DllVersion & 0xFFFF,
											CURRENT_VERSION_RTUSBAPI >> 0x10, CURRENT_VERSION_RTUSBAPI & 0xFFFF);
		
		TerminateApplication(String, TRUE);
	}		
	else printf(" Rtusbapi.dll Version --> OK\n");

	// ������� ��������� �� ��������� ������ USB3000
	pModule = (LPVOID *)CreateInstance_Rtusbapi("usb3000");
	if(!pModule) TerminateApplication(" Module Interface --> Bad\n", TRUE);
	else printf(" Module Interface --> OK\n");

	// ��������� ���������� ������ USB3000 � ������ 127 ����������� ������
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++) if(OpenDevice_Usb3000(pModule, i)) break;
	// ���-������ ����������?
	if(i == MaxVirtualSoltsQuantity) TerminateApplication(" Can't find module USB3000 in first 127 virtual slots!\n", TRUE);
	else printf(" OpenDevice_Usb3000(%u) --> OK\n", i);

	// ��������� �������� ���������� (handle) ����������
	hModule = GetModuleHandle_Usb3000(pModule);
	if(hModule == INVALID_HANDLE_VALUE) TerminateApplication(" Can't get module handle!\n", TRUE);
	else printf(" GetModuleHandle_Usb3000() --> OK\n");

	// ��������� �������� ������������� ������ 
	if(!GetModuleName_Usb3000(pModule, ModuleName)) TerminateApplication(" GetModuleName_Usb3000() --> Bad\n", TRUE);
	else printf(" GetModuleName_Usb3000() --> OK\n");
	// ��������, ��� ��� 'USB3000'
	if(strcmp(ModuleName, "USB3000")) TerminateApplication(" The module is not 'USB3000'\n", TRUE);
	else printf(" The module is 'USB3000'\n");

	// ������ ������� �������� ������ ���� USB20
	if(!GetUsbSpeed_Usb3000(pModule, &UsbSpeed)) { printf(" GetUsbSpeed_Usb3000() --> Bad\n"); exit(1); }
	else printf(" GetUsbSpeed_Usb3000() --> OK\n");
	// ������ ��������� ������ �������� AVR
	printf(" USB Speed is %s\n", UsbSpeed ? "HIGH (480 Mbit/s)" : "FULL (12 Mbit/s)");

	// ��������� �������� ����� ������
	if(!GetModuleSerialNumber_Usb3000(pModule, ModuleSerialNumber)) TerminateApplication(" GetModuleSerialNumber_Usb3000() --> Bad\n", TRUE);
	else printf(" GetModuleSerialNumber_Usb3000() --> OK\n");
	// ������ ��������� �������� ����� ������
	printf(" Module Serial Number is %s\n", ModuleSerialNumber);

	// ��������� ������ �������� AVR
	if(!GetAvrVersion_Usb3000(pModule, AvrVersion)) TerminateApplication(" GetAvrVersion_Usb3000() --> Bad\n", TRUE);
	else printf(" GetAvrVersion_Usb3000() --> OK\n");
	// ������ ��������� ������ �������� AVR
	printf(" Avr Driver Version is %s\n", AvrVersion);

	// ��� �������� DSP ������ �� ���������������� ������� ������� DLL ����������
	if(!LOAD_DSP_Usb3000(pModule, NULL)) TerminateApplication(" LOAD_DSP_Usb3000() --> Bad\n", TRUE);
	else printf(" LOAD_DSP_Usb3000() --> OK\n");

	// �������� �������� ������
 	if(!MODULE_TEST_Usb3000(pModule)) TerminateApplication(" MODULE_TEST_Usb3000() --> Bad\n", TRUE);
	else printf(" MODULE_TEST_Usb3000() --> OK\n");

	// ������� ������ ������������ �������� DSP
	if(!GET_DSP_INFO_Usb3000(pModule, &di)) TerminateApplication(" GET_DSP_INFO_Usb3000() --> Bad\n", TRUE);
	else printf(" GET_DSP_INFO_Usb3000() --> OK\n");
	// ������ ��������� ������ ������������ �������� DSP
	printf(" DSP Driver version is %1u.%1u\n", di.DspMajor, di.DspMinor);

	// ����������� ����������������� ���� size ��������� RTUSB3000::FLASH
	fi.size = sizeof(FLASH_USB3000);
	// ������� ���������� �� ���� ������
	if(!GET_FLASH_Usb3000(pModule, &fi)) TerminateApplication(" GET_FLASH_Usb3000() --> Bad\n", TRUE);
	else printf(" GET_FLASH_Usb3000() --> OK\n");

	// ����������� ����������������� ���� size ��������� RTUSB3000::INPUT_PARS
	ip.size = sizeof(INPUT_PARS_USB3000);
	// ������� ������� ��������� ������ ���
	if(!GET_INPUT_PARS_Usb3000(pModule, &ip)) TerminateApplication(" GET_INPUT_PARS_Usb3000() --> Bad\n", TRUE);
	else printf(" GET_INPUT_PARS_Usb3000() --> OK\n");

	// ��������� �������� ��������� ���
	ip.CorrectionEnabled = TRUE;				// �������� ������������� �������� ������
	ip.InputClockSource = INTERNAL_INPUT_CLOCK_USB3000;	// ����� ������������ ���������� �������� �������� ��� ����� ������
//	ip.InputClockSource = EXTERNAL_INPUT_CLOCK_USB3000;	// ����� ������������ ������� �������� �������� ��� ����� ������
	ip.SynchroType = NO_SYNCHRO_USB3000;			// �� ����� ������������ ������� ������������� ��� ����� ������ 
//	ip.SynchroType = TTL_START_SYNCHRO_USB3000;	// ����� ������������ �������� ������������� ������ ��� ����� ������  
	ip.ChannelsQuantity = 0x4;					// ������ �������� ������
	for(i = 0x0; i < ip.ChannelsQuantity; i++) ip.ControlTable[i] = (WORD)(i);
	ip.InputRate = ReadRate;					// ������� ������ ��� � ���
	ip.InterKadrDelay = 0.0;					// ����������� �������� - ���� ������ ������������� � 0.0
	ip.InputFifoBaseAddress = 0x0;  			// ������� ����� FIFO ������ ���
	ip.InputFifoLength = 0x3000;	 			// ����� FIFO ������ ���
	// ����� ������������ ��������� ������������� ������������, ������� ��������� � ���� ������
	for(i = 0x0; i < 0x8; i++) { ip.AdcOffsetCoef[i] = fi.AdcOffsetCoef[i]; ip.AdcScaleCoef[i] = fi.AdcScaleCoef[i]; }
	// ��������� ��������� ��������� ������ ��� � ������
	if(!SET_INPUT_PARS_Usb3000(pModule, &ip)) TerminateApplication(" SET_INPUT_PARS_Usb3000() --> Bad\n", TRUE);
	else printf(" SET_INPUT_PARS_Usb3000() --> OK\n");

	// ��������� �� ������ ������� ��������� ������ ������ USB3000
	printf(" \n");
	printf(" Module USB3000 (S/N %s) is ready ... \n", ModuleSerialNumber);
	printf(" Adc parameters:\n");
	printf("   InputClockSource is %s\n", ip.InputClockSource ? "EXTERNAL" : "INTERNAL");
	printf("   SynchroType is %s\n", ip.SynchroType ? "TTL_START_SYNCHRO" : "NO_SYNCHRO");
	printf("   ChannelsQuantity = %2d\n", ip.ChannelsQuantity);
	printf("   AdcRate = %8.3f kHz\n", ip.InputRate);
	printf("   InterKadrDelay = %2.4f ms\n", ip.InterKadrDelay);
	printf("   ChannelRate = %8.3f kHz\n", ip.ChannelRate);

	// ���� �������� ����� ��� :(
	hFile = INVALID_HANDLE_VALUE;
	// ������� ���� ������ ������ ����� ������
	ThreadErrorNumber = 0x0;

	// ��������� �������� ������ ��� ����� ��� �������� � ������ ������
	ReadBuffer = NULL;
	ReadBuffer = malloc(NBlockRead * DataStep * sizeof(SHORT));
	if(!ReadBuffer) TerminateApplication(" Cannot allocate memory for ReadBuffer :(((\n", TRUE);

	// ������� � ��������� ����� ����� ����� ������ �� ������
	hReadThread=CreateThread(0x0, 0x2000, ServiceReadThread, 0x0, 0x0, &ReadTid);
	if(!hReadThread) TerminateApplication("Can't start input data thread!", TRUE);

	// ���� ���������� ������ ������� ������
	printf("\n");
	while(!IsThreadComplete)
	{
		if(OldCounter != Counter) { printf(" Counter %4u from %4u\r", Counter, NBlockRead); OldCounter = Counter; }
		Sleep(20);
	}

	// ��� ��������� ������ ������ ������ ������
	WaitForSingleObject(hReadThread, INFINITE);
	// ��� ������ �������
	printf("\n\n");

	// ���� �� ���� ������ ����� ������ - ������� ���������� ������ � ����
 	if(!ThreadErrorNumber)
	{
		// ������� ���� ��� ������ ���������� ������
		hFile = CreateFile("Test.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE) TerminateApplication(" Open file 'Test.dat' --> Failed!!!\n", TRUE);
		else printf(" CreateFile(Test.dat) --> Ok\n");

		// ������ ������� � ���� ���������� ������
		FileBytesWritten = 0x0;
		if(!WriteFile(	hFile,							// handle to file to ite to
	    					ReadBuffer,						// pointer to data to ite to file
							2*NBlockRead*DataStep,		// number of bytes to ite
    						&FileBytesWritten,			// pointer to number of bytes itten
					   	NULL			  					// pointer to structure needed for overlapped I/O
					   ))  TerminateApplication(" WriteFile(Test.dat) --> Failed!!!", TRUE);
		else printf(" WriteFile(Test.dat) --> Ok\n");
	}

	// ���� ���� ������ - ������� �� ����
	if(ThreadErrorNumber) { TerminateApplication(NULL, FALSE); ShowThreadErrorMessage(); }
	else { printf("\n"); TerminateApplication("\n The program was completed successfully!!!\n", FALSE); }
}

//------------------------------------------------------------------------
// ����� � ������� �������������� ���� ������ � �� �� ������
//------------------------------------------------------------------------
DWORD WINAPI ServiceReadThread(PVOID Context)
{
	WORD RequestNumber;
	WORD i;
	// ������������� ������� �� ���� �������
	HANDLE ReadEvent[2];
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED ReadOv[2];
	DWORD BytesTransferred[2];
//	DWORD TimeOut;

	// ��������� ���� ������ � ������������ ��������� ��������������� ����� bulk USB
	if(!STOP_READ_Usb3000(pModule)) { ThreadErrorNumber = 0x6; IsThreadComplete = TRUE; return 0; }

	// �������� ��� �������
	ReadEvent[0] = CreateEvent(NULL, FALSE , FALSE, NULL);
	memset(&ReadOv[0], 0, sizeof(OVERLAPPED)); ReadOv[0].hEvent = ReadEvent[0];
	ReadEvent[1] = CreateEvent(NULL, FALSE , FALSE, NULL);
	memset(&ReadOv[1], 0, sizeof(OVERLAPPED)); ReadOv[1].hEvent = ReadEvent[1];

	// ������� ����� ������
//	TimeOut = (DWORD)(DataStep/ReadRate) + 1000;

	// ������ ��������������� ������ �� ���� ������
	RequestNumber = 0;
	if(!ReadData_Usb3000(pModule, ReadBuffer, &DataStep, &BytesTransferred[RequestNumber], &ReadOv[RequestNumber]))
				if(GetLastError() != ERROR_IO_PENDING) { CloseHandle(ReadEvent[0]); CloseHandle(ReadEvent[1]); ThreadErrorNumber = 0x2; IsThreadComplete = TRUE; return 0; }

	// ������ ��������� ���� ������
	if(START_READ_Usb3000(pModule))
	{
		// ���� ����� ������
		for(i = 0x1; i < NBlockRead; i++)
		{
			RequestNumber ^= 0x1;
			// ������� ������ �� ��������� ������ ������
			if(!ReadData_Usb3000(pModule, ReadBuffer + i*DataStep, &DataStep, &BytesTransferred[RequestNumber], &ReadOv[RequestNumber]))
					if(GetLastError() != ERROR_IO_PENDING) { ThreadErrorNumber = 0x2; break; }

			// ��� ��������� �������� ����� ��������� ������ ������
			if(!WaitingForRequestCompleted(&ReadOv[RequestNumber^0x1])) break;
//			if(WaitForSingleObject(ReadEvent[RequestNumber^0x1], TimeOut) == WAIT_TIMEOUT)
//				            		{ ThreadErrorNumber = 0x3; break; }

			if(ThreadErrorNumber) break;
			else if(kbhit()) { ThreadErrorNumber = 0x1; break; }
			else Sleep(20);
			Counter++;
		}

		// ��� ���������� �������� ����� ��������� ������ ������
		if(!ThreadErrorNumber)
		{
			RequestNumber ^= 0x1;
			WaitingForRequestCompleted(&ReadOv[RequestNumber^0x1]);
//			if(WaitForSingleObject(ReadEvent[RequestNumber^0x1], TimeOut) == WAIT_TIMEOUT) ThreadErrorNumber = 0x3;
			Counter++;
		}
	}
	else { ThreadErrorNumber = 0x5; }

	// ��������� ���� ������
	if(!STOP_READ_Usb3000(pModule)) ThreadErrorNumber = 0x6;
	// ���� ����, �� ������ ������������� ����������� ������
	if(!CancelIo(GetModuleHandle_Usb3000(pModule))) ThreadErrorNumber = 0x7;
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadEvent[i]);
	// ��������� ��������
	Sleep(100);
	// ��������� ������ ��������� ������ ����� ������
	IsThreadComplete = TRUE;
	// ������ ����� �������� �� ������ ����� ������
	return 0;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv)
{
	DWORD ReadBytesTransferred;

	while(TRUE)
	{
		if(GetOverlappedResult(ModuleHandle, ReadOv, &ReadBytesTransferred, FALSE)) break;
		else if(GetLastError() !=  ERROR_IO_INCOMPLETE) { ThreadErrorNumber = 0x3; return FALSE; }
		else if(kbhit()) { ThreadErrorNumber = 0x1; return FALSE; }
		else Sleep(20);
	}
	return TRUE;
}

//------------------------------------------------------------------------
// ��������� ��������� � �������
//------------------------------------------------------------------------
void ShowThreadErrorMessage(void)
{
	switch(ThreadErrorNumber)
	{
		case 0x0:
			break;

		case 0x1:
			// ���� ��������� ���� ������ ��������, ��������� ���� ��������
			printf("\n READ Thread: The program was terminated! :(((\n");
			break;

		case 0x2:
			printf("\n READ Thread: ReadData() --> Bad :(((\n");
			break;

		case 0x3:
			printf("\n READ Thread: Read Request --> Bad :(((\n");
//			printf("\n READ Thread: Timeout is occured :(((\n");
			break;

		case 0x4:
			printf("\n READ Thread: Buffer Data Error! :(((\n");
			break;

		case 0x5:
			printf("\n READ Thread: START_READ() --> Bad :(((\n");
			break;

		case 0x6:
			printf("\n READ Thread: STOP_READ() --> Bad! :(((\n");
			break;

		case 0x7:
			printf("\n READ Thread: Can't complete input and output (I/O) operations! :(((");
			break;

		default:
			printf("\n READ Thread: Unknown error! :(((\n");
			break;
	}

	return;
}

//------------------------------------------------------------------------
// ����� ��������� �, ���� �����, ��������� ����� �� ���������
//------------------------------------------------------------------------
void TerminateApplication(char *ErrorString, BOOL TerminationFlag)
{
	// ��������� ��������� ������
	if(pModule)
	{ 
		// ��������� ��������� ������
		if(!ReleaseInstance_Usb3000(pModule)) printf(" ReleaseInstance_Usb3000() --> Bad\n", TRUE);
		else printf(" ReleaseInstance_Usb3000() --> OK\n");
		// ������� ��������� �� ��������� ������
		pModule = NULL; 
	}

	// ��������� �� �����
	if(ReadBuffer) { free(ReadBuffer); ReadBuffer = NULL; }
	// ��������� ������������� ������ ����� ������
	if(hReadThread) { CloseHandle(hReadThread); hReadThread = NULL; }
	// ��������� ������������� ����� ������
	if(hFile != INVALID_HANDLE_VALUE) { CloseHandle(hFile); hFile = INVALID_HANDLE_VALUE; }

	// ������� ����� ���������
	if(ErrorString) printf(ErrorString);

	// ���� ����� - �������� ��������� ���������
	if(TerminationFlag) exit(1);
	else return;
}
