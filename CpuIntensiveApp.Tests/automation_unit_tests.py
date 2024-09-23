import subprocess
import time

# Number of times to execute the C program
num_executions = 15

# Path to the compiled C program
program_path = "dotnet test"

# Time to sleep (in seconds) between executions
sleep_duration = 10

# Library path (can be customized as needed)
ld_library_path = "/home/kleinert/papi/src/install/lib"

for i in range(num_executions):
    print(f"Execution {i + 1}/{num_executions}")

    # Execute the C program with sudo, env, and LD_LIBRARY_PATH
    subprocess.run([
        "sudo", "env", f"LD_LIBRARY_PATH={ld_library_path}", program_path
    ])

    # Sleep for the specified duration between executions
    if i < num_executions - 1:
        print(f"Sleeping for {sleep_duration} seconds...")
        time.sleep(sleep_duration)
