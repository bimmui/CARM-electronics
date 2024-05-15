#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "altitude.hpp"
#include "StateDetermination.hpp"

struct SensorData
{
    uint32_t time;
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float temp, bmp_temp, pressure, altitude;
};

int main()
{
    std::ifstream file("shifted_time_alt.csv");
    std::string line;
    std::vector<SensorData> sensorData;
    StateDeterminer state_determiner = StateDeterminer();

    if (file.is_open())
    {
        // Skip the header line
        std::getline(file, line);

        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string token;

            SensorData data;
            std::getline(ss, token, ',');
            data.time = std::stoll(token);
            std::getline(ss, token, ',');
            data.ax = std::stof(token);
            std::getline(ss, token, ',');
            data.ay = std::stof(token);
            std::getline(ss, token, ',');
            data.az = std::stof(token);
            std::getline(ss, token, ',');
            data.gx = std::stof(token);
            std::getline(ss, token, ',');
            data.gy = std::stof(token);
            std::getline(ss, token, ',');
            data.gz = std::stof(token);
            std::getline(ss, token, ',');
            data.temp = std::stof(token);
            std::getline(ss, token, ',');
            data.mx = std::stof(token);
            std::getline(ss, token, ',');
            data.my = std::stof(token);
            std::getline(ss, token, ',');
            data.mz = std::stof(token);
            std::getline(ss, token, ',');
            data.bmp_temp = std::stof(token);
            std::getline(ss, token, ',');
            data.pressure = std::stof(token);
            std::getline(ss, token);
            data.altitude = std::stof(token);

            sensorData.push_back(data);
        }

        file.close();

        std::ofstream outFile("output.csv");
        if (outFile.is_open())
        {
            // Write the header
            outFile << "Time (ms),Altitude,Vertical Velocity,Vertical Acceleration,State\n";

            // Use sensorData vector here
            std::cout << "Starting to write to file" << std::endl;
            for (const auto &data : sensorData)
            {
                float accel_data[3] = {float(data.ax / 9.81), float(data.ay / 9.81), float(data.az / 9.81)};
                float gyro_data[3] = {data.gx, data.gy, data.gz};
                state_determiner.determineState(accel_data, gyro_data, data.altitude, data.time, outFile);
            }

            outFile.close();
            std::cout << "Finished write to file" << std::endl;
        }
        else
        {
            std::cerr << "Unable to open output file!" << std::endl;
        }
    }
    else
    {
        std::cerr << "Unable to open file!" << std::endl;
    }

    return 0;
}
