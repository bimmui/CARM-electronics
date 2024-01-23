# CARM Electronics

This repository houses the source code for the flight computer software running on our rocket, **CARM**, as well as the ground station software responsible for monitoring and collecting data from the rocket during its flight. Whether you're a member of our development team, a collaborator, or an enthusiast interested in rocketry and software, you've come to the right place.

This document provides instructions on setting up the Conda environment for the project using the provided ```carm.yml``` file and prepping your Arduino environment with the proper libraries.


## Getting Started
The flight computer is based on the Adafruit Feather M0 RFM96 LoRa Radio (433MHz) and utilizes the Arduino environment for ease of library management and quick development. Therefore, the Arduino IDE is recommened for writing the flight code. The ground station uses Dash, a Python framework made by Plotly for creating interactive web applications. All Python packages are managed using Conda, however, you are free to use any package manager of your choosing.


Before doing anything, download the following:
- [Git](https://git-scm.com/)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Conda](https://docs.conda.io/en/latest/) or [Miniconda](https://docs.conda.io/projects/miniconda/en/latest/)

### Python Environment Setup

Follow the steps below (designed to work on Windows) to set up the Conda environment for the project:

```bash
# 1. Clone the Repository
git clone https://github.com/bimmui/CARM-electronics.git
cd CARM-electronics

# 2. Create Conda Environment
conda env create -f carm.yml -n carm

# 3. Activate the Conda Environment
conda activate carm

# 4. Verify Installation
conda list
```

### Arduino Environment Setup

1\. Open the Arduino IDE and navigate to the boards manager

2\. Download `Arduino SAMD Boards` (ver 1.7.13 at time of writing). This will allow the IDE to communicate with your Feather M0 (compile, upload code, access serial, etc).

3\. Navigate to the library manager. Download the following libraries and their dependencies. The version number at the time of writing is included.
- `Adafruit BMP3XX Library` (ver 2.1.4)
- `Adafruit_LSM9DS1` (ver 2.1.3)
- `Adafruit_MCP9808` (ver 2.0.2)
- `Adafruit GPS` (ver 1.7.4)
