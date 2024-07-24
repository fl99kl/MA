using System;
using System.Runtime.InteropServices;

public static class PapiWrapper
{
    [DllImport("libpapi_wrapper.so", EntryPoint = "initialize_papi")]
    public static extern void InitializePapi();

    [DllImport("libpapi_wrapper.so", EntryPoint = "start_counters")]
    public static extern void StartCounters();

    [DllImport("libpapi_wrapper.so", EntryPoint = "stop_counters")]
    public static extern void StopCounters(long[] values);
}
