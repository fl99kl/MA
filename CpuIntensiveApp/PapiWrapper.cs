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
    public struct TestCase
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 50)]
        public string test_case_id;
        public double duration;
        public double total_energy_consumed_package;
        public double average_energy_consumed_package;
        public double median_energy_consumed_package;
        public double total_energy_consumed_dram;
        public double average_energy_consumed_dram;
        public double median_energy_consumed_dram;
        
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string timestamp;
    }
    
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct IntermediateTestCase
    {
        public double total_energy_consumed_package;
        public double total_energy_consumed_dram;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string timestamp;
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
    
    const string DllPath = "papi_wrapper.so";
    
    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr startRapl(string outputFilePath);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern TestCase readAndStopRapl(IntPtr raplData, string outputFilePath, string testCaseName);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void outputStart(string outputFilePath);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void outputEnd(string outputFilePath);
    
    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void clearFile(string outputFilePath);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void addLineToFile(string outputFilePath, string testName);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void updateOrAddTestCase(string filename, TestCase newCase);
    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void addTsdbEntry(TestCase newCase);
    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntermediateTestCase getIntermediateRaplResults(IntPtr raplData, double beforeTotalPackage, double beforeTotalDram);
}
