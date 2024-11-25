import pandas as pd
import matplotlib
matplotlib.use('Agg')  # Use the 'Agg' backend for non-GUI environments
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
token = "N9mKfB0tAgaQHk5h0MxIaBHE3tshaLH7a-qTvPIKe3XuZyLnugd5a8KnqHtt98FcsGi9g9l3eOBjphdoiaoOCw=="
org = "MA"
# Use the values from the command-line argument in the Flux query
test_name = args.test_name
bucket = args.bucket_name

# Create an InfluxDB client
client = InfluxDBClient(url=url, token=token, org=org)

# Define the Flux query, filtering by the test name
query = f'''
from(bucket: "{bucket}")
  |> range(start: -300m)
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

    # Calculate the average values
    avg_pkg = df['avg_energy_pkg'].mean()
    avg_dram = df['avg_energy_dram'].mean()
    
    # Plotting the data, using 'data_point' for x-axis instead of 'time'
    plt.figure(figsize=(10, 6))
    plt.plot(df['data_point'], df['avg_energy_pkg'], label='Average for Processor')
    plt.plot(df['data_point'], df['avg_energy_dram'], label='Average for Memory')

    # Adding labels and title
    plt.xlabel('Number of Measurements')
    plt.ylabel('Average Electrical Power (W)')
    plt.title(f'Average Electrical Power Consumption for {test_name}')
    #plt.title(f'Average Electrical Power Consumption in Idle Mode')
    plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))  # Legend beside the graph
    plt.grid(True)

    # Annotate the average values on the plot
    x_offset = len(df['data_point']) + 2  # Increase the x position for better visibility
    plt.text(x_offset, avg_pkg, f'Overall Average Power: {avg_pkg:.2f}W', color='blue', fontsize=10, ha='left')
    plt.text(x_offset, avg_dram, f'Overall Average Power: {avg_dram:.2f}W', color='orange', fontsize=10, ha='left')
    
    # Adjust layout to prevent overlap
    plt.tight_layout(rect=[0, 0, 0.85, 1])  # Leave space for the legend
    
    # Save the plot with the test name
    plt.savefig(f'energy_consumption_{test_name}.png')

