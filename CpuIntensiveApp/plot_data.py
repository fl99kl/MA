import pandas as pd
import matplotlib.pyplot as plt
from influxdb_client import InfluxDBClient

# InfluxDB connection details
url = "http://localhost:8086"
token = "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
org = "MA"
bucket = "myBucket"

# Create an InfluxDB client
client = InfluxDBClient(url=url, token=token, org=org)

# Define the Flux query
query = '''
from(bucket: "myBucket")
  |> range(start: -30d)
  |> filter(fn: (r) => r._measurement == "unit_test_energy" and r.test_name == "SleepingTest")
  |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> sort(columns: ["_time"])
'''

# Execute the query and convert to DataFrame
result = client.query_api().query_data_frame(query)
df = pd.DataFrame(result)

print(df.columns)

# Ensure DataFrame columns are properly named
df = df[['_time', 'test_name', 'duration', 'avg_energy_pkg', 'total_energy_pkg', 'median_energy_pkg', 'avg_energy_dram', 'total_energy_dram', 'median_energy_dram', 'run_number']]
df.columns = ['time', 'test_name', 'duration', 'avg_energy_pkg', 'total_energy_pkg', 'median_energy_pkg', 'avg_energy_dram', 'total_energy_dram', 'median_energy_dram', 'run_number']

# Plotting the data
plt.figure(figsize=(10, 6))
plt.plot(df['run_number'], df['avg_energy_pkg'], label='Average Energy Package')
plt.plot(df['run_number'], df['total_energy_pkg'], label='Total Energy Package')
plt.plot(df['run_number'], df['avg_energy_dram'], label='Average Energy DRAM')
plt.plot(df['run_number'], df['total_energy_dram'], label='Total Energy DRAM')

# Adding labels and title
plt.xlabel('Run Number')
plt.ylabel('Energy Consumption')
plt.title('Energy Consumption Over Test Runs')
plt.legend()
plt.grid(True)

# Save the plot
plt.savefig('energy_consumption.png')