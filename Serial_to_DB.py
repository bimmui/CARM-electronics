import serial
import json
from django.core.wsgi import get_wsgi_application
import os

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "carm_electronics.settings")
application = get_wsgi_application()

from telemetry.models import TelemetryData

ser = serial.Serial("/dev/ttyUSB0", 9600)  # Update the serial port

while True:
    data = ser.readline()
    try:
        parsed_data = json.loads(data)
        TelemetryData.objects.create(
            exterior_temp=parsed_data["exterior_temp"],
            interior_temp=parsed_data["interior_temp"],
            # ... and so on for the other fields
        )
    except json.JSONDecodeError:
        continue
