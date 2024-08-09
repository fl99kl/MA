using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("Test before");

        string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output3.txt";
		PapiWrapper.startOutput(outputPath);
		PapiWrapper.endOutput(outputPath);
        //IntPtr raplData = PapiWrapper.startRapl(outputPath);
        //PapiWrapper.readAndStopRapl(raplData, outputPath);

        Console.WriteLine("Test after rapl");
    }
}
