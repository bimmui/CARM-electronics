from django.db import models


class TelemetryData(models.Model):
    # time series data
    time_stamp = models.DateTimeField(auto_now_add=True)

    # temperature
    temp_sensor_exterior_temp = models.DecimalField(max_digits=6, decimal_places=3)
    temp_sensor_interior_temp = models.DecimalField(max_digits=6, decimal_places=3)
    barometric_sensor_interior_temp = models.DecimalField(
        max_digits=6, decimal_places=3
    )

    #
    accelerometer_data = models.DecimalField()
    gyro_data = models.DecimalField()
    magnetometer_data = models.DecimalField()
    barometric_pressure = models.DecimalField(max_digits=6, decimal_places=3)
    altitude = models.DecimalField(max_digits=10, decimal_places=3)
    gps_position = models.CharField(max_length=100)
