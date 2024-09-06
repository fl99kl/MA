class Program
{
    static void Main()
    {
        string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output3.txt";
		PapiWrapper.outputStart(outputPath);
		PapiWrapper.outputEnd(outputPath);
        //IntPtr raplData = PapiWrapper.startRapl(outputPath);
        //PapiWrapper.readAndStopRapl(raplData, outputPath);
    }
}
