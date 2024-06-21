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
        self.written_data = []

    # Need to make getter and setter funcs bc proxy objs are spawned by the manager,
    #   so direct access is not possible
    def set(self, min_dict):
        for key, value in min_dict.items():
            self.time_series_data[key].append(value)
        # print(self.time_series_data)

    def get(self):
        return self.time_series_data

    def get_written(self):
        return self.written_data

    def add_written(self, tuple_data):
        self.written_data.append(tuple_data)


def read_serial_data(rocket, timeout=1):
    try:
        # Open the serial port
        ser = serial.Serial("COM11", 115200, timeout=timeout)

        print(f"Connected to COM11 at 115200 baudrate.")

        while True:
            if ser.in_waiting > 0:
                data = ser.readline().decode("utf-8").strip()
                labeled_data = {"Compressed Data": data}
                rocket.set(labeled_data)

            # Sleep for a short duration to avoid busy-waiting
            print(data)
            time.sleep(0.1)

    except serial.SerialException as e:
        print(f"Error: {e}")

    except KeyboardInterrupt:
        print("\nExiting...")


def write_to_csv(rocket):
    # Gather the data to be written to the CSV
    while True:
        data_to_write = {k: v for k, v in rocket.get().items()}

        if not data_to_write:
            return

        # Prepare the headers and rows for the CSV
        headers = data_to_write.keys()
        rows = zip(*data_to_write.values())

        with open(
            "compresseddata.csv",
            "a",
            newline="",
        ) as csvfile:
            csvwriter = csv.writer(csvfile)

            # Write headers if the file is empty
            if csvfile.tell() == 0:
                csvwriter.writerow(headers)

            for row in rows:
                # Convert the row to a tuple for hashable comparison
                row_tuple = tuple(row)
                if row_tuple not in rocket.get_written():
                    csvwriter.writerow(row)
                    rocket.add_written(row_tuple)


if __name__ == "__main__":
    BaseManager.register("Rocket", Rocket)
    manager = BaseManager()
    manager.start()
    inst = manager.Rocket()

    p1 = multiprocessing.Process(target=write_to_csv, args=[inst])
    p2 = multiprocessing.Process(target=read_serial_data, args=[inst])

    p2.start()
    time.sleep(3)
    p1.start()

    p2.join()
    p1.join()
