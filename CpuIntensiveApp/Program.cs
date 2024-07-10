using System;
using System.Runtime.InteropServices;

class PapiWrapper
{
    [DllImport("libs/libpapi_wrapper.so", EntryPoint = "init_papi")]
    public static extern int InitPapi();

    [DllImport("libs/libpapi_wrapper.so", EntryPoint = "start_counters")]
    public static extern int StartCounters();

    [DllImport("libs/libpapi_wrapper.so", EntryPoint = "stop_counters")]
    public static extern int StopCounters(long[] values, int numEvents);
}

class Program
{
    static void Main(string[] args)
    {
        if (PapiWrapper.InitPapi() != 0)
        {
            Console.WriteLine("Failed to initialize PAPI");
            return;
        }

        if (PapiWrapper.StartCounters() != 0)
        {
            Console.WriteLine("Failed to start counters");
            return;
        }

        // Your code to be measured here

        long[] values = new long[2];
        if (PapiWrapper.StopCounters(values, 2) != 0)
        {
            Console.WriteLine("Failed to stop counters");
            return;
        }

        Console.WriteLine($"Counter 1: {values[0]}");
        Console.WriteLine($"Counter 2: {values[1]}");
    }
}
