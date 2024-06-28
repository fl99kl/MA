using System;
using System.Runtime.InteropServices;

public static class MyLibrary
{
    [DllImport("D:\\MA\\CpuIntensiveApp\\libmyclibrary.so", EntryPoint = "hello_from_c", CallingConvention = CallingConvention.Cdecl)]
    public static extern void HelloFromC();
}