using Xunit.Sdk;
using System.Reflection;

namespace CpuIntensiveApp.Tests;

public class LogEnergyConsumptionAttribute : BeforeAfterTestAttribute
{
	// Code to run before each decorated test case. This is always necessary to receive the current name of the unit test.
	public override void Before(MethodInfo methodUnderTest)
	{
		// Get DebugTest instance from static accessor in SorterTests
		var testWrapper = SorterTests.GetTestWrapperInstance();
		// Log and set the name of the test before it runs
		TestWrapper.AddLineToFile(methodUnderTest.Name);
		testWrapper.SetTestCaseName(methodUnderTest.Name);
		testWrapper.BeforeTestCase();
	}

	public override void After(MethodInfo methodUnderTest)
	{
		var testWrapper = SorterTests.GetTestWrapperInstance();
		testWrapper.AfterTestCase();
		Thread.Sleep(5000);
	}
}

[Collection("Test Wrapper Collection")]
public class SorterTests : IClassFixture<TestWrapper>
{
	private const int ArrayLength = 50000; // Change this value to test different lengths
	private static List<int> _unsortedList;
	private static List<int> _expectedList;
	private static TestWrapper _testWrapper = null!;

	public SorterTests(TestWrapper testWrapper)
	{
		_testWrapper = testWrapper;  // Store the instance in the static field
		var random = new Random(42); // Use a fixed seed for reproducibility
		_unsortedList = Enumerable.Range(1, ArrayLength).OrderBy(_ => random.Next()).ToList();
		_expectedList = Enumerable.Range(1, ArrayLength).ToList();
	}

	// Static accessor for DebugTest so the attribute can access it
	public static TestWrapper GetTestWrapperInstance()
	{
		return _testWrapper;
	}

	[LogEnergyConsumption]
	[Fact]
	public void Sort_SortsListCorrectly()
	{
		// Arrange
		const int arrayLength = 1000; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		// Act
		var sortedList = Sorter.Sort(unsortedList);

		// Assert
		Assert.Equal(expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void BuiltInSort_SortsListCorrectly()
	{
		// Act
		var sortedList = Sorter.Sort(_unsortedList);

		// Assert
		Assert.Equal(_expectedList, sortedList);
	}

	/*[LogEnergyConsumption]
	[Fact]
	public void Sort_SortsListCorrectly3()
	{
		// Arrange
		const int arrayLength = 100; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		// Act
		var sortedList = Sorter.Sort(unsortedList);

		// Assert
		Assert.Equal(expectedList, sortedList);
	} */

	[LogEnergyConsumption]
	[Fact]
	public void MergeSort_SortsListCorrectly()
	{
		var sortedList = Sorter.MergeSort(_unsortedList);

		Assert.Equal(_expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void BubbleSort_SortsListCorrectly()
	{
		var sortedList = Sorter.BubbleSort(_unsortedList);

		Assert.Equal(_expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void SleepingTest()
	{

		Thread.Sleep(5000);

		Assert.True(true);
	}

	[LogEnergyConsumption]
	[Fact]
	public void SelectionSort_SortsListCorrectly()
	{
		var sortedList = Sorter.SelectionSort(_unsortedList);

		Assert.Equal(_expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_10Entries()
	{
		const int arrayLength = 10; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();
		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}
	
	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_100Entries()
	{
		const int arrayLength = 100; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();
		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}	
	
	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_1000Entries()
	{
		const int arrayLength = 1000; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();
		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}	
	
	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_10000Entries()
	{
		const int arrayLength = 10000; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();
		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}	
	
	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_100000Entries()
	{
		const int arrayLength = 100000; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();
		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}
	

	[LogEnergyConsumption]
	[Fact]
	public void Sort_SortsListCorrectly4()
	{
		// Arrange
		const int arrayLength = 1000; // Change this value to test different lengths
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		// Act
		var sortedList = Sorter.Sort(unsortedList);

		// Assert
		Assert.Equal(expectedList, sortedList);
	}
}
