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

    # Drop unnecessary columns if they exist
    df = df.drop(columns=['result', 'table', '_start', '_stop', '_field', '_measurement'], errors='ignore')

    # Ensure DataFrame has columns we need (test names, removing '_time')
    test_columns = [col for col in df.columns if col != '_time']

    print(f"Test columns: {test_columns}")  # Print to check the available test columns

    # Add an individual run count for each test
    for test_name in test_columns:
        # Create a new column for the run count for each test
        df[f'{test_name}_run_count'] = df[test_name].notna().cumsum()
        print(f"{test_name} - number of valid entries: {df[test_name].notna().sum()}")  # Debugging info

    # Plotting the data for all tests
    plt.figure(figsize=(10, 6))
    for test_name in test_columns:
        if df[test_name].notna().sum() > 0:  # Only plot if there are valid values
            plt.plot(df[f'{test_name}_run_count'], df[test_name], label=f'{test_name}')
    
            # Print the values of the metric for valid entries
            valid_values = df[test_name][df[test_name].notna()]
            print(f"{test_name} - Valid values: {valid_values.tolist()}")

    # Adding labels and title
    plt.xlabel('Run Count')
    plt.ylabel(f'{metric}')
    plt.title(f'{metric} Over Run Count for All Tests')
    plt.legend(loc='upper right', bbox_to_anchor=(1.15, 1))
    plt.grid(True)

    # Save the plot
    plt.savefig(f'{metric}_all_tests.png')