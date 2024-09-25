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
token = "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
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

    # Ensure DataFrame has columns we need (test names, removing '_time')
    test_columns = [col for col in df.columns]

    print(f"Test columns: {test_columns}")  # Print to check the available test columns

    # Group by test_name
    grouped = df.groupby('test_name')

    # Create a single plot for all test_names
    plt.figure()

    # Create a plot for each test_name
    for name, group in grouped:
        plt.plot(range(1, len(group) + 1), group[f'{metric}'], label=name)
        print(f"y: {group[f'{metric}']}")  # Print to check values
        print(f"x: {name}")  # Print to check values

    # Adding labels and title
    plt.xlabel('Run Count')
    plt.ylabel(f'{metric}')
    plt.title(f'{metric} Over Run Count for All Tests')
    plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
    plt.grid(True)

    # Set the y-axis limits
    min_y = 100
    max_y = 140
    plt.ylim(min_y, max_y)

    # Save the plot
    plt.savefig(f'{metric}_all_tests.png')
