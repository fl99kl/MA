# File path: boxplot_energy_consumption.py

import pandas as pd
import matplotlib.pyplot as plt
from influxdb_client import InfluxDBClient
import argparse

# Parse command-line arguments to get the metric name
parser = argparse.ArgumentParser(description='Query and plot energy consumption metric for all tests.')
parser.add_argument('--metric', type=str, default='avg_energy_pkg', help='Metric to query (e.g., avg_energy_pkg)')
parser.add_argument('--bucket', type=str, default='myBucket', help='Name of the Bucket to query from InfluxDb')
args = parser.parse_args()

# InfluxDB connection details
url = "http://localhost:8086"
token = "N9mKfB0tAgaQHk5h0MxIaBHE3tshaLH7a-qTvPIKe3XuZyLnugd5a8KnqHtt98FcsGi9g9l3eOBjphdoiaoOCw=="
org = "MA"

# Use the values from the command-line argument
metric = args.metric
bucket = args.bucket

# Create an InfluxDB client
client = InfluxDBClient(url=url, token=token, org=org)

# Define the Flux query for all tests but only for the selected metric
query = f'''
from(bucket: "{bucket}")
  |> range(start: -300m)
  |> filter(fn: (r) => r._measurement == "unit_test_energy" and r._field == "{metric}")
  |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> sort(columns: ["_time"])
'''

# Execute the query and convert the result into a pandas DataFrame
result = client.query_api().query_data_frame(query)
df = pd.DataFrame(result)

if df.empty:
    print(f"No data found for metric: {metric}")
else:
    print(df.columns)

    # Drop unnecessary columns if they exist
    df = df.drop(columns=['_start', '_stop', '_measurement', '_time'], errors='ignore')

    # List of test names to exclude from the graph
    excluded_tests = ['idle_consumption', 'SleepingTest', 'Sort_SortsListCorrectly', 'Sort_SortsListCorrectly4']

    # Filter out excluded tests
    df = df[~df['test_name'].isin(excluded_tests)]

    # Group data by 'test_name' and prepare for boxplot
    boxplot_data = [group[metric].dropna() for test_name, group in df.groupby('test_name')]

    # Adjust figure size for compactness
    plt.figure(figsize=(8, 5))  # Compact dimensions

    # Create the boxplot
    box = plt.boxplot(boxplot_data, vert=True, patch_artist=True, labels=df['test_name'].unique())

    # Adding labels and title
    plt.xlabel('Test Names')
    plt.ylabel('Metric Value')
    plt.title(f'Distribution of {metric} for All Tests')
    plt.xticks(rotation=45, ha='right')  # Rotate test names for better readability
    plt.grid(axis='y')

    # Manually adjust y-axis if needed
    all_values = pd.concat(boxplot_data)  # Combine all data to find min and max
    plt.ylim(all_values.min() - 5, all_values.max() + 5)  # Add padding around actual data

    # Label medians with dynamic placement
    for i, median_line in enumerate(box['medians']):
        median_value = median_line.get_ydata()[0]  # Get the median value
        q1 = box['boxes'][i].get_ydata()[1]       # Lower quartile (Q1)
        q3 = box['boxes'][i].get_ydata()[2]       # Upper quartile (Q3)

        # Determine vertical alignment dynamically
        if abs(median_value - q1) < abs(median_value - q3):
            # Closer to Q1, label above the median
            va = 'bottom'
            offset = 2  # Small vertical offset
        else:
            # Closer to Q3, label below the median
            va = 'top'
            offset = -2  # Small vertical offset

        # Add text annotation
        plt.text(
            i + 1,  # X-coordinate (box index + 1 since it starts at 1)
            median_value + offset,  # Adjust Y-coordinate based on offset
            f'{median_value:.2f}',  # Text to display (rounded to 2 decimals)
            horizontalalignment='center',  # Center align text
            verticalalignment=va,  # Dynamic alignment
            fontsize=9, color='blue'
        )

    # Tight layout with adjusted margins for compactness
    plt.tight_layout(pad=1.0)  # Reduce padding to make the plot more compact
    plt.savefig(f'boxplot_{metric}.png')
