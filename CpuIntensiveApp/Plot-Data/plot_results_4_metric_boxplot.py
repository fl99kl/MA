import pandas as pd
import matplotlib.pyplot as plt
import argparse

# Parse command-line arguments to get the metric name and CSV file
parser = argparse.ArgumentParser(description='Plot energy consumption metric from CSV file.')
parser.add_argument('--metric', type=str, default='duration', help='Metric to plot (e.g., avg_energy_pkg)')
parser.add_argument('--csv', type=str, default='output.csv', help='Path to the CSV file with data')
args = parser.parse_args()

# Load data from the CSV file
try:
    df = pd.read_csv(args.csv)
except FileNotFoundError:
    print(f"Error: File '{args.csv}' not found.")
    exit(1)

# Ensure the specified metric exists in the data as a value in '_field'
metric = args.metric
if '_field' not in df.columns or '_value' not in df.columns:
    print("Error: The required '_field' or '_value' columns are missing in the CSV file.")
    exit(1)

# Filter data for the specified metric
filtered_df = df[df['_field'] == metric]
if filtered_df.empty:
    print(f"Error: No data found for metric '{metric}' in the CSV file.")
    exit(1)

# Drop unnecessary columns if they exist
drop_columns = ['_start', '_stop', '_measurement', '_time', '_field']
filtered_df = filtered_df.drop(columns=[col for col in drop_columns if col in filtered_df.columns], errors='ignore')

# List of test names to exclude from the graph
excluded_tests = ['idle_consumption', 'SleepingTest', 'Sort_SortsListCorrectly', 'Sort_SortsListCorrectly4']

# Filter out excluded tests if 'test_name' column exists
if 'test_name' in filtered_df.columns:
    filtered_df = filtered_df[~filtered_df['test_name'].isin(excluded_tests)]
    boxplot_data = [group['_value'].dropna() for test_name, group in filtered_df.groupby('test_name')]
    labels = filtered_df['test_name'].unique()
else:
    print("Error: 'test_name' column not found in the CSV file.")
    exit(1)

# Adjust figure size for compactness
plt.figure(figsize=(8, 5))  # Compact dimensions

# Create the boxplot
box = plt.boxplot(boxplot_data, vert=True, patch_artist=True, labels=labels)

# Adding labels and title
plt.xlabel('Name of Test Case')
plt.ylabel('Duration (s)')
plt.xticks(rotation=45, ha='right', fontsize=8)  # Rotate test names for better readability
plt.title('Duration for Test Cases')
plt.grid(axis='y')

# Manually adjust y-axis if needed
all_values = pd.concat(boxplot_data)  # Combine all data to find min and max
plt.ylim(all_values.min() - 5, all_values.max() + 5)  # Add padding around actual data

# Label medians dynamically
for i, median_line in enumerate(box['medians']):
    median_value = median_line.get_ydata()[0]

    # Position the median label
    if i == 0:  # For the leftmost boxplot
        plt.text(
            i + 1 + 0.3,  # Slightly to the right of the median line
            median_value,
            f'{median_value:.2f}',
            horizontalalignment='left',
            verticalalignment='center',
            fontsize=9, color='blue'
        )
    else:  # For all other boxplots
        plt.text(
            i + 1 - 0.3,  # Slightly to the left of the median line
            median_value,
            f'{median_value:.2f}',
            horizontalalignment='right',
            verticalalignment='center',
            fontsize=9, color='blue'
        )

# Tight layout with adjusted margins for compactness
plt.tight_layout(pad=1.0)  # Reduce padding to make the plot more compact
plt.savefig(f'boxplot_{metric}.png')
plt.show()
print(f"Boxplot saved as 'boxplot_{metric}.png'.")
