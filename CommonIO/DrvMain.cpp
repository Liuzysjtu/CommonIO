#include<ntifs.h>
#include<ntddk.h>
// 0X800 ~ 0XFFF
#define READWRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)

VOID UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	DbgPrint("Driver Unloaded\n");
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	// 返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("Dispatch Create\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	// 返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("Dispatch Close\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchRead(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	char buffer[255] = "Hello world r0";
	
#if 0
	PVOID sysBuffer = pIrp->AssociatedIrp.SystemBuffer;

	if (!MmIsAddressValid(sysBuffer)) {
		DbgPrint("sysBuffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
#endif // 0

#if 0
	PVOID mdlBuffer = MmGetMdlVirtualAddress(pIrp->MdlAddress); // 获取MDL缓冲区地址

	if (!MmIsAddressValid(mdlBuffer)) {
		DbgPrint("mdlBuffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
#endif // 0

	PVOID userBuffer = pIrp->UserBuffer;

	if(!MmIsAddressValid(userBuffer)) {
		DbgPrint("Buffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}

	RtlCopyMemory(userBuffer, buffer, sizeof(buffer));

	pIrp->IoStatus.Information = sizeof(buffer);
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("Dispatch Close\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchWrite(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

#if 0
	PVOID sysBuffer = pIrp->AssociatedIrp.SystemBuffer;

	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	int length = stack->Parameters.Read.Length;

	if (!MmIsAddressValid(sysBuffer) && length > 0) {
		DbgPrint("sysBuffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
#endif // 0

#if 0
	PVOID mdlBuffer = MmGetMdlVirtualAddress(pIrp->MdlAddress); // 获取MDL缓冲区地址
	ULONG length = MmGetMdlByteCount(pIrp->MdlAddress); // 获取MDL缓冲区大小

	if (!MmIsAddressValid(mdlBuffer) && length > 0) {
		DbgPrint("mdlBuffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
#endif // 0

	PVOID userBuffer = pIrp->UserBuffer;
	
	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	int length = stack->Parameters.Read.Length;

	if (!MmIsAddressValid(userBuffer) && length > 0) {
		DbgPrint("User Buffer is null\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	
	DbgPrint("User buffer address: %p\n", userBuffer);
	DbgPrint("Write data: %s\n", userBuffer);

	pIrp->IoStatus.Information = length;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("Dispatch Close\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	char buffer[255] = "Hello world r0";

	PVOID sysBuffer = pIrp->AssociatedIrp.SystemBuffer;

	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	//int length = stack->Parameters.DeviceIoControl.InputBufferLength;

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case READWRITE:
		if (!MmIsAddressValid(sysBuffer)) {
			DbgPrint("sysBuffer is null\n");
			pIrp->IoStatus.Information = 0;
			pIrp->IoStatus.Status = STATUS_INVALID_USER_BUFFER;
			IoCompleteRequest(pIrp, IO_NO_INCREMENT);

			return STATUS_SUCCESS;
		}

		// 读取Ring3传递的数据
		DbgPrint("sysBuffer: %s\n", sysBuffer);

		// 向Ring3返回数据
		RtlCopyMemory(sysBuffer, buffer, sizeof(buffer));

		break;
	default:
		break;
	}


	pIrp->IoStatus.Information = sizeof(buffer);
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("Dispatch Close\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	pDriverObject->DriverUnload = UnloadDriver;

	NTSTATUS status;

	PDEVICE_OBJECT pDeviceObject;

	UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\DemoDevice");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\DemoDevice");

	// create IO device
	status = IoCreateDevice(pDriverObject,
		0, 
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		NULL, 
		FALSE,
		&pDeviceObject);

	if (!NT_SUCCESS(status)){
		DbgPrint("Failed to create device\n");
		return status;
	}

	//pDeviceObject->Flags |= DO_BUFFERED_IO; // Buffered IO
	//pDeviceObject->Flags |= DO_DIRECT_IO; // Direct IO
	//pDeviceObject->Flags = 0; // Neither IO

	status = IoCreateSymbolicLink(&symLink, &deviceName);
	if (!NT_SUCCESS(status)) {
		DbgPrint("Failed to create symbolic link\n");
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	// 例程
	pDriverObject -> MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	//pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	//pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;

	return STATUS_SUCCESS;
}