using System;
using System.Runtime.InteropServices;



class Program
{
	// DllImport-Attribut, um die C-Funktion zu importieren
	[DllImport("your_hello_world", CallingConvention = CallingConvention.Cdecl)]
	public static extern void hello_papi();

    static void Main(string[] args)
    {
        Console.WriteLine("first test");
		hello_papi();

        Console.WriteLine("second test");
    }
}
