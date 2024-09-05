using System.Collections.Generic;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;
using CpuIntensiveApp;
using System.Globalization;

namespace CpuIntensiveApp.Tests
{
    [Collection("Debug collection")]
    public class SorterTests : IClassFixture<DebugTest>, IAsyncLifetime
    {
        private DebugTest _debugTest;

        public SorterTests(DebugTest debugTest)
        {
            //Setup();
            this._debugTest = debugTest;
		}

        public async Task InitializeAsync()
        {
			// Asynchronous setup code that runs before each test
        	await Task.Run(() => _debugTest.BeforeTestCase());
        }

        public Task DisposeAsync()
        {   
            // Asynchronous cleanup code that runs after each test.
            _debugTest.AfterTestCase();
            return Task.CompletedTask;
        }
        
        [Fact]
        [DisplayTestMethodName]
		public void Sort_SortsListCorrectly()
        {
            // Arrange
    		int arrayLength = 1000; // Change this value to test different lengths
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		// Act
    		var sortedList = Sorter.Sort(unsortedList);

    		// Assert
    		Assert.Equal(expectedList, sortedList);
		}
        [Fact]
        public void Sort_SortsListCorrectly2()
        {
            // Arrange
    		int arrayLength = 500; // Change this value to test different lengths
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		// Act
    		var sortedList = Sorter.Sort(unsortedList);

    		// Assert
    		Assert.Equal(expectedList, sortedList);
		}
        [Fact]
        public void Sort_SortsListCorrectly3()
        {
            // Arrange
    		int arrayLength = 100; // Change this value to test different lengths
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		// Act
    		var sortedList = Sorter.Sort(unsortedList);

    		// Assert
    		Assert.Equal(expectedList, sortedList);
		}
        [Fact]
        public void Sort_SortsListCorrectly4()
        {
            // Arrange
    		int arrayLength = 1000; // Change this value to test different lengths
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		// Act
    		var sortedList = Sorter.Sort(unsortedList);

    		// Assert
    		Assert.Equal(expectedList, sortedList);
		}

		[Fact]
		public void BubbleSort_SortsListCorrectly()
		{
    		int arrayLength = 1000;
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		var sortedList = Sorter.BubbleSort(unsortedList);

    		Assert.Equal(expectedList, sortedList);
		}

		[Fact]
		public void SelectionSort_SortsListCorrectly()
		{	
    		int arrayLength = 1000;
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		var sortedList = Sorter.SelectionSort(unsortedList);

    		Assert.Equal(expectedList, sortedList);
		}

		[Fact]
		public void InsertionSort_SortsListCorrectly()
		{
    		int arrayLength = 1000;
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		var sortedList = Sorter.InsertionSort(unsortedList);

    		Assert.Equal(expectedList, sortedList);
		}

		[Fact]
		public void MergeSort_SortsListCorrectly()
		{
    		int arrayLength = 1000;
    		var random = new Random();
    		var unsortedList = Enumerable.Range(1, arrayLength).OrderBy(x => random.Next()).ToList();
    		var expectedList = Enumerable.Range(1, arrayLength).ToList();

    		var sortedList = Sorter.MergeSort(unsortedList);

    		Assert.Equal(expectedList, sortedList);
		}

    	private class DisplayTestMethodNameAttribute : BeforeAfterTestAttribute
    	{
        	public override void Before(MethodInfo methodUnderTest)
        	{
				_debugTest.AddLineToFile(methodUnderTest.Name);
        	}

        	public override void After(MethodInfo methodUnderTest)
        	{
            	// Console.WriteLine("TearDown for test '{0}.'", methodUnderTest.Name);
        	}
    	}
    }
}
