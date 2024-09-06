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

	    public static List<int> BubbleSort(List<int> inputList)
    	{
        	var list = new List<int>(inputList); // Create a copy to avoid modifying the original list
        	for (int i = 0; i < list.Count - 1; i++)
        	{
            	for (int j = 0; j < list.Count - i - 1; j++)
            	{
                	if (list[j] > list[j + 1])
                	{
              	    	// Swap
          	         	(list[j], list[j + 1]) = (list[j + 1], list[j]);
	                }
            	}
        	}
        	return list;
    	}
	    public static List<int> SelectionSort(List<int> inputList)
    	{
        	var list = new List<int>(inputList); // Create a copy
        	for (int i = 0; i < list.Count - 1; i++)
        	{
            	int minIndex = i;
            	for (int j = i + 1; j < list.Count; j++)
            	{
                	if (list[j] < list[minIndex])
                	{
                    	minIndex = j;
                	}
            	}
            	// Swap
            	(list[i], list[minIndex]) = (list[minIndex], list[i]);
	        }
        	return list;
    	}

    	public static List<int> InsertionSort(List<int> inputList)
    	{
        	var list = new List<int>(inputList);
        	for (int i = 1; i < list.Count; i++)
        	{
            	int key = list[i];
            	int j = i - 1;
            	while (j >= 0 && list[j] > key)
            	{
                	list[j + 1] = list[j];
                	j--;
           		}
            	list[j + 1] = key;
        	}
        	return list;
    	}

		public static List<int> MergeSort(List<int> inputList)
    	{
        	if (inputList.Count <= 1)
        	{
            	return inputList;
        	}

        	var mid = inputList.Count / 2;
        	var left = MergeSort(inputList.Take(mid).ToList());
        	var right = MergeSort(inputList.Skip(mid).ToList());

        	return Merge(left, right);
    	}

    	private static List<int> Merge(List<int> left, List<int> right)
    	{
        	var result = new List<int>();
        	int i = 0, j = 0;
        	while (i < left.Count && j < right.Count)
        	{
            	if (left[i] <= right[j])
            	{
                	result.Add(left[i]);
                	i++;
            	}
            	else
            	{
                	result.Add(right[j]);
                	j++;
            	}
        	}

        	result.AddRange(left.Skip(i));
        	result.AddRange(right.Skip(j));
        	return result;
    	}
    }
    
    public class DebugTest : IDisposable
    {
	    private const string OutputPath = "/home/kleinert/MA/CpuIntensiveApp/output2.txt";
		private IntPtr _data;
		string _testCaseName = "";
        public DebugTest()
        {
			PapiWrapper.clearFile(OutputPath);
            PapiWrapper.outputStart(OutputPath);
        }

        public void SetTestCaseName(string newTestCaseName)
        {
	        _testCaseName = newTestCaseName;
        }

		public static void AddLineToFile(string testName) 
		{
            PapiWrapper.addLineToFile(OutputPath, testName);
		}

		public void BeforeTestCase() 
		{
        	_data = PapiWrapper.startRapl(OutputPath);
		}

		public void AfterTestCase() 
		{
        	PapiWrapper.TestCase papiResult = PapiWrapper.readAndStopRapl(_data, OutputPath, _testCaseName);
	        PapiWrapper.updateOrAddTestCase("/home/kleinert/MA/CpuIntensiveApp/ResultCsv.csv", papiResult);
		}

        public void Dispose()
        {
            PapiWrapper.outputEnd(OutputPath);
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
