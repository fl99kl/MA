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

# Filter rows for relevant energy fields
energy_fields = ['total_energy_pkg', 'total_energy_dram']
energy_df = df[df['_field'].isin(energy_fields)]

# Pivot the table to have energy fields as columns
pivoted = energy_df.pivot_table(
    index=['test_name', '_time'],  # Use both test_name and _time to group by unique runs
    columns='_field',
    values='_value',
    aggfunc='sum',
    fill_value=0
).reset_index()

# Ensure both energy fields exist (even if they weren't in the data)
pivoted = pivoted.reindex(columns=['test_name', '_time'] + energy_fields, fill_value=0)

# Calculate the total consumed energy for each run
pivoted['total_energy'] = pivoted['total_energy_pkg'] + pivoted['total_energy_dram']

# Group by test_name to calculate sums and averages
results = pivoted.groupby('test_name').agg(
    total_energy_sum=('total_energy', 'sum'),
    entry_count=('_time', 'count'),  # Count unique runs by `_time`
).reset_index()

# Calculate average energy
results['average_energy'] = results['total_energy_sum'] / results['entry_count']

# Display results
print("Test Case Energy Consumption Summary:")
print(results[['test_name', 'total_energy_sum', 'entry_count', 'average_energy']])

# Example formatting for better readability (optional)
print("\nFormatted Results:")
for _, row in results.iterrows():
    print(f"Test Case: {row['test_name']}")
    print(f"  Total Energy Sum: {row['total_energy_sum']:.2f}")
    print(f"  Entry Count: {row['entry_count']}")
    print(f"  Average Energy: {row['average_energy']:.2f}")
    print("-" * 40)
