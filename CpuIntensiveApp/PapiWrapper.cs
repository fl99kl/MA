using System;
using System.Runtime.InteropServices;

public static class PapiWrapper
{
    [DllImport("papi_wrapper.so", EntryPoint = "runRapl")]
    public static extern void runRapl();

}
