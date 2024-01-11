import cairosvg
from PIL import Image
from io import BytesIO
from bit_field import render, jsonml_stringify

# Register description in JSON format
bit_organization = [
    {"name": "Frag #", "bits": 2, "type": 4},
    {"name": "Time (s)", "bits": 9, "type": 5},
    {"name": "Internal Temp Sensor", "bits": 7, "type": 6},
    {"name": "External Temp Sensor", "bits": 7, "type": 6},
    {"name": "Barometric Pressure", "bits": 7, "type": 7},
    ################################################################
    {"name": "Frag #", "bits": 2, "type": 4},
    {"name": "Altitude", "bits": 12, "type": 5},
    {"name": "Acceleration", "bits": 7, "type": 6},
    {"name": "Gyro measurements", "bits": 7, "type": 7},
]

options = {"hspace": 888, "lanes": 3, "bits": 64}

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
