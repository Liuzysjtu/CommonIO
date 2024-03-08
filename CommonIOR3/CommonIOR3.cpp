#include <Windows.h>
#include <iostream>

#define DEVICE_LINK_NAME "\\\\.\\DemoDevice"
// 0X800 ~ 0XFFF
#define READWRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
void main() {
	BOOL bRet;
	HANDLE hDevice = NULL;

	hDevice = CreateFileA(DEVICE_LINK_NAME, 
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Create file failed\n");
	}

	// 3种通讯方式
	// 1. 通过Buffered I/O
	// 2. 通过Direct I/O
	// 3. 通过Neither I/O
	char outBuffer[MAXBYTE] = { 0 };
	char inBuffer[MAXBYTE] = "Hello world r3";

	//bRet = ReadFile(hDevice, outBuffer, sizeof(outBuffer), NULL, NULL);
	//if (!bRet) {
	//	printf("Read file failed\n");
	//}
	//else {
	//	printf("Read file success\n");
	//	printf("Read data: %s\n", outBuffer);
	//}

	//bRet = WriteFile(hDevice, inBuffer, sizeof(inBuffer), NULL, NULL); 

	bRet = DeviceIoControl(hDevice, 
				READWRITE, 
				inBuffer, 
				sizeof(inBuffer), 
				outBuffer, 
				sizeof(outBuffer), 
				NULL, 
				NULL);
	if (!bRet) {
		printf("DeviceIoControl failed\n");
	}
	else {
		printf("Read data: %s\n", outBuffer);
	}

	system("pause");

	CloseHandle(hDevice);

	system("pause");
}