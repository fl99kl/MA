using System;
using System.Runtime.InteropServices;

public static class PapiWrapper
{
    [DllImport("papi_wrapper.so", EntryPoint = "initialize_papi")]
    public static extern void runRapl();

}
