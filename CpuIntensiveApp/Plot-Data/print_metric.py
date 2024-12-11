import pandas as pd

# Load the CSV file
csv_path = 'output.csv'  # Replace with your actual file path
try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print(f"Error: File '{csv_path}' not found.")
    exit(1)

# Ensure the required columns exist
required_columns = ['_field', '_value', 'test_name', '_time']
if not all(column in df.columns for column in required_columns):
    print(f"Error: The CSV file must contain the following columns: {required_columns}")
    exit(1)

# Filter rows for the duration field
duration_field = 'duration'
duration_df = df[df['_field'] == duration_field]

# Pivot the table to have the duration field as a column
pivoted = duration_df.pivot_table(
    index=['test_name', '_time'],  # Use both test_name and _time to group by unique runs
    columns='_field',
    values='_value',
    aggfunc='sum',
    fill_value=0
).reset_index()

# Rename the column for clarity (optional)
pivoted = pivoted.rename(columns={duration_field: 'duration'})

# Group by test_name to calculate sums and averages
results = pivoted.groupby('test_name').agg(
    total_duration_sum=('duration', 'sum'),
    entry_count=('_time', 'count'),  # Count unique runs by `_time`
).reset_index()

# Calculate average duration
results['average_duration'] = results['total_duration_sum'] / results['entry_count']

# Display results
print("Test Case Duration Summary:")
print(results[['test_name', 'total_duration_sum', 'entry_count', 'average_duration']])

# Example formatting for better readability (optional)
print("\nFormatted Results:")
for _, row in results.iterrows():
    print(f"Test Case: {row['test_name']}")
    print(f"  Total Duration Sum: {row['total_duration_sum']:.6f}")
    print(f"  Entry Count: {row['entry_count']}")
    print(f"  Average Duration: {row['average_duration']:.6f}")
    print("-" * 40)
