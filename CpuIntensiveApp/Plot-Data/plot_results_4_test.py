import pandas as pd
import matplotlib.pyplot as plt
from influxdb_client import InfluxDBClient
import argparse

# Parse command-line arguments to get the test name
parser = argparse.ArgumentParser(description='Query and plot energy consumption of a unit test.')
parser.add_argument('--test_name', type=str, required=True, help='Name of the Unit Test to query from InfluxDB')
parser.add_argument('--bucket_name', type=str, required=True, help='Name of the Bucket to query from InfluxDB')
args = parser.parse_args()

# InfluxDB connection details
url = "http://localhost:8086"
token = "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
org = "MA"
# Use the values from the command-line argument in the Flux query
test_name = args.test_name
bucket = args.bucket_name

# Create an InfluxDB client
client = InfluxDBClient(url=url, token=token, org=org)

# Define the Flux query, filtering by the test name
query = f'''
from(bucket: "{bucket}")
  |> range(start: -30m)
  |> filter(fn: (r) => r._measurement == "unit_test_energy" and r.test_name == "{test_name}")
  |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> sort(columns: ["_time"])
'''

# Execute the query and convert to DataFrame
result = client.query_api().query_data_frame(query)
df = pd.DataFrame(result)

if df.empty:
    print(f"No data found for test: {test_name}")
else:
    print(df.columns)

    # Ensure DataFrame columns are properly named
    df = df[['_time', 'test_name', 'duration', 'avg_energy_pkg', 'total_energy_pkg', 'median_energy_pkg', 'avg_energy_dram', 'total_energy_dram', 'median_energy_dram']]
    df.columns = ['time', 'test_name', 'duration', 'avg_energy_pkg', 'total_energy_pkg', 'median_energy_pkg', 'avg_energy_dram', 'total_energy_dram', 'median_energy_dram']

    # Create a new column for the x-axis index (starting from 1)
    df['data_point'] = range(1, len(df) + 1)

    # Plotting the data, using 'data_point' for x-axis instead of 'time'
    plt.figure(figsize=(10, 6))
    plt.plot(df['data_point'], df['avg_energy_pkg'], label='Average Energy Package')
    plt.plot(df['data_point'], df['total_energy_pkg'], label='Total Energy Package')
    plt.plot(df['data_point'], df['avg_energy_dram'], label='Average Energy DRAM')
    plt.plot(df['data_point'], df['total_energy_dram'], label='Total Energy DRAM')

    # Adding labels and title
    plt.xlabel('Data Point')
    plt.ylabel('Energy Consumption')
    plt.title(f'Energy Consumption Over Test Runs for {test_name}')
    plt.legend()
    plt.grid(True)

    # Save the plot with the test name
    plt.savefig(f'energy_consumption_{test_name}.png')
