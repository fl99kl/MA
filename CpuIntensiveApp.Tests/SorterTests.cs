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
            var unsortedList = new List<int> { 5, 3, 8, 4, 2, 7, 1, 10, 9, 6 };
            var expectedList = new List<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            // Act
            var sortedList = Sorter.Sort(unsortedList);

            // Assert
            Assert.Equal(expectedList, sortedList);
        }
        [Fact]
        public void Sort_SortsListCorrectly2()
        {
            // Arrange
            var unsortedList = new List<int> { 5, 3, 8, 4, 2, 7, 1, 10, 9, 6 };
            var expectedList = new List<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            // Act
            var sortedList = Sorter.Sort(unsortedList);

            // Assert
            Assert.Equal(expectedList, sortedList);
        }
        [Fact]
        public void Sort_SortsListCorrectly3()
        {
            // Arrange
            var unsortedList = new List<int> { 5, 3, 8, 4, 2, 7, 1, 10, 9, 6 };
            var expectedList = new List<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            // Act
            var sortedList = Sorter.Sort(unsortedList);

            // Assert
            Assert.Equal(expectedList, sortedList);
        }
    }
}
