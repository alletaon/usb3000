// ���������� ��������� ��� ����������� ������ ������ ��� ������ USB3000
//******************************************************************************
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "Rtusbapi.h"

// ��������� ���������� ���������
void TerminateApplication(char *ErrorString, bool TerminationFlag = true);
// ����������� ������ ���������� ���������
void ShowThreadErrorMessage(void);
// ����������
WORD Round(double Data);
// ��������� �����
DWORD PutLine(SHORT x1, SHORT y1, SHORT x2, SHORT y2, DWORD n);

// ������� ������ ������ ������ �� �� � ������
DWORD WINAPI ServiceWriteThread(PVOID /*Context*/);
// ������������� ������ ������
HANDLE hWriteThread;
DWORD WriteTid;

// ����������� ��������� ���-�� ������������ ����������� ������
const WORD MaxVirtualSoltsQuantity = 127;
// ���������� ��������� pi
const double M_PI = 3.14159265358979323846;

// ������� ������ ���������� Rtusbapi.dll
DWORD DllVersion;
// ��������� �� ��������� ������
IRTUSB3000 *pModule;
// �������� ������
char ModuleName[10];
// �������� ������ ���� USB
BYTE UsbSpeed;
// �������� ����� ������
char ModuleSerialNumber[9];
// ������ �������� AVR
char AvrVersion[5];
// ��������� ���������� � ���� ������
RTUSB3000::FLASH fi;
// ���������, ���������� ���������� � ������ �������� DSP
RTUSB3000::DSP_INFO di;
// ���������, ���������� ��������� ������ ���
RTUSB3000::OUTPUT_PARS dp;

//max ��������� ���-�� ������������ �������� (������� 32) ��� �. ReadData � WriteData()
DWORD DataStep = 8 * 1024;
// ����� ������ ��� ��������� ������
DWORD WritePoints = 2 * DataStep;
// ��������� �� ����� ��� ��������� ������
SHORT	*WriteBuffer;

// ����������� ������ ��� ���
SHORT DacSample;
// ����� ������ ���
const WORD DacNumber = 0x1;
// �������  ������ ������
const double WriteRate = 100.0;

// ��������� ���������� �������
double CurrentTime = 0.0; 			  				// � ���
const double SignalFrequency = 0.1; 	  		// � ���
const double SignalAmplitude = 1000.0;			// � ����� ���

												// �������� �������-���������
DWORD Counter = 0x0, OldCounter = 0xFFFFFFFF;

// ����� ������ ��� ���������� ������ ����� ������
WORD ThreadErrorNumber;
// ������ ���������� ������ ������ ������
bool IsThreadComplete = false;

//------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------
void main(void)
{
	WORD i;

	// �������� ����� ��������	
	system("cls");

	printf(" *******************************************\n");
	printf(" Console example of Data Writing to USB3000 \n");
	printf(" *******************************************\n\n");

	// �������� ������ ������������ ���������� Rtusbapi.dll
	if ((DllVersion = RtGetDllVersion()) != CURRENT_VERSION_RTUSBAPI)
	{
		char String[128];
		sprintf(String, " Rtusbapi.dll Version Error!!!\n   Current: %1u.%1u. Required: %1u.%1u",
			DllVersion >> 0x10, DllVersion & 0xFFFF,
			CURRENT_VERSION_RTUSBAPI >> 0x10, CURRENT_VERSION_RTUSBAPI & 0xFFFF);

		TerminateApplication(String);
	}
	else printf(" Rtusbapi.dll Version --> OK\n");

	// ������� ��������� �� ��������� ������ USB3000
	pModule = static_cast<IRTUSB3000 *>(RtCreateInstance("usb3000"));
	if (pModule == NULL)  TerminateApplication(" Module Interface --> Bad\n");
	else printf(" Module Interface --> OK\n");

	// ��������� ���������� ������ USB3000 � ������ 127 ����������� ������
	for (i = 0x0; i < MaxVirtualSoltsQuantity; i++) if (pModule->OpenDevice(i)) break;
	// ���-������ ����������?
	if (i == MaxVirtualSoltsQuantity) TerminateApplication(" Can't find module USB3000 in first 127 virtual slots!\n");
	else printf(" OpenDevice(%u) --> OK\n", i);

	// ��������� �������� ������������� ������
	if (!pModule->GetModuleName(ModuleName)) TerminateApplication(" GetModuleName() --> Bad\n");
	else printf(" GetModuleName() --> OK\n");
	// ��������, ��� ��� 'USB3000'
	if (strcmp(ModuleName, "USB3000")) TerminateApplication(" The module is not 'USB3000'\n");
	else printf(" The module is 'USB3000'\n");

	// ������ ������� �������� ������ ���� USB20
	if (!pModule->GetUsbSpeed(&UsbSpeed)) { printf(" GetUsbSpeed() --> Bad\n"); exit(1); }
	else printf(" GetUsbSpeed() --> OK\n");
	// ������ ��������� ������ �������� AVR
	printf(" USB Speed is %s\n", UsbSpeed ? "HIGH (480 Mbit/s)" : "FULL (12 Mbit/s)");

	// ��������� �������� ����� ������
	if (!pModule->GetModuleSerialNumber(ModuleSerialNumber)) TerminateApplication(" GetModuleSerialNumber() --> Bad\n");
	else printf(" GetModuleSerialNumber() --> OK\n");
	// ������ ��������� �������� ����� ������
	printf(" Module Serial Number is %s\n", ModuleSerialNumber);

	// ��������� ������ �������� AVR
	if (!pModule->GetAvrVersion(AvrVersion)) TerminateApplication(" GetAvrVersion() --> Bad\n");
	else printf(" GetAvrVersion() --> OK\n");
	// ������ ��������� ������ �������� AVR
	printf(" Avr Driver Version is %s\n", AvrVersion);

	// ��������� ����
	fi.size = sizeof(RTUSB3000::FLASH);
	if (!pModule->GET_FLASH(&fi)) TerminateApplication(" GET_FLASH() --> Bad\n");
	else printf(" GET_FLASH() --> OK\n");

	// ��� �������� DSP ������ �� ���������������� ������� ������� DLL ����������
	if (!pModule->LOAD_DSP()) TerminateApplication(" LOAD_DSP() --> Bad\n");
	else printf(" LOAD_DSP() --> OK\n");

	// �������� �������� ������
	if (!pModule->MODULE_TEST()) TerminateApplication(" MODULE_TEST() --> Bad\n");
	else printf(" MODULE_TEST() --> OK\n");

	// ������� ���������� �� ����������� �������� DSP
	if (!pModule->GET_DSP_INFO(&di)) TerminateApplication(" GET_DSP_INFO() --> Bad\n");
	else printf(" GET_DSP_INFO() --> OK\n");
	// ������ ��������� ������ ������������ �������� DSP
	printf(" DSP Driver version is %1u.%1u\n", di.DspMajor, di.DspMinor);

	// ������� ������� ������ �� ������ ����� ���
	DacSample = 0x0;
	// ����������� ������ ��� ������� ������
	DacSample = Round((DacSample + fi.DacOffsetCoef[0])*fi.DacScaleCoef[0]);
	if (!pModule->WRITE_SAMPLE(0x0, &DacSample)) TerminateApplication(" WRITE_SAMPLE(0) --> Bad\n");
	else printf(" WRITE_SAMPLE(0) --> OK\n");

	// ����� ������� ������ ������� �� ������ ����� ���
	DacSample = 0x0;
	// ����������� ������ ��� ������� ������
	DacSample = Round((DacSample + fi.DacOffsetCoef[1])*fi.DacScaleCoef[1]);
	if (!pModule->WRITE_SAMPLE(0x1, &DacSample)) TerminateApplication(" WRITE_SAMPLE(1) --> Bad\n");
	else printf(" WRITE_SAMPLE(1) --> OK\n");

	// ��������� ������� ��������� ������ ������
	dp.size = sizeof(RTUSB3000::OUTPUT_PARS);
	if (!pModule->GET_OUTPUT_PARS(&dp)) TerminateApplication(" GET_OUTPUT_PARS() --> Bad\n");
	else printf(" GET_OUTPUT_PARS() --> OK\n");

	// ��������� �������� ��������� ������ ������ �� ������ USB3000
	dp.OutputRate = WriteRate;		  			// ������� ������ ������ � ���
	dp.OutputFifoBaseAddress = 0x3000;   	// ������� ����� FIFO ������ ������
	dp.OutputFifoLength = 0xF80;				// ����� FIFO ������ ������

												// ��������� ��������� ��������� ������ ������
	if (!pModule->SET_OUTPUT_PARS(&dp)) TerminateApplication(" SET_OUTPUT_PARS() --> Bad\n");
	else printf(" SET_OUTPUT_PARS() --> OK\n");

	// ��������� �� ������ �������� ��������� ������ ������ �� ������ ������ 
	printf(" \n");
	printf(" Module USB3000 (S/N %s) is ready ... \n", ModuleSerialNumber);
	printf("  Ouput parameters:\n");
	printf("    WriteRate = %6.1f kHz\n", dp.OutputRate);

	// ������� ���� ������ ������ ����� ������
	ThreadErrorNumber = 0x0;

	// ������� � ��������� ����� ������ ������ �� �� � ������
	hWriteThread = CreateThread(0, 0x2000, ServiceWriteThread, 0x0, 0x0, &WriteTid);
	if (!hWriteThread) TerminateApplication("Cann't start output data thread!");

	// ���� ���������� ������ ������� ������
	printf(" (you can press any key to terminate the program)\n\n");
	while (!IsThreadComplete)
	{
		if (OldCounter != Counter) { printf(" Counter %3u\r", Counter); OldCounter = Counter; }
		else Sleep(0);
	}

	// ��� ��������� ������ ������ ������ ������
	WaitForSingleObject(hWriteThread, INFINITE);
	// ��� ������ �������
	printf("\n\n");

	// ���� ���� ������ - ������� �� ����
	if (ThreadErrorNumber) { TerminateApplication(NULL, false); ShowThreadErrorMessage(); }
	else { printf("\n"); TerminateApplication("\n The program was completed successfully!!!\n", false); }
}

//------------------------------------------------------------------------
// ����� � ������� �������������� ����� ������ �� �� � ������
//------------------------------------------------------------------------
DWORD WINAPI ServiceWriteThread(PVOID /*Context*/)
{
	WORD RequestNumber;
	DWORD i, j, k;
	DWORD BaseIndex;
	// ������������� ������� �� ���� �������
	HANDLE WriteEvent[2];
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED WriteOv[2];
	DWORD BytesTransferred[2];
	DWORD TimeOut;

	SHORT l = 2048;
	DataStep = l * 2 * 3;
	// ��������� �������� ������ ��� ����� ��� ��������� � ������ ������
	WriteBuffer = new SHORT[2 * DataStep];
	if (!WriteBuffer) TerminateApplication(" Cannot allocate memory for data buffer \n");

	
	DWORD SizeLine1 = PutLine(0, 0, l/4, l, 0);
	DWORD SizeLine2 = PutLine(l/2, l, l, 0, SizeLine1);
	DWORD SizeLine3 = PutLine(l, 0, 0, 0, SizeLine1 + SizeLine2);
	//DWORD SizeLine4 = PutLine(l, 0, 0, 0, SizeLine1 + SizeLine2 + SizeLine3);
	PutLine(0, 0, l / 2, l, DataStep);
	PutLine(l / 2, l, l, 0, DataStep + SizeLine1);
	PutLine(l, 0, 0, 0, DataStep + SizeLine1 + SizeLine2);
	//PutLine(l, 0, 0, 0, DataStep + SizeLine1 + SizeLine2 + SizeLine3);
	
	

	// ��������� ������� FIFO ����� ������ � DSP ������
	if (!pModule->PUT_DM_ARRAY(dp.OutputFifoBaseAddress, dp.OutputFifoLength, (SHORT *)WriteBuffer))
	{
		ThreadErrorNumber = 0x1; IsThreadComplete = true; return 1;
	}

	// ������ ��������� ��������� ������ ��� ����� ������ WriteBuffer (�������� �������������)
	

	// ��������� ����� ������ � ������������ ��������� ��������������� ����� bulk USB (PIPE_RESET)
	if (!pModule->STOP_WRITE()) { ThreadErrorNumber = 0x6; IsThreadComplete = true; return 0; }

	// �������� ��� �������
	WriteEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	memset(&WriteOv[0], 0, sizeof(OVERLAPPED)); WriteOv[0].hEvent = WriteEvent[0];
	WriteEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	memset(&WriteOv[1], 0, sizeof(OVERLAPPED)); WriteOv[1].hEvent = WriteEvent[1];

	// ������� ������ ������
	TimeOut = (DWORD)(DataStep / dp.OutputRate + 1000);

	// ������ ��������������� ������ �� ����� ������
	RequestNumber = 0x0;
	if (!pModule->WriteData(WriteBuffer, &DataStep, &BytesTransferred[RequestNumber], &WriteOv[RequestNumber]))
		if (GetLastError() != ERROR_IO_PENDING) { CloseHandle(WriteEvent[0]); CloseHandle(WriteEvent[1]); ThreadErrorNumber = 0x2; IsThreadComplete = true; return 0; }

	// ������ ��������� ���������� ��� ����� ������
	if (pModule->START_WRITE())
	{
		// ���� ������������� ������ ������
		for (;;)
		{
			RequestNumber ^= 0x1;
			// ������� ������ �� ����� ��������� ������ ������ � DSP ������
			if (!pModule->WriteData(WriteBuffer + RequestNumber*DataStep, &DataStep, &BytesTransferred[RequestNumber], &WriteOv[RequestNumber]))
				if (GetLastError() != ERROR_IO_PENDING) { ThreadErrorNumber = 0x2; break; }

			// ��� ��������� �������� ������ ��������� ������ ������
			if (WaitForSingleObject(WriteEvent[RequestNumber ^ 0x1], TimeOut) == WAIT_TIMEOUT)
			{
				ThreadErrorNumber = 0x3; break;
			}

			// ���������� ��������� ������ ��������� ������ (�������� �������������)
			BaseIndex = (RequestNumber ^ 0x1)*DataStep;
				
			if (ThreadErrorNumber) break;
			else if (kbhit()) { ThreadErrorNumber = 0x1; break; }
			else Sleep(0);
			Counter++;
		}
	}
	else { ThreadErrorNumber = 0x5; }

	// ��������� ����� ������
	if (!pModule->STOP_WRITE()) ThreadErrorNumber = 0x6;
	// ����� �� �����
	if (!CancelIo(pModule->GetModuleHandle())) ThreadErrorNumber = 0x7;
	// ��������� �������������� �������
	CloseHandle(WriteEvent[0]); CloseHandle(WriteEvent[1]);

	// ��������� ������ ��������� ������ ������ ������
	IsThreadComplete = true;

	return 0;							// ������ �� ������
}

DWORD PutLine(SHORT x1, SHORT y1, SHORT x2, SHORT y2, DWORD n)
{
	const SHORT deltaX = abs(x2 - x1);
	const SHORT deltaY = abs(y2 - y1);
	const SHORT signX = x1 < x2 ? 1 : -1;
	const SHORT signY = y1 < y2 ? 1 : -1;
	DWORD i = 0;
	int error = deltaX - deltaY;
	while (x1 != x2 || y1 != y2) {
		const int error2 = error * 2;
		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
			WriteBuffer[n + i] = x1;
			WriteBuffer[n + i] &= (WORD)(0xFFF);
			WriteBuffer[n + i] |= (WORD)(0x1 << 15) | (WORD)(0x1 << 14);
			i++;
		}
		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
			WriteBuffer[n + i] = y1;
			WriteBuffer[n + i] &= (WORD)(0xFFF);
			WriteBuffer[n + i] |= (WORD)(0x0 << 15) | (WORD)(0x1 << 14);
			i++;
		}
		
	}
	return  i;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
WORD Round(double Data)
{
	if (Data > 0.0) Data += 0.5;
	else if (Data < 0.0) Data = -0.5;
	return (WORD)Data;
}


//------------------------------------------------------------------------
// ��������� ��������� � �������
//------------------------------------------------------------------------
void ShowThreadErrorMessage(void)
{
	switch (ThreadErrorNumber)
	{
	case 0x0:
		break;

	case 0x1:
		// ���� ��������� ���� ������ ��������, ��������� ���� ��������
		printf("\n WRITE Thread: The program was terminated! :(((\n");
		break;

	case 0x2:
		printf("\n WRITE Thread: WriteData() --> Bad :(((\n");
		break;

	case 0x3:
		printf("\n WRITE Thread: Timeout is occured :(((\n");
		break;

	case 0x4:
		printf("\n WRITE Thread: Buffer Data Error! :(((\n");
		break;

	case 0x5:
		printf("\n WRITE Thread: START_WRITE() --> Bad :(((\n");
		break;

	case 0x6:
		printf("\n WRITE Thread: STOP_WRITE() --> Bad! :(((\n");
		break;

	case 0x7:
		printf("\n READ Thread: Can't complete input and output (I/O) operations! :(((");
		break;

	default:
		printf("\n WRITE Thread: Unknown error! :(((\n");
		break;
	}

	return;
}

//------------------------------------------------------------------------
// ����� ��������� �, ���� �����, ��������� ����� �� ���������
//------------------------------------------------------------------------
void TerminateApplication(char *ErrorString, bool TerminationFlag)
{
	// ��������� ��������� ������
	if (pModule)
	{
		// ��������� ��������� ������
		if (!pModule->ReleaseInstance()) printf(" ReleaseInstance() --> Bad\n");
		else printf(" ReleaseInstance() --> OK\n");
		// ������� ��������� �� ��������� ������
		pModule = NULL;
	}

	// ����������� ������� �������
	if (WriteBuffer) { delete[] WriteBuffer; WriteBuffer = NULL; }
	// ��������� ������������� ������ ������ ������
	if (hWriteThread) { CloseHandle(hWriteThread); hWriteThread = NULL; }

	// ������� ����� ���������
	if (ErrorString) printf(ErrorString);

	// ���� ����� - �������� ��������� ���������
	if (TerminationFlag) exit(1);
	else return;
}
