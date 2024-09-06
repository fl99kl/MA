using Xunit.Sdk;
using System.Reflection;

namespace CpuIntensiveApp.Tests;

public class LogTestNameAttribute : BeforeAfterTestAttribute
{
	public override void Before(MethodInfo methodUnderTest)
	{
		// Get DebugTest instance from test class context (TestContext)
		var debugTest = GetDebugTestFromTestClass(methodUnderTest);

		// Log the name of the test before it runs
		DebugTest.AddLineToFile("Hallo: " + methodUnderTest.Name);

		debugTest?.SetTestCaseName(methodUnderTest.Name);
	}

	private DebugTest? GetDebugTestFromTestClass(MethodInfo methodUnderTest)
	{
		// Fetch the test class instance and retrieve the DebugTest dependency
		var testClass = methodUnderTest.DeclaringType;
		var field = testClass?.GetField("_debugTest", BindingFlags.Instance | BindingFlags.NonPublic);

		return field?.GetValue(testClass) as DebugTest;
	}
}

[Collection("Debug collection")]
public class SorterTests(DebugTest debugTest) : IClassFixture<DebugTest>, IAsyncLifetime
{
	public async Task InitializeAsync()
	{
		// Asynchronous setup code that runs before each test
		await Task.Run(debugTest.BeforeTestCase);
	}

	public Task DisposeAsync()
	{   
		// Asynchronous cleanup code that runs after each test.
		debugTest.AfterTestCase();
		return Task.CompletedTask;
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