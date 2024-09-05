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
		string outputPath = "/home/kleinert/MA/CpuIntensiveApp/output2.txt";
		public IntPtr data;
        public DebugTest()
        {
			PapiWrapper.clearFile(outputPath);
            PapiWrapper.outputStart(outputPath);
        }

		public void Add1ToFile() 
		{
            PapiWrapper.add1ToFile(outputPath);
		}

		public void Add2ToFile() 
		{
            PapiWrapper.add2ToFile(outputPath);
		}

		public void BeforeTestCase() 
		{
        	data = PapiWrapper.startRapl(outputPath);
		}

		public void AfterTestCase() 
		{
        	PapiWrapper.readAndStopRapl(data, outputPath);
		}

        public void Dispose()
        {
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
