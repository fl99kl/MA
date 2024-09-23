import subprocess
import time

# Number of times to execute the dotnet test command
num_executions = 15

# Command to execute the dotnet test
command = ["dotnet", "test"]

# Time to sleep (in seconds) between executions
sleep_duration = 10

for i in range(num_executions):
    print(f"Execution {i + 1}/{num_executions}")

    # Execute the dotnet test command
    subprocess.run(command)

    # Sleep for the specified duration between executions
    if i < num_executions - 1:
        print(f"Sleeping for {sleep_duration} seconds...")
        time.sleep(sleep_duration)
