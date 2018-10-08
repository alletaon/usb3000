#ifndef __WRRTUSBAPI__
#define __WRRTUSBAPI__
#include "Rtusbapi.h"

// *****************************************************************************
// wrRtusbapi.dll - "�������" �������� Rtusbapi.dll, ��� ������ � ������� ��
// ��������������� ��������� �/��� ������ (�, MSVB, NI LabView&LabWindows � �.�.)
// *****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif
	// ����� ������� ��� ������ � �������� �� R-Technology
	DWORD WINAPI 	GetDllVersion_Rtusbapi(void);
	LPVOID WINAPI 	CreateInstance_Rtusbapi(PCHAR const DeviceName);
#ifdef __cplusplus
 }
#endif



// =============================================================================
//                            ������ USB2185
// =============================================================================
// ***************************** ���� ��� **************************************


// =============================================================================
//                            ������ USB3000
// =============================================================================

// --------------------------- ��������� ---------------------------------------
#pragma pack(4)
// ���������, �������� ������ ����� ������ ��� ������ USB-3000
typedef struct _INPUT_PARS_USB3000_
{
	WORD size;
	BOOL InputEnabled;			  		// ������ ����������/���������� ����� ������ (������ ��� ������)
	BOOL CorrectionEnabled;				// ���������� �������������� ������� ������ (� ���)
	WORD InputClockSource;				// �������� �������� ��������� ��� ����� ������
	WORD InputType;						// ��� �������� � ������ ����� (��� ��� ���)
	WORD SynchroType;						// ��� ������������� �������� � ������ �����
	WORD SynchroAdType;					// ��� ���������� �������������
	WORD SynchroAdMode; 					// ����� ���������� ������������
	WORD SynchroAdChannel;  			// ����������� ��� ��� ���������� �������������
	SHORT SynchroAdPorog; 				// ����� ������������ ��� ��� ���������� �������������
	WORD ChannelsQuantity;				// ����� �������� ���������� �������
	WORD ControlTable[128];				// ����������� ������� � ��������� ����������� ��������
	WORD InputFifoBaseAddress;			// ������� ����� FIFO ������ ����� � DSP ������
	WORD InputFifoLength;	  			// ����� FIFO ������ ����� � DSP ������
	double InputRate;	  			  		// �������� ������� ����� ������ � ���
	double InterKadrDelay;		  		// ����������� �������� � ��
	double ChannelRate;					// ������� ����� �������������� ����������� ������
	double AdcOffsetCoef[8]; 			// ���������������� ����. �������� ���� ��� ���
	double AdcScaleCoef[8];				// ���������������� ����. �������� ��� ���
} INPUT_PARS_USB3000;

// ���������, �������� ������ ������ ������ ��� ������ USB-3000
typedef struct _OUTPUT_PARS_USB3000_
{
	WORD size;
	BOOL OutputEnabled;					// ����������/���������� ������ ������
	double OutputRate;	  		  		// ������� ������ ������ � ���
	WORD OutputFifoBaseAddress;  		// ������� ����� FIFO ������ ������
	WORD OutputFifoLength;				// ����� FIFO ������ ������
} OUTPUT_PARS_USB3000;

// ��������� ����������������� ����
typedef struct _FLASH_USB3000_
{
	WORD CRC16;								// ����������� �����
	WORD size;								// ������ ������ ��������� � ������
	BYTE SerialNumber[9];				// �������� ����� ������
	BYTE Name[11];							// �������� ������
	BYTE Revision;							// ������� ������
	BYTE DspType[17];						// ��� �������������� DSP
	BYTE IsDacPresented; 				// ������ ������� ���
	DWORD DspClockout; 					// �������� ������� DSP � ��
	float AdcOffsetCoef[8];				// ���������������� ����. �������� ���� ��� ���
	float AdcScaleCoef[8];				// ���������������� ����. �������� ��� ���
	float DacOffsetCoef[2];				// ���������������� ����. �������� ���� ��� ���
	float DacScaleCoef[2];				// ���������������� ����. �������� ��� ���
	BYTE ReservedByte[129];				// ���������������
} FLASH_USB3000;

// ���������, ���������� ���������� � ������ �������� DSP
typedef struct _DSP_INFO_USB3000_
{
	BYTE Target[10];						//	������, ��� �������� ������������ ������ ������� DSP
	BYTE Label[6];							// ����� ������������ �������� DSP
	BYTE DspMajor;							// ������� ����� ������ �������� DSP
	BYTE DspMinor;							// ������� ����� ������ �������� DSP
} DSP_INFO_USB3000;

#pragma pack()


// --------------------------- ��������� ---------------------------------------
// ����� ������ �������� ����� ������ � ������ �������� DSP
const WORD VarsBaseAddress_USB3000 = 0x30;
// �������� ������� ������ DSP � ���
const DWORD DSP_CLOCK_OUT_USB3000 = 72000;

// ��������� ��������� �������� ��������� ��� ����� ������
enum {	INTERNAL_INPUT_CLOCK_USB3000, EXTERNAL_INPUT_CLOCK_USB3000, INVALID_INPUT_CLOCK_USB3000 };
// ��������� ���� ������������� �������� � ������ �����
enum {	NO_SYNCHRO_USB3000, TTL_START_SYNCHRO_USB3000, TTL_KADR_SYNCHRO_USB3000, ANALOG_SYNCHRO_USB3000, INVALID_INPUT_SYNCHRO_USB3000 };
// ��������� ���� �������� � ������ �����
enum {	EMPTY_DATA_USB3000, ADC_DATA_USB3000, TTL_DATA_USB3000, MIXED_DATA_USB3000, INVALID_INPUT_DATA_USB3000 };
// ��������� ������� �������� ������ ���� USB
enum {	USB11_USB3000, USB20_USB3000, INVALID_USB_SPEED_USB3000 };

// ���������� �������� �������� DSP (������������� � ������ �������� DSP)
#define D_PROGRAM_BASE_ADDRESS_USB3000			(VarsBaseAddress_USB3000 + 0x0);
#define D_TARGET_USB3000							(VarsBaseAddress_USB3000 + 0x1);
#define D_LABEL_USB3000								(VarsBaseAddress_USB3000 + 0x6);
#define D_VERSION_USB3000							(VarsBaseAddress_USB3000 + 0x9);
#define D_TEST_VAR1_USB3000						(VarsBaseAddress_USB3000 + 0xA);
#define D_TEST_VAR2_USB3000						(VarsBaseAddress_USB3000 + 0xB);
#define D_TEST_INTR_VAR_USB3000					(VarsBaseAddress_USB3000 + 0xC);
#define D_MODULE_READY_USB3000					(VarsBaseAddress_USB3000 + 0xD);
#define D_COMMAND_USB3000							(VarsBaseAddress_USB3000 + 0xE);
#define D_INPUT_CLOCK_SOURCE						(VarsBaseAddress + 0x10);

#define D_CONTROL_TABLE_LENGHT_USB3000			(VarsBaseAddress_USB3000 + 0x20);
#define D_INPUT_SAMPLE_USB3000					(VarsBaseAddress_USB3000 + 0x21);
#define D_INPUT_CHANNEL_USB3000					(VarsBaseAddress_USB3000 + 0x22);
#define D_INPUT_RATE_USB3000						(VarsBaseAddress_USB3000 + 0x23);
#define D_INTER_KADR_DELAY_USB3000				(VarsBaseAddress_USB3000 + 0x24);
#define D_FIRST_SAMPLE_DELAY_USB3000			(VarsBaseAddress_USB3000 + 0x25);
#define D_INPUT_ENABLED_USB3000					(VarsBaseAddress_USB3000 + 0x26);
#define D_INPUT_FIFO_BASE_ADDRESS_USB3000		(VarsBaseAddress_USB3000 + 0x27);
#define D_INPUT_FIFO_LENGTH_USB3000				(VarsBaseAddress_USB3000 + 0x28);
#define D_CUR_INPUT_FIFO_LENGTH_USB3000 		(VarsBaseAddress_USB3000 + 0x29);

#define D_CORRECTION_ENABLED_USB3000			(VarsBaseAddress_USB3000 + 0x2B);

#define D_INPUT_TYPE_USB3000						(VarsBaseAddress_USB3000 + 0x2C);
#define D_SYNCHRO_TYPE_USB3000					(VarsBaseAddress_USB3000 + 0x2D);
#define D_SYNCHRO_AD_TYPE_USB3000				(VarsBaseAddress_USB3000 + 0x2E);
#define D_SYNCHRO_AD_MODE_USB3000				(VarsBaseAddress_USB3000 + 0x2F);
#define D_SYNCHRO_AD_CHANNEL_USB3000			(VarsBaseAddress_USB3000 + 0x30);
#define D_SYNCHRO_AD_POROG_USB3000				(VarsBaseAddress_USB3000 + 0x31);

#define D_OUTPUT_SAMPLE_USB3000					(VarsBaseAddress_USB3000 + 0x40);
#define D_OUTPUT_SCLK_DIV_USB3000				(VarsBaseAddress_USB3000 + 0x41);
#define D_OUTPUT_RATE_USB3000						(VarsBaseAddress_USB3000 + 0x42);
#define D_OUTPUT_ENABLED_USB3000					(VarsBaseAddress_USB3000 + 0x43);
#define D_OUTPUT_FIFO_BASE_ADDRESS_USB3000	(VarsBaseAddress_USB3000 + 0x44);
#define D_OUTPUT_FIFO_LENGTH_USB3000			(VarsBaseAddress_USB3000 + 0x45);
#define D_CUR_OUTPUT_FIFO_LENGTH_USB3000		(VarsBaseAddress_USB3000 + 0x46);

#define D_ENABLE_TTL_OUT_USB3000					(VarsBaseAddress_USB3000 + 0x4D);
#define D_TTL_OUT_USB3000							(VarsBaseAddress_USB3000 + 0x4E);
#define D_TTL_IN_USB3000							(VarsBaseAddress_USB3000 + 0x4F);

#define D_ADC_SCALE_USB3000						(VarsBaseAddress_USB3000 + 0x50);
#define D_ADC_ZERO_USB3000							(VarsBaseAddress_USB3000 + 0x58);
#define D_CONTROL_TABLE_USB3000					(0x100);

// ---------------------------- ������� ----------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
	// ������� ��� ������ � ������� USB3000
	BOOL WINAPI 	OpenDevice_Usb3000(LPVOID IDevPtr, WORD VirtualSlot);
	BOOL WINAPI 	CloseDevice_Usb3000(LPVOID IDevPtr);
	HANDLE WINAPI 	GetModuleHandle_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	GetUsbSpeed_Usb3000(LPVOID IDevPtr, BYTE * const UsbSpeed);
	BOOL WINAPI 	GetModuleName_Usb3000(LPVOID IDevPtr, PCHAR const ModuleName);
	BOOL WINAPI 	GetModuleSerialNumber_Usb3000(LPVOID IDevPtr, PCHAR const SerialNumber);
	BOOL WINAPI 	GetAvrVersion_Usb3000(LPVOID IDevPtr, PCHAR const AvrVersion);
	BOOL WINAPI 	ReleaseInstance_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	RESET_DSP_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	LOAD_DSP_Usb3000(LPVOID IDevPtr, PCHAR const FileName);
	BOOL WINAPI 	MODULE_TEST_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	GET_DSP_INFO_Usb3000(LPVOID IDevPtr, DSP_INFO_USB3000* const DspInfo);
	BOOL WINAPI 	SEND_COMMAND_Usb3000(LPVOID IDevPtr, WORD Command);
	int WINAPI 		GetLastErrorString_Usb3000(LPVOID IDevPtr, LPTSTR const lpBuffer, DWORD nSize);
	BOOL WINAPI 	GET_INPUT_PARS_Usb3000(LPVOID IDevPtr, INPUT_PARS_USB3000* const ap);
	BOOL WINAPI 	SET_INPUT_PARS_Usb3000(LPVOID IDevPtr, INPUT_PARS_USB3000* const ap);
	BOOL WINAPI 	START_READ_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	STOP_READ_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	READ_KADR_Usb3000(LPVOID IDevPtr, SHORT * const Data);
	BOOL WINAPI 	READ_SAMPLE_Usb3000(LPVOID IDevPtr, WORD Channel, SHORT * const Sample);
	BOOL WINAPI 	ReadData_Usb3000(LPVOID IDevPtr, SHORT * const lpBuffer, DWORD * const nNumberOfWordsToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
	BOOL WINAPI 	GET_OUTPUT_PARS_Usb3000(LPVOID IDevPtr, OUTPUT_PARS_USB3000* const dp);
	BOOL WINAPI 	SET_OUTPUT_PARS_Usb3000(LPVOID IDevPtr, OUTPUT_PARS_USB3000* const dp);
	BOOL WINAPI 	START_WRITE_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	STOP_WRITE_Usb3000(LPVOID IDevPtr);
	BOOL WINAPI 	WriteData_Usb3000(LPVOID IDevPtr, SHORT * const lpBuffer, DWORD * const nNumberOfWordsToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
	BOOL WINAPI 	WRITE_SAMPLE_Usb3000(LPVOID IDevPtr, WORD Channel, SHORT * const Sample);
	BOOL WINAPI 	ENABLE_TTL_OUT_Usb3000(LPVOID IDevPtr, BOOL EnableTtlOut);
	BOOL WINAPI 	TTL_IN_Usb3000(LPVOID IDevPtr, WORD * const TtlIn);
	BOOL WINAPI 	TTL_OUT_Usb3000(LPVOID IDevPtr, WORD TtlOut);
	BOOL WINAPI 	ENABLE_FLASH_WRITE_Usb3000(LPVOID IDevPtr, BOOL EnableFlashWrite);
	BOOL WINAPI 	PUT_FLASH_Usb3000(LPVOID IDevPtr, FLASH_USB3000 * const fi);
	BOOL WINAPI 	GET_FLASH_Usb3000(LPVOID IDevPtr, FLASH_USB3000 * const fi);
	BOOL WINAPI 	PUT_VAR_WORD_Usb3000(LPVOID IDevPtr, WORD Address, SHORT Data);
	BOOL WINAPI 	GET_VAR_WORD_Usb3000(LPVOID IDevPtr, WORD Address, SHORT * const Data);
	BOOL WINAPI 	PUT_DM_WORD_Usb3000(LPVOID IDevPtr, WORD Address, SHORT Data);
	BOOL WINAPI 	GET_DM_WORD_Usb3000(LPVOID IDevPtr, WORD Address, SHORT * const Data);
	BOOL WINAPI 	PUT_PM_WORD_Usb3000(LPVOID IDevPtr, WORD Address, LONG Data);
	BOOL WINAPI 	GET_PM_WORD_Usb3000(LPVOID IDevPtr, WORD Address, LONG * const Data);
	BOOL WINAPI 	PUT_DM_ARRAY_Usb3000(LPVOID IDevPtr, WORD BaseAddress, WORD NPoints, SHORT * const Data);
	BOOL WINAPI 	GET_DM_ARRAY_Usb3000(LPVOID IDevPtr, WORD BaseAddress, WORD NPoints, SHORT * const Data);
	BOOL WINAPI 	PUT_PM_ARRAY_Usb3000(LPVOID IDevPtr, WORD BaseAddress, WORD NPoints, LONG * const Data);
	BOOL WINAPI 	GET_PM_ARRAY_Usb3000(LPVOID IDevPtr, WORD BaseAddress, WORD NPoints, LONG * const Data);

#ifdef __cplusplus
 }
#endif

#endif
