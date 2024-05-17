import serial
import serial.tools.list_ports
import time
from tqdm import tqdm
import numpy as np


ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()

portsList = []

for onePort in ports:
    portsList.append(str(onePort))
    print(str(onePort))

val = input("Select Port: COM")

for x in range(0, len(portsList)):
    if portsList[x].startswith("COM" + str(val)):
        portVar = "COM" + str(val)
        print(portVar)

serialInst.baudrate = 9600
serialInst.port = portVar
serialInst.open()

time.sleep(2)  # Wait for the serial connection to initialize

numberOfIterations = 10000  # Number of readings to collect

accelHistoryX = []
accelHistoryY = []
accelHistoryZ = []

gyroHistoryX = []
gyroHistoryY = []
gyroHistoryZ = []

baroHistory = []

filename = "sensor_data.txt"
with open(filename, "w") as file:

    # Main loop to read and parse serial data
    for _ in tqdm(range(numberOfIterations)):
        data = serialInst.readline().decode().strip()
        if data:
            # Parse the received data
            parts = data.split("|")
            if len(parts) == 3:
                # Extract accelerometer values
                accel_values = parts[0].split(":")[1].strip().split(",")
                accel = [float(val.strip()) for val in accel_values]

                # Extract gyro values
                gyro_values = parts[1].split(":")[1].strip().split(",")
                gyro = [float(val.strip()) for val in gyro_values]

                # Extract barometer value
                baro_value = parts[2].split(":")[1].strip()
                baro = float(baro_value)

                # Append values to history lists
                accelHistoryX.append(accel[0])
                accelHistoryY.append(accel[1])
                accelHistoryZ.append(accel[2])
                gyroHistoryX.append(gyro[0])
                gyroHistoryY.append(gyro[1])
                gyroHistoryZ.append(gyro[2])
                baroHistory.append(baro)

                # Write data to file
                file.write(
                    f"Accel: {accel[0]}, {accel[1]}, {accel[2]} | Gyro: {gyro[0]}, {gyro[1]}, {gyro[2]} | Baro: {baro}\n"
                )

        time.sleep(0.1)  # Adjust delay as needed to match the Arduino's loop delay

serialInst.close()

# Calculate standard deviations for each axis
sigmaAccelX = np.std(accelHistoryX)
sigmaAccelY = np.std(accelHistoryY)
sigmaAccelZ = np.std(accelHistoryZ)

sigmaGyroX = np.std(gyroHistoryX)
sigmaGyroY = np.std(gyroHistoryY)
sigmaGyroZ = np.std(gyroHistoryZ)

sigmaBaro = np.std(baroHistory)

print("Accelerometer Standard Deviation:")
print("     X - ", sigmaAccelX, " Y - ", sigmaAccelY, " Z - ", sigmaAccelZ)
print("Gyroscope Standard Deviation:")
print("     X - ", sigmaGyroX, " Y - ", sigmaGyroY, " Z - ", sigmaGyroZ)
print("Barometer Standard Deviation:", sigmaBaro)
