/*++
  NativeShell MHz measurer command
  Author: Generated for NativeShell project
  Purpose: Measure CPU frequency in MHz using RDTSC and display in native shell
--*/

#include "precomp.h" // NativeShell headers

// Convert cycles to MHz string with 2 decimal places
VOID UlongToAsciiDecimal(ULONG mhz100, char* buffer)
{
    ULONG integer = mhz100 / 100;
    ULONG fraction = mhz100 % 100;
    sprintf(buffer, "%lu.%02lu", integer, fraction);
}

// Measure CPU MHz over 3-second interval
ULONG MeasureMHz()
{
    ULONGLONG start = 0, end = 0, delta = 0;
    ULONG mhz100;
    UCHAR tmp[8];

    // Read starting TSC
    __asm {
        rdtsc
        mov dword ptr [tmp], eax
        mov dword ptr [tmp+4], edx
    }
    start = *(ULONGLONG*)tmp;

    // Wait 3 seconds using PIT-based busy wait
    KeStallExecutionProcessor(3000000); // 3,000,000 µs = 3 seconds

    // Read ending TSC
    __asm {
        rdtsc
        mov dword ptr [tmp], eax
        mov dword ptr [tmp+4], edx
    }
    end = *(ULONGLONG*)tmp;

    delta = end - start;

    // Convert to MHz * 100 for 2 decimal precision
    mhz100 = (ULONG)((delta * 100) / 3000000ULL);

    return mhz100;
}

// NativeShell command: single measurement
NTSTATUS CmdMeasureMHz(VOID)
{
    char buf[32];
    ULONG mhz100 = MeasureMHz();
    UlongToAsciiDecimal(mhz100, buf);

    RtlCliDisplayString("CPU MHz: %s\n", buf);

    return STATUS_SUCCESS;
}

// NativeShell command: continuous loop
NTSTATUS CmdMeasureMHzLoop(VOID)
{
    char buf[32];

    while (TRUE)
    {
        ULONG mhz100 = MeasureMHz();
        UlongToAsciiDecimal(mhz100, buf);

        // \r overwrites previous line
        RtlCliDisplayString("\rCPU MHz: %s  ", buf);

        KeStallExecutionProcessor(500000); // 0.5 second delay
    }

    return STATUS_SUCCESS;
}
