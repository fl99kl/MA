﻿using System.Timers;
using Xunit;

namespace CpuIntensiveApp;

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
	private const int MsTimer = 50;
	public DebugTest()
	{
		PapiWrapper.clearFile(OutputPath);
	}
        
	private static System.Timers.Timer aTimer;

	private void SetTimer()
	{
		// run every 5 ms
		aTimer = new System.Timers.Timer(MsTimer);
		// Hook up the Elapsed event for the timer. 
		aTimer.Elapsed += OnTimedEvent;
		aTimer.AutoReset = true;
		aTimer.Enabled = true;
	}

	private void OnTimedEvent(Object source, ElapsedEventArgs e)
	{
		var intermediateRaplResults = PapiWrapper.getIntermediateRaplResults(_data, intermediatePackageValues.Count > 0 ? intermediatePackageValues.Last() : 0, intermediateDramValues.Count > 0 ? intermediateDramValues.Last() : 0);

		// Add the differences to the arrays
		intermediatePackageValues.Add(intermediateRaplResults.total_energy_consumed_package);
		intermediateDramValues.Add(intermediateRaplResults.total_energy_consumed_dram);
	}
        
	// Function to calculate the median of an array
	public static double CalculateMedian(List<double> array)
	{
		const double timerInSeconds = MsTimer / 1000d;
		if (array.Count == 1)
		{
			// If there's only one entry, return 0 or the single entry directly based on your requirement.
			// If you want to return the single entry as the median:
			return array[0] / timerInSeconds;
		}

		if (array.Count < 2)
		{
			// If there are fewer than 2 entries, return 0 or handle it as required.
			return 0;
		}

		double[] differences = new double[array.Count - 1];
		// Calculate differences between consecutive elements
		for (int i = 1; i < array.Count; i++)
		{
			differences[i - 1] = array[i] - array[i - 1];
		}

		var sortedDifferences = differences.OrderBy(x => x).ToList();
		int count = sortedDifferences.Count;

		// Calculate median of the sorted differences array
		double median;
		if (count % 2 == 0)
		{
			// If even, return the average of the two middle elements
			median = (sortedDifferences[count / 2 - 1] + sortedDifferences[count / 2]) / 2;
		}
		else
		{
			// If odd, return the middle element
			median = sortedDifferences[count / 2];
		}

		// Convert the median value based on timerInSeconds
		return median / timerInSeconds;
	}

	public void SetTestCaseName(string newTestCaseName)
	{
		_testCaseName = newTestCaseName;
	}

	public static void AddLineToFile(string testName) 
	{
		PapiWrapper.addLineToFile(OutputPath, testName);
	}
		
	    
	public static long GetCurrentTimestamp()
	{
		// Get current time in ticks from DateTimeOffset (gives precise system time)
		long currentTimeInTicks = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() * 1_000_000; // Convert milliseconds to nanoseconds
        
		// Return as nanoseconds
		return currentTimeInTicks;
	}
		
	public void BeforeTestCase()
	{
		intermediatePackageValues = new();
		intermediateDramValues = new();
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
		PapiWrapper.addLineToFile(OutputPath, "output end");
	}
}
    
[CollectionDefinition("Debug collection")]
public class DebugCollection : ICollectionFixture<DebugTest>
{
	// This class has no code, and is never created. Its purpose is simply
	// to be the place to apply [CollectionDefinition] and all the
	// ICollectionFixture<> interfaces.
}