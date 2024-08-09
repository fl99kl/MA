using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("Test before");

        string outputPath = "/path/to/output.txt";
        IntPtr raplData = startRapl(outputPath);
        readAndStopRapl(raplData, outputPath);

        Console.WriteLine("Test after rapl");
    }
}
