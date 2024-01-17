# Author(s):
#   Daniel Opara <hello@danielopara.me>

"""This script generates the bitfield schema seen in docs\\diagrams\\bitfield_diagram.png.
It utilizes the Python port of the bitfield module developed by Arth-ur.

For information on how to produce a similar diagram, visit https://github.com/Arth-ur/bitfield for the Python port
and https://github.com/wavedrom/bitfield for the original module written in js.
"""

import cairosvg
from PIL import Image
from io import BytesIO
from bit_field import render, jsonml_stringify

# note: typeColor() in bit_field was modified to add more colors. To acheive the same affect,
# edit typeColor(t) and change the "styles" dict to the following:
"""styles = {
        '2': 0,
        '3': 80,
        '4': 170,
        '5': 45,
        '6': 126,
        '7': 215,
        '8': 20,
        '9': 420,
    }"""


# Register description in JSON format
bit_organization = [
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "TIME", "bits": 8, "type": 9},
    {"name": "INT TEMP ENGINE", "bits": 12, "type": 2},
    {"name": "ND", "bits": 9},
    ################################################################
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "GPS LAT_SIGN", "bits": 1, "type": 5, "rotate": -45},
    {"name": "GPS LAT", "bits": 27, "type": 5},
    {"name": "ND", "bits": 1},
    ################################################################
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "GPS LONG_SIGN", "bits": 1, "type": 5, "rotate": -45},
    {"name": "GPS LONG", "bits": 28, "type": 5},
    ################################################################
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "GYRO_SIGN", "bits": 1, "type": 10, "rotate": -45},
    {"name": "GYRO", "bits": 9, "type": 10},
    {"name": "ACCEL_DEC", "bits": 2, "type": 8, "rotate": -45},
    {"name": "ACCEL_SIGN", "bits": 1, "type": 8, "rotate": -45},
    {"name": "ACCEL", "bits": 16, "type": 8},
    ################################################################
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "ALT_DEC", "bits": 1, "type": 7, "rotate": -45},
    {"name": "ALT", "bits": 16, "type": 7},
    {"name": "INT TEMP AVBAY", "bits": 12, "type": 2},
    ################################################################
    {"name": "FRAG NUM", "bits": 3, "type": 4},
    {"name": "BAR PR_DEC", "bits": 1, "type": 6, "rotate": -45},
    {"name": "BAR PR", "bits": 14, "type": 6},
    {"name": "EXT TEMP_DEC", "bits": 1, "type": 2, "rotate": -45},
    {"name": "EXT TEMP_SIGN", "bits": 1, "type": 2, "rotate": -45},
    {"name": "EXT TEMP", "bits": 12, "type": 2},
    ################################################################
]

options = {
    "vspace": 100,
    "hspace": 1400,
    "lanes": 6,
    "bits": 192,
    "hflip": True,
    "fontsize": 13,
}


# Render the bit field diagram
jsonml = render(bit_organization, **options)

# Convert the JSONML to an HTML string
html = jsonml_stringify(jsonml)

# Convert SVG to PNG using cairosvg
png_data = cairosvg.svg2png(bytestring=html)
png_image = Image.open(BytesIO(png_data))

# Save the PNG file
png_file_path = "diagrams\\bitfield_schema.png"
png_image.save(png_file_path, "PNG")
