import pandas as pd
import matplotlib
matplotlib.use('Agg')  # Use the 'Agg' backend for non-GUI environments
import matplotlib.pyplot as plt
import argparse

# Parse command-line arguments to get test name and CSV file
parser = argparse.ArgumentParser(description='Plot energy consumption of a unit test from CSV.')
parser.add_argument('--test_name', type=str, default='SelectionSort_SortsListCorrectly', help='Name of the Unit Test to filter')
parser.add_argument('--csv', type=str, default='output.csv', help='Path to the CSV file with data')
args = parser.parse_args()

# Load data from CSV file
try:
    df = pd.read_csv(args.csv)
except FileNotFoundError:
    print(f"Error: File '{args.csv}' not found.")
    exit(1)

# Ensure the required columns exist
required_columns = ['_value', '_field', 'test_name']
for col in required_columns:
    if col not in df.columns:
        print(f"Error: Required column '{col}' is missing from the CSV file.")
        exit(1)

# Filter data for the given test_name
filtered_df = df[df['test_name'] == args.test_name]

# Extract 'avg_energy_pkg' and 'avg_energy_dram' fields
pkg_df = filtered_df[filtered_df['_field'] == 'avg_energy_pkg']
dram_df = filtered_df[filtered_df['_field'] == 'avg_energy_dram']

if pkg_df.empty and dram_df.empty:
    print(f"No data found for test: {args.test_name}")
    exit(1)

# Assign x-axis points (number of measurements)
pkg_df = pkg_df.reset_index(drop=True)
dram_df = dram_df.reset_index(drop=True)
pkg_df['data_point'] = range(1, len(pkg_df) + 1)
dram_df['data_point'] = range(1, len(dram_df) + 1)

# Calculate medians
median_pkg = pkg_df['_value'].median() if not pkg_df.empty else None
median_dram = dram_df['_value'].median() if not dram_df.empty else None

# Plot the data
plt.figure(figsize=(10, 6))
if not pkg_df.empty:
    plt.plot(pkg_df['data_point'], pkg_df['_value'], label='Average Energy (Processor)', color='blue')
    plt.axhline(median_pkg, color='blue', linestyle='--', label=f'Median Energy (Processor): {median_pkg:.2f}W')
if not dram_df.empty:
    plt.plot(dram_df['data_point'], dram_df['_value'], label='Average Energy (Memory)', color='orange')
    plt.axhline(median_dram, color='orange', linestyle='--', label=f'Median Energy (Memory): {median_dram:.2f}W')

# Add labels and title
plt.xlabel('Number of Measurements')
plt.ylabel('Average Energy Consumption (W)')
plt.title(f'Energy Consumption for Test: {args.test_name}')
plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))  # Legend beside the graph
plt.grid(True)

# Annotate the median values on the plot
if median_pkg is not None:
    plt.text(len(pkg_df) * 0.8, median_pkg, f'{median_pkg:.2f}W', color='blue', fontsize=10, ha='right')
if median_dram is not None:
    plt.text(len(dram_df) * 0.8, median_dram, f'{median_dram:.2f}W', color='orange', fontsize=10, ha='right')

# Adjust layout and save the plot
plt.tight_layout(rect=[0, 0, 0.85, 1])  # Leave space for the legend
plt.savefig(f'energy_consumption_{args.test_name}.png')
print(f"Plot saved as 'energy_consumption_{args.test_name}.png'.")
