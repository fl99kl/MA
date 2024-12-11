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
bucket = "myBucket"

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

    # Group by test_name
    grouped = df.groupby('test_name')

    # Create a plot for each test_name
    plt.figure(figsize=(10, 6))
    for test_name, group in grouped:
        if test_name in excluded_tests:
            continue
        plt.plot(range(1, len(group) + 1), group[metric], label=test_name)

        # Calculate and annotate the average value for this test
        avg_value = group[metric].mean()
        #plt.text(len(group), avg_value, f'Overall Average Power: {avg_value:.2f}', fontsize=9, ha='left', va='center')

    # Adding labels and title
    plt.xlabel('Number of Measurements')
    plt.ylabel('Average Electrical Power (W)')
    plt.title(f'Average Electrical Power Consumption for the processor')
    plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))  # Legend beside the graph
    plt.grid(True)

    # Adjust layout to prevent overlap
    plt.tight_layout(rect=[0, 0, 0.85, 1])  # Leave space for the legend
    plt.savefig(f'all_tests_{metric}.png')
