using System;

class Program
{
    static void Main()
    {
        // Initialize PAPI
        PapiWrapper.InitializePapi();

        long[] values = new long[2];

        // Start PAPI counters
        PapiWrapper.StartCounters();

        // Your CPU intensive code here

        // Stop PAPI counters and read values
        PapiWrapper.StopCounters(values);

        // Print the counter values
        Console.WriteLine($"Total cycles: {values[0]}");
        Console.WriteLine($"Total instructions: {values[1]}");
    }
}
