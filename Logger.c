#include "logging.h"
VOID Log(const char* format, ...)
{
    HANDLE fileHandle;
    UNICODE_STRING fileName;
    IO_STATUS_BLOCK ioStatusBlock;
    OBJECT_ATTRIBUTES objectAttributes;
    NTSTATUS status;

    RtlInitUnicodeString(&fileName, L"\\??\\C:\\log.txt");

    InitializeObjectAttributes(&objectAttributes, &fileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwCreateFile(&fileHandle, FILE_ALL_ACCESS, &objectAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, FILE_NON_DIRECTORY_FILE, NULL, 0);

    if (NT_SUCCESS(status))
    {
        FILE_STANDARD_INFORMATION fileInfo;
        status = ZwQueryInformationFile(fileHandle, &ioStatusBlock, &fileInfo, sizeof(fileInfo), FileStandardInformation);

        if (NT_SUCCESS(status)) {

            va_list args;
            va_start(args, format);

            char logMessage[1024];
            vsprintf(logMessage, format, args);

            status = ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, (PVOID)logMessage, (ULONG)strlen(logMessage), &fileInfo.EndOfFile, NULL);

            va_end(args);
        }
        ZwClose(fileHandle);
    }
}