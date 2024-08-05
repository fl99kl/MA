using System;
using System.Runtime.InteropServices;

public static class PapiWrapper
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RaplData
    {
        public int EventSet;
        public long before_time;
        public int num_events;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public EventInfo[] event_infos;
    }
    
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct EventInfo
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string event_name;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string unit;
        public int data_type;
    }
    
    const string dllPath = "papi_wrapper.so"; 
    
    [DllImport("papi_wrapper.so", EntryPoint = "runRapl")]
    public static extern void runRapl();
    
    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr startRapl();

    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void readAndStopRapl(IntPtr raplDataPtr);

}
