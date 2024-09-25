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
	private static TestWrapper _testWrapper = null!;

	public SorterTests(TestWrapper testWrapper)
	{
		_testWrapper = testWrapper;  // Store the instance in the static field
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
	public void Sort_SortsListCorrectly2()
	{
		// Arrange
		const int arrayLength = 500; // Change this value to test different lengths
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

	[LogEnergyConsumption]
	[Fact]
	public void BubbleSort_SortsListCorrectly()
	{
		const int arrayLength = 1000;
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		var sortedList = Sorter.BubbleSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
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
		const int arrayLength = 1000;
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		var sortedList = Sorter.SelectionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void InsertionSort_SortsListCorrectly()
	{
		const int arrayLength = 1000;
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		var sortedList = Sorter.InsertionSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}

	[LogEnergyConsumption]
	[Fact]
	public void MergeSort_SortsListCorrectly()
	{
		const int arrayLength = 1000;
		var random = new Random();
		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(_ => random.Next()).ToList();
		var expectedList = Enumerable.Range(1, arrayLength).ToList();

		var sortedList = Sorter.MergeSort(unsortedList);

		Assert.Equal(expectedList, sortedList);
	}
}
