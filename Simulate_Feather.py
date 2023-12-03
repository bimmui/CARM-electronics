import random
import json
import time
from django.core.wsgi import get_wsgi_application
import os

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "carm_electronics.settings")
application = get_wsgi_application()

from carm_gui.models import TelemetryData


def generate_fake_data():
    return {
        "exterior_temp": random.uniform(-50, 50),
        "interior_temp": random.uniform(20, 30),
        "accelerometer_data": {
            "x": random.random(),
            "y": random.random(),
            "z": random.random(),
        },
        "gyro_data": {"x": random.random(), "y": random.random(), "z": random.random()},
        "magnetometer_data": {
            "x": random.random(),
            "y": random.random(),
            "z": random.random(),
        },
        "altimeter": random.uniform(0, 40000),
        "barometric_pressure": random.uniform(900, 1100),
        "altitude": random.uniform(0, 40000),
        "gps_position": f"{random.uniform(-180, 180)}, {random.uniform(-90, 90)}",
    }


def save_data(data):
    TelemetryData.objects.create(
        exterior_temp=data["exterior_temp"],
        interior_temp=data["interior_temp"],
        accelerometer_data=data["accelerometer_data"],
        gyro_data=data["gyro_data"],
        magnetometer_data=data["magnetometer_data"],
        altimeter=data["altimeter"],
        barometric_pressure=data["barometric_pressure"],
        altitude=data["altitude"],
        gps_position=data["gps_position"],
    )


try:
    # Replace with actual serial port and baud rate
    ser = serial.Serial("/dev/ttyUSB0", 9600)
    while True:
        data = ser.readline()
        save_data(json.loads(data))
except (serial.SerialException, json.JSONDecodeError):
    while True:
        fake_data = generate_fake_data()
        save_data(fake_data)
        time.sleep(1)  # Simulate data every 1 second
