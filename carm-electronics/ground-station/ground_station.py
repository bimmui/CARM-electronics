import multiprocessing
from pprint import pprint
from multiprocessing.managers import BaseManager
import sys
import time
import datetime
import pdb
from queue import Empty
import serial
import csv


class Rocket:

    def __init__(self):
        self.time_series_data = {"Compressed Data": []}

    # Need to make getter and setter funcs bc proxy objs are spawned by the manager,
    #   so direct access is not possible
    def set(self, min_dict):
        for key, value in min_dict.items():
            self.time_series_data[key].append(value)
        # print(self.time_series_data)

    def get(self):
        return self.time_series_data


def read_serial_data(rocket, port, baudrate, timeout=1):
    try:
        # Open the serial port
        ser = serial.Serial(port, baudrate, timeout=timeout)

        print(f"Connected to {port} at {baudrate} baudrate.")

        while True:
            if ser.in_waiting > 0:
                data = ser.readline().decode("utf-8").strip().split(",")
                print(f"Received data: {data}")
                labeled_data = {}
                rocket.set(labeled_data)

            # Sleep for a short duration to avoid busy-waiting
            time.sleep(0.1)

    except serial.SerialException as e:
        print(f"Error: {e}")

    except KeyboardInterrupt:
        print("\nExiting...")

    finally:
        ser.close()
        print("Serial port closed.")


def write_to_csv(rocket, filename):
    # Gather the data to be written to the CSV
    while True:
        data_to_write = {
            k: v
            for k, v in rocket.time_series_data.items()
            if isinstance(v, list) and v
        }

        if not data_to_write:
            return

        # Prepare the headers and rows for the CSV
        headers = data_to_write.keys()
        rows = zip(*data_to_write.values())

        with open(filename, "a", newline="") as csvfile:
            csvwriter = csv.writer(csvfile)

            # Write headers if the file is empty
            if csvfile.tell() == 0:
                csvwriter.writerow(headers)

            for row in rows:
                # Convert the row to a tuple for hashable comparison
                row_tuple = tuple(row)
                if row_tuple not in rocket.written_data:
                    csvwriter.writerow(row)
                    rocket.written_data.add(row_tuple)


if __name__ == "__main__":
    BaseManager.register("Rocket", Rocket)
    manager = BaseManager()
    manager.start()
    inst = manager.Rocket()

    p1 = multiprocessing.Process(target=write_to_csv, args=[inst])
    p2 = multiprocessing.Process(target=dashboard, args=[inst])
    p3 = multiprocessing.Process(target=populate_infludb, args=[inst])

    p1.start()
    time.sleep(3)
    p2.start()
    p3.start()

    p1.join()
    p2.join()
    p3.join()
