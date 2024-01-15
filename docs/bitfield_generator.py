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

# Register description in JSON format
bit_organization = [
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "GPS LAT", "bits": 28, "type": 5},
    {"name": "ND", "bits": 1},
    ################################################################
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "GPS LONG", "bits": 29, "type": 5},
    ################################################################
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "ALT", "bits": 16, "type": 7},
    {"name": "EXT TEMP", "bits": 13, "type": 2},
    ################################################################
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "ACCEL", "bits": 16, "type": 8},
    {"name": "INT TEMP AVBAY", "bits": 12, "type": 2},
    {"name": "ND", "bits": 1},
    ################################################################
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "BAR PR", "bits": 16, "type": 6},
    {"name": "INT TEMP ENGINE", "bits": 12, "type": 2},
    {"name": "ND", "bits": 1},
    ################################################################
    {"name": "Frag Num", "bits": 3, "type": 4},
    {"name": "GYRO", "bits": 14, "type": 10},
    {"name": "TIME", "bits": 12, "type": 9},
    {"name": "ND", "bits": 3},
    ################################################################
]

options = {"hspace": 888, "lanes": 6, "bits": 192, "vflip": True, "hflip": True}


# Render the bit field diagram
jsonml = render(bit_organization, **options)

# Convert the JSONML to an HTML string
html = jsonml_stringify(jsonml)

# Convert SVG to PNG using cairosvg
png_data = cairosvg.svg2png(bytestring=html)
png_image = Image.open(BytesIO(png_data))

# Save the PNG file
png_file_path = "diagrams\\bitfield_diagram.png"
png_image.save(png_file_path, "PNG")
