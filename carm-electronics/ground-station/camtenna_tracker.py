import sqlite3
from pantilt_library import PanTiltMount
from filterpy.kalman import KalmanFilter

# Connect to the database
conn = sqlite3.connect('rocket_data.db')
cursor = conn.cursor()

# Initialize the pan-tilt mount and Kalman filter
mount = PanTiltMount()
kalman_filter = KalmanFilter()

def update_mount_position():
    # Fetch the latest sensor data from the database
    cursor.execute("SELECT * FROM sensor_data ORDER BY time DESC LIMIT 1")
    sensor_data = cursor.fetchone()

    # Update the Kalman filter with the new sensor data
    kalman_filter.update(sensor_data)

    # Get the predicted position from the Kalman filter
    predicted_position = kalman_filter.get_predicted_position()

    # Calculate the desired pan and tilt angles based on the predicted position
    pan_angle = calculate_pan_angle(predicted_position)
    tilt_angle = calculate_tilt_angle(predicted_position)

    # Move the camera to the desired position
    mount.panDegrees(pan_angle)
    mount.tiltDegrees(tilt_angle)

    # Update the radio antenna orientation based on the predicted position
    antenna_orientation = calculate_antenna_orientation(predicted_position)
    mount.move_antenna(antenna_orientation)

# Main loop
while True:
    update_mount_position()
    # Add a delay or wait for new data to arrive

conn.close()