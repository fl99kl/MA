# MA (Master's Thesis Project)

## Overview
This project is part of a master's thesis focused on measuring the energy consumption of unit tests. The goal is to analyze the energy efficiency of various sorting to determine how useful this approach is, using tools like PAPI and RAPL to gather energy consumption data. The results are then stored and analyzed using a time-series database for further research and comparison across multiple runs.

## Features
- **Energy Efficiency Testing**: Measures the energy consumption of unit tests using PAPI and RAPL energy readings inside a .Net project.
- **Time-Series Analysis**: Stores and analyzes energy metrics over multiple test runs using a time-series database (InfluxDB).
- **Data Plotting**: Python Scripts to plot the data from the time-series database.
  
## Technologies
- **Languages**: C, C#, Python
- **Frameworks**: xUnit for unit testing, .Net for the example application
- **Measurement Tool**: PAPI (with RAPL)
- **Time-Series Database**: InfluxDB for storing energy consumption metrics

## Setup Instructions
First, ensure all requirements are installed and working as expected to run the program. Afterwards, the following setup steps are necessary: <br>
<code>export PAPI_DIR=PATH_TO_PAPI <br>
export PATH=${PAPI_DIR}/bin:$PATH <br>
export LD_LIBRARY_PATH=${PAPI_DIR}/lib:$LD_LIBRARY_PATH <br>
</code>
To find the PAPI directory use:
<code>
sudo find / -name "libpapi.so" 2>/dev/null
</code>
Afterwards, to run the project use the following commands:
<code>
cd CpuIntensiveApp <br>
gcc -fPIC -shared -o papi_wrapper.so papi_wrapper.c -I/${PAPI_DIR}/include -L/${PAPI_DIR}/lib -lpapi -lcurl <br>
cd ../CpuIntensiveApp.Tests
sudo env LD_LIBRARY_PATH=LIBRARY_PATH dotnet test
</code>

### Requirements
- [.NET SDK](https://dotnet.microsoft.com/download)
- [PAPI](https://github.com/icl-utk-edu/papi) with the RAPL component enabled
- [InfluxDB](https://docs.influxdata.com/influxdb/v2/)
