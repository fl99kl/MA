import pandas as pd
import matplotlib.pyplot as plt
from influxdb_client import InfluxDBClient
import argparse

# Parse command-line arguments to get the metric name
parser = argparse.ArgumentParser(description='Query and plot energy consumption metric for all tests.')
parser.add_argument('--metric', type=str, default='avg_energy_pkg', help='Metric to query (e.g., avg_energy_pkg)')
parser.add_argument('--bucket', type=str, default='myBucket', help='Metric to query (e.g., avg_energy_pkg)')
args = parser.parse_args()

# InfluxDB connection details
url = "http://localhost:8086"
token = "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
org = "MA"
# Use the values from the command-line argument
metric = args.metricbucket = "myBucket"
bucket = args.bucket = "myBucket"

# Create an InfluxDB client
client = InfluxDBClient(url=url, token=token, org=org)

# Define the Flux query for all tests but only for the selected metric
query = f'''
from(bucket: "{bucket}")
  |> range(start: -30d)
  |> filter(fn: (r) => r._measurement == "unit_test_energy" and r._field == "{metric}")
  |> pivot(rowKey:["_time"], columnKey: ["test_name"], valueColumn: "_value")
  |> sort(columns: ["_time"])
'''

# Execute the query and convert the result into a pandas DataFrame
result = client.query_api().query_data_frame(query)
df = pd.DataFrame(result)

if df.empty:
    print(f"No data found for metric: {metric}")
else:
    print(df.columns)

    # Ensure DataFrame has columns we need (time and test names)
    test_columns = [col for col in df.columns if col != '_time']
    df.columns = ['time'] + test_columns

    # Plotting the data for all tests
    plt.figure(figsize=(10, 6))
    for test_name in test_columns:
        plt.plot(df['time'], df[test_name], label=f'{test_name}')

    # Adding labels and title
    plt.xlabel('Time')
    plt.ylabel(f'{metric}')
    plt.title(f'{metric} Over Time for All Tests')
    plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
    plt.grid(True)

    # Save the plot
    plt.savefig(f'{metric}_all_tests.png')
