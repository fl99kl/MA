using System;
using System.Collections.Generic;
using Xunit;

namespace CpuIntensiveApp
{
    public static class Sorter
    {
        public static List<int> Sort(List<int> unsortedList)
        {
            unsortedList.Sort();
            return unsortedList;
        }
    }
    
    public class DebugTest : IDisposable
    {
        public DebugTest()
        {
            string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output2.txt";

            PapiWrapper.outputStart(outputPath);
        }

		public ClearFile() 
		{
			string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output2.txt";

            PapiWrapper.clearFile(outputPath);
		}

        public void Dispose()
        {
            string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output2.txt";
            PapiWrapper.outputEnd(outputPath);
        }
    }
    
    [CollectionDefinition("Debug collection")]
    public class DebugCollection : ICollectionFixture<DebugTest>
    {
        // This class has no code, and is never created. Its purpose is simply
        // to be the place to apply [CollectionDefinition] and all the
        // ICollectionFixture<> interfaces.
    }
}
