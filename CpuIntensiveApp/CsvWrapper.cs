using System;
using System.Runtime.InteropServices;

public class CsvWrapper
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TestCase
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 50)]
        public string test_case_id;
        public double metric1;
        public int metric2;
        public int metric3;
        
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
        public string timestamp;
    }

    public static class TestCaseLibWrapper
    {
        const string dllPath = "csv_wrapper.so";

        [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int read_csv(string filename, [In, Out] TestCase[] testCases, int maxTestCases);

        [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr find_test_case([In, Out] TestCase[] testCases, int numCases, string testCaseId);

        [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern void write_csv(string filename, [In, Out] TestCase[] testCases, int numCases);

        [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern void get_timestamp([Out] char[] buffer, int len);

        [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern void update_or_add_test_case(string filename, TestCase newCase);
    }
}