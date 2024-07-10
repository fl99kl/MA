using System;
using System.Runtime.InteropServices;

public static class MyLibrary
{
    [DllImport("libs/libmylibrary.so", EntryPoint = "hello_from_c", CallingConvention = CallingConvention.Cdecl)]
    public static extern void HelloFromC();
}
