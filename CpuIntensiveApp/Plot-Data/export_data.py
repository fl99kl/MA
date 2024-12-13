from influxdb_client import InfluxDBClient
import csv

# Configuration
url = "http://localhost:8086"  # Replace with your InfluxDB URL
token = "N9mKfB0tAgaQHk5h0MxIaBHE3tshaLH7a-qTvPIKe3XuZyLnugd5a8KnqHtt98FcsGi9g9l3eOBjphdoiaoOCw=="  # Replace with your InfluxDB token
org = "MA"  # Replace with your organization name
bucket = "myBucket"  # Replace with your bucket name
output_file = "outputFinal.csv"  # Name of the output file

# Query to extract data
query = f'''from(bucket: "{bucket}")
  |> range(start: -3d)
'''


# Function to write query results to CSV
def write_to_csv(results, output_file):
    with open(output_file, mode='w', newline='') as file:
        writer = csv.writer(file)

        # Write header
        header_written = False
        for table in results:
            for record in table.records:
                if not header_written:
                    writer.writerow(record.values.keys())
                    header_written = True
                # Write rows
                writer.writerow(record.values.values())


# Main script
if __name__ == "__main__":
    try:
        with InfluxDBClient(url=url, token=token, org=org) as client:
            query_api = client.query_api()

            print("Querying data from InfluxDB...")
            result = query_api.query(query)

            print("Writing data to CSV...")
            write_to_csv(result, output_file)
            print(f"Data successfully written to {output_file}")
    except Exception as e:
        print(f"An error occurred: {e}")
