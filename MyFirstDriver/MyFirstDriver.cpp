//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutine
//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pcreate_process_notify_routine
//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetloadimagenotifyroutine
//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pload_image_notify_routine
//https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
//https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/writing-a-driverentry-routine

#include <intrin.h>
#include <ntifs.h>
#include <ntddk.h>

VOID ProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create) {
    UNREFERENCED_PARAMETER(ParentId);

    if (Create) {
        // Get the process object from the process ID
        PEPROCESS process;

        if (NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &process))) {
            // Get the process name
            PUNICODE_STRING processName = NULL;
            NTSTATUS status = SeLocateProcessImageName(process, &processName);

            if (NT_SUCCESS(status)) {

                // Print the process information
                KdPrint(("Process Created:\n"));
                KdPrint(("    Process ID (PID): %llu\n", (LONG64)ProcessId));
                KdPrint(("    Name: %wZ\n", processName));
         
                // Free the allocated buffer for the process name
                ExFreePool(processName);
            }

            // Release the process object
            ObDereferenceObject(process);
        }
    }
}

VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo) {
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ImageInfo);

    if (FullImageName) {
        KdPrint(("Image Loaded: %wZ\n", FullImageName));
    }
}

void SampleUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    // Remove the process and image load callbacks
    PsSetCreateProcessNotifyRoutine(ProcessNotifyRoutine, TRUE);
    PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);

    KdPrint(("MyFirstDriver Unload called\n"));
}


extern "C" NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    // Retrieve CPUID information
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    // Print CPUID information
    KdPrint(("CPUID Information:\n"));
    KdPrint(("   Processor family: %d\n", (cpuInfo[0] >> 8) & 0xf));
    KdPrint(("   Processor model: %d\n", (cpuInfo[0] >> 4) & 0xf));
    KdPrint(("   Processor stepping: %d\n", cpuInfo[0] & 0xf));
    KdPrint(("   Brand ID: %d\n", (cpuInfo[1] >> 16) & 0xff));
    KdPrint(("   APIC ID: %d\n", (cpuInfo[1] >> 24) & 0xff));

    KdPrint(("   Feature Information:\n"));
    KdPrint(("     SSE3 Instructions: %s\n", (cpuInfo[2] & 0x1) ? "Supported" : "Not supported"));
    KdPrint(("     PCLMULQDQ Instruction: %s\n", (cpuInfo[2] & 0x2) ? "Supported" : "Not supported"));
    KdPrint(("     64-bit Instruction: %s\n", (cpuInfo[2] & 0x200) ? "Supported" : "Not supported"));
    KdPrint(("     AVX Instruction: %s\n", (cpuInfo[2] & 0x1000) ? "Supported" : "Not supported"));
    KdPrint(("     FXSAVE/FXRSTOR: %s\n", (cpuInfo[2] & 0x4000) ? "Supported" : "Not supported"));
    KdPrint(("     Time Stamp Counter: %s\n", (cpuInfo[3] & 0x10) ? "Supported" : "Not supported"));
    KdPrint(("     AES Instruction: %s\n", (cpuInfo[2] & 0x2000000) ? "Supported" : "Not supported"));
    KdPrint(("     XSAVE/XRSTOR: %s\n", (cpuInfo[2] & 0x400000) ? "Supported" : "Not supported"));
    KdPrint(("     OS-Enabled Extended State Management: %s\n", (cpuInfo[2] & 0x10000000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512F Instruction: %s\n", (cpuInfo[2] & 0x20000000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512DQ Instruction: %s\n", (cpuInfo[2] & 0x40000000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512IFMA Instruction: %s\n", (cpuInfo[2] & 0x80000000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512PF Instruction: %s\n", (cpuInfo[3] & 0x10000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512ER Instruction: %s\n", (cpuInfo[3] & 0x20000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512CD Instruction: %s\n", (cpuInfo[3] & 0x40000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512BW Instruction: %s\n", (cpuInfo[3] & 0x80000) ? "Supported" : "Not supported"));
    KdPrint(("     AVX-512VL Instruction: %s\n", (cpuInfo[3] & 0x100000) ? "Supported" : "Not supported"));
    KdPrint(("     SGX: %s\n", (cpuInfo[2] & 0x40000000) ? "Supported" : "Not supported"));



    // Set driver unload routine
    DriverObject->DriverUnload = SampleUnload;

    // Register process and image load notification routines
    PsSetCreateProcessNotifyRoutine(ProcessNotifyRoutine, FALSE);
    PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);

    return STATUS_SUCCESS;
}