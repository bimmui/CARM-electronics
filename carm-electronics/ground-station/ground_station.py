import sqlite3
import plotly.graph_objs as go
from plotly.subplots import make_subplots

conn = sqlite3.connect('rocket_data.db')
cursor = conn.cursor()

# Fetch data from SQL database
cursor.execute("SELECT * FROM sensor_data")
data = cursor.fetchall()

# Create Plotly figures
fig = make_subplots(rows=2, cols=2, specs=[[{"type": "xy"}, {"type": "xy"}],
                                           [{"type": "xy"}, {"type": "polar"}]])

# Altitude plot
fig.add_trace(go.Scatter(x=[d[0] for d in data], y=[d[6] for d in data], mode='lines', name='Altitude'), row=1, col=1)
fig.update_xaxes(title_text="Time (s)", row=1, col=1)
fig.update_yaxes(title_text="Altitude (m)", row=1, col=1)

# Velocity plot
fig.add_trace(go.Scatter(x=[d[0] for d in data], y=[d[16] for d in data], mode='lines', name='Velocity'), row=1, col=2)
fig.update_xaxes(title_text="Time (s)", row=1, col=2)
fig.update_yaxes(title_text="Velocity (m/s)", row=1, col=2)

# Acceleration plot
fig.add_trace(go.Scatter(x=[d[0] for d in data], y=[d[10] for d in data], mode='lines', name='Acceleration'), row=2, col=1)
fig.update_xaxes(title_text="Time (s)", row=2, col=1)
fig.update_yaxes(title_text="Acceleration (m/s²)", row=2, col=1)

# Temperature gauge
fig.add_trace(go.Indicator(mode="gauge+number",
                            value=data[-1][1],
                            title={'text': "Temperature (°C)"},
                            gauge={'axis': {'range': [0, 100]}}), row=2, col=2)

# Add minimap based on GPS data
minimap = go.Scattermapbox(lat=[d[17] for d in data],
                           lon=[d[18] for d in data],
                           mode='lines')
fig.add_trace(minimap, row=1, col=2)

# Add rocket image
rocket_image = go.layout.Image(source="https://example.com/rocket.png", xref="paper", yref="paper",
                               x=0.5, y=0.5, sizex=0.5, sizey=0.5, sizing="stretch", opacity=1, layer="above")
fig.add_layout_image(rocket_image)

# Add labels and annotations
fig.add_annotation(x=0.05, y=0.95, text=f"Current State: {data[-1][-1]}", showarrow=False, xref='paper', yref='paper')
fig.add_annotation(x=0.05, y=0.9, text=f"Altitude: {data[-1][6]} m", showarrow=False, xref='paper', yref='paper')
fig.add_annotation(x=0.05, y=0.85, text=f"Temperature: {data[-1][1]} °C", showarrow=False, xref='paper', yref='paper')
fig.add_annotation(x=0.05, y=0.8, text=f"Velocity: {data[-1][16]} m/s", showarrow=False, xref='paper', yref='paper')
fig.add_annotation(x=0.05, y=0.75, text=f"Acceleration: {data[-1][10]} m/s²", showarrow=False, xref='paper', yref='paper')
fig.add_annotation(x=0.05, y=0.7, text=f"GPS: {data[-1][17]}, {data[-1][18]}", showarrow=False, xref='paper', yref='paper')

fig.update_layout(title_text="Rocket Telemetry", height=800, width=1200)
fig.show()

conn.close()