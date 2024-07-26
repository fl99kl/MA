using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("Test before");
        // Stop PAPI counters and read values
        PapiWrapper.runRapl();
        Console.WriteLine("Test after rapl");
    }
}
