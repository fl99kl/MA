using Xunit.Sdk;
using System.Reflection;

namespace CpuIntensiveApp.Tests;

public class LogTestNameAttribute : BeforeAfterTestAttribute
{
	public override void Before(MethodInfo methodUnderTest)
	{
		// Get DebugTest instance from static accessor in SorterTests
		var debugTest = SorterTests.GetDebugTestInstance();

		// Log the name of the test before it runs
		DebugTest.AddLineToFile(methodUnderTest.Name);

		debugTest.SetTestCaseName(methodUnderTest.Name);
	}
}

[Collection("Debug collection")]
public class SorterTests : IClassFixture<DebugTest>, IAsyncLifetime
{
	private static DebugTest _debugTest;

	public SorterTests(DebugTest debugTest)
	{
		_debugTest = debugTest;  // Store the instance in the static field
	}

	public async Task InitializeAsync()
	{
		// Asynchronous setup code that runs before each test
		await Task.Run(_debugTest.BeforeTestCase);
	}

	public Task DisposeAsync()
	{
		// Asynchronous cleanup code that runs after each test
		_debugTest.AfterTestCase();
		return Task.CompletedTask;
	}
	
	// Static accessor for DebugTest so the attribute can access it
	public static DebugTest GetDebugTestInstance()
	{
		return _debugTest;
	}
        
	[LogTestName]
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
	
	[LogTestName]
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
	
	[LogTestName]
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
	
	[LogTestName]
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

	[LogTestName]
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

	[LogTestName]
	[Fact]
	public void SleepingTest()
	{
		
		Thread.Sleep(5000);

		Assert.True(true);
	}

	[LogTestName]
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

	[LogTestName]
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

	[LogTestName]
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