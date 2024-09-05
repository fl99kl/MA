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
        private DebugTest test;

        public SorterTests(DebugTest _test)
        {
            //Setup();
            this.test = _test;
        }

        public async Task InitializeAsync()
        {
            // Asynchronous setup code that runs before each test.
            test.BeforeTestCase();
        }

        public Task DisposeAsync()
        {   
            // Asynchronous cleanup code that runs after each test.
            test.AfterTestCase();
            return Task.CompletedTask;
        }
        
        [Fact]
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
    }
}
