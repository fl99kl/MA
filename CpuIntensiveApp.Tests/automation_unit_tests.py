import subprocess
import time

# Number of times to execute the command
num_executions = 15

# Command and environment variable setup
ld_library_path = "/home/kleinert/papi/src/install/lib"
command = ["sudo", "env", f"LD_LIBRARY_PATH={ld_library_path}", "dotnet", "test"]

# Time to sleep (in seconds) between executions
sleep_duration = 10

for i in range(num_executions):
    print(f"Execution {i + 1}/{num_executions}")

    # Execute the command with sudo and LD_LIBRARY_PATH
    subprocess.run(command)

    # Sleep for the specified duration between executions
    if i < num_executions - 1:
        print(f"Sleeping for {sleep_duration} seconds...")
        time.sleep(sleep_duration)
