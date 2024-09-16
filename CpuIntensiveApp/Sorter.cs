using System.Timers;
using Xunit;
using Timer = System.Threading.Timer;

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
		private static List<double> intermediatePackageValues = new();
		private static List<double> intermediateDramValues = new();
        public DebugTest()
        {
			PapiWrapper.clearFile(OutputPath);
            PapiWrapper.outputStart(OutputPath);
        }
        
        private static System.Timers.Timer aTimer;

        private void SetTimer()
        {
	        // run every 5 ms
	        aTimer = new System.Timers.Timer(5);
	        // Hook up the Elapsed event for the timer. 
	        aTimer.Elapsed += OnTimedEvent;
	        aTimer.AutoReset = true;
	        aTimer.Enabled = true;
        }

        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
	        var intermediateRaplResults = PapiWrapper.getIntermediateRaplResults(_data);

	        // Calculate the difference from the last added entry (if there is one)
	        double diff1 = intermediateRaplResults.total_energy_consumed_package - (intermediatePackageValues.Count > 0 ? intermediatePackageValues.Last() : 0);
	        double diff2 = intermediateRaplResults.total_energy_consumed_dram - (intermediateDramValues.Count > 0 ? intermediateDramValues.Last() : 0);

	        // Add the differences to the arrays
	        intermediatePackageValues.Add(diff1);
	        intermediateDramValues.Add(diff2);
        }
        
        // Function to calculate the median of an array
        public static double CalculateMedian(List<double> array)
        {
	        if (array.Count == 0)
		        return 0;

	        var sortedArray = array.OrderBy(x => x).ToList();
	        int count = sortedArray.Count;
	        if (count % 2 == 0)
	        {
		        // If even, return the average of the two middle elements
		        return (sortedArray[count / 2 - 1] + sortedArray[count / 2]) / 2;
	        }
	        else
	        {
		        // If odd, return the middle element
		        return sortedArray[count / 2];
	        }
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
	        SetTimer();
		}

		public void AfterTestCase() 
		{
			aTimer.Stop();
			aTimer.Dispose();
        	PapiWrapper.TestCase papiResult = PapiWrapper.readAndStopRapl(_data, OutputPath, _testCaseName);
	        papiResult.median_energy_consumed_package = CalculateMedian(intermediatePackageValues);
	        papiResult.median_energy_consumed_dram = CalculateMedian(intermediateDramValues);
	        PapiWrapper.updateOrAddTestCase("/home/kleinert/MA/CpuIntensiveApp/ResultCsv.csv", papiResult);
	        PapiWrapper.addTsdbEntry(papiResult);
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
