
/*
 * setupSensorIMU
 * Parameters: Reference (pointer) to the instantiated IMU sensor object
 * Purpose: Sets up the range at which the sensor's individual components measure
 *           higher ranges = less precision but can measure larger movements
 * Returns: Nothing
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout/overview
 */
void setupSensorIMU(Adafruit_LSM9DS1 &lsm_obj)
{
    Serial.println("Setting up IMU...");
    if (!lsm_obj.begin())
    {
        Serial.println("Failed to communicate with LSM9DS1.");
        while (1)
            ;
    }
    lsm_obj.setupAccel(lsm_obj.LSM9DS1_ACCELRANGE_2G);
    lsm_obj.setupMag(lsm_obj.LSM9DS1_MAGGAIN_4GAUSS);
    lsm_obj.setupGyro(lsm_obj.LSM9DS1_GYROSCALE_245DPS);
    Serial.println("Complete!");
}

/*
 * setupSensorBMP
 * Parameters: Reference (pointer) to the instantiated barometric pressure sensor object
 * Purpose: Configures various settings of the BMP3 sensor to control the oversampling of temperature and pressure,
 *          set the IIR (Infinite Impulse Response) filter coefficient, and define the output data rate
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-bmp388-bmp390-bmp3xx/overview
 */
void setupSensorBMP(Adafruit_BMP3XX bmp_obj)
{
    Serial.println("Setting up barometric pressure sensor...");
    if (!bmp_obj.begin_I2C())
    {
        Serial.println("Could not find a valid BMP3 sensor, check wiring!");
        while (1)
            ;
    }

    // Set up oversampling and filter initialization
    bmp_obj.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp_obj.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp_obj.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp_obj.setOutputDataRate(BMP3_ODR_50_HZ);
    Serial.println("Complete!");
}

/*
 * setupSensorTemp
 * Parameters: Reference (pointer) to the instantiated temp sensor object
 * Purpose: Connects the sensor to its address and sets the precision of measurements
 * Returns: Nothing
 * Notes: The sensor was set up in I2C. The resolution of measurements impacts the time it takes to collect sample data,
 *            recommended to check out test/feather_scripts and view the comments on the example if you want to better understand how it works.
 *            For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-mcp9808-precision-i2c-temperature-sensor-guide/overview
 */
void setupSensorTemp(Adafruit_MCP9808 &tempsensor_obj)
{
    Serial.println("Setting up temperature sensor...");
    if (!tempsensor_obj.begin(0x18))
    {
        Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
        while (1)
            ;
    }
    tempsensor_obj.setResolution(3);
    Serial.println("Complete!");
}