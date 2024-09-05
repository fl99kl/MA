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
    
    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr startRapl(string outputFilePath);

    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void readAndStopRapl(IntPtr raplData, string outputFilePath);

    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void outputStart(string outputFilePath);

    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void outputEnd(string outputFilePath);
    
    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void clearFile(string outputFilePath);

    [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void addLineToFile(string outputFilePath, string testName);

}
