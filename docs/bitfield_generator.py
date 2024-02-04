# Author(s):
#   Daniel Opara <hello@danielopara.me>

"""This script generates the bitfield schema seen in docs\\diagrams\\bitfield_schema.png.
It utilizes the Python port of the bitfield module developed by Arth-ur.

For information on how to produce a similar diagram, visit https://github.com/Arth-ur/bitfield for the Python port
and https://github.com/wavedrom/bitfield for the original module written in js.
"""

import cairosvg
import json
from PIL import Image
from io import BytesIO
from bit_field import render, jsonml_stringify

# note: typeColor() in bit_field was modified to add more colors. To acheive the same affect,
# edit typeColor(t) and change the "styles" dict to the following:
"""
styles = {
    '2': 0,
    '3': 80,
    '4': 170,
    '5': 45,
    '6': 126,
    '7': 215,
    '8': 20,
    '9': 275,
    '10': 200,
    '11': 250,
    '12': 1000
}
"""


def read_schema_json():
    filename = input("Enter the name of the JSON file (without extension): ")
    data = None
    path = f"bitfield-schema-source\\{filename}.json"
    try:
        with open(path, "r") as file:
            lines = file.readlines()
            # Ignore lines starting with #
            filtered_lines = [line for line in lines if not line.startswith("#")]
            # Join the lines to form a valid JSON string
            json_data = "".join(filtered_lines)
            # Load the JSON data
            data = json.loads(json_data)
    except FileNotFoundError:
        print(f"File '{filename}.json' not found.")
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}")
    return data[0], data[1], filename


# Register description in JSON format
# Tweak the options in the json file based on the schema you are generating
bit_organization, options, schema_name = read_schema_json()

# Convert the list of dictionaries to a single dictionary
# i used gpt for this shit i couldnt bro
options = {
    key: entry[key] if not isinstance(entry[key], bool) else str(entry[key]).lower()
    for entry in options
    for key in entry
}

# Render the bit field diagram
jsonml = render(bit_organization, **options)

# Convert the JSONML to an HTML string
html = jsonml_stringify(jsonml)

# Convert SVG to PNG using cairosvg
png_data = cairosvg.svg2png(bytestring=html)
png_image = Image.open(BytesIO(png_data))

# Save the PNG file
png_file_path = f"diagrams\\{schema_name}.png"
png_image.save(png_file_path, "PNG")
