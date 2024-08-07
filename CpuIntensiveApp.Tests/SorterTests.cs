using System.Collections.Generic;
using Xunit;
using Xunit.Abstractions;
using CpuIntensiveApp;
using System.Globalization;

namespace CpuIntensiveApp.Tests
{
    public class SorterTests
    {
        private readonly ITestOutputHelper _testOutputHelper;

        public SorterTests(ITestOutputHelper testOutputHelper)
        {
            _testOutputHelper = testOutputHelper;
            Setup();
        }

        public void Setup()
        {
            // Setup code before each test
            _testOutputHelper.WriteLine("Setup code in Setup method before each test");
        }
        
        [Fact]
        public void Sort_SortsListCorrectly()
        {
            MyLibrary.HelloFromC();
            _testOutputHelper.WriteLine("example console statement");
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
