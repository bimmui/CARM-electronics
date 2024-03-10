import plotly.graph_objs as go

# Fetch GPS data from the database
cursor.execute("SELECT gps_lat, gps_long, gps_altitude FROM sensor_data")
gps_data = cursor.fetchall()

# Create a 3D scatter plot
rocket_path = go.Scatter3d(
    x=[d[1] for d in gps_data],
    y=[d[0] for d in gps_data],
    z=[d[2] for d in gps_data],
    mode='lines'
)

layout = go.Layout(
    scene=dict(
        xaxis=dict(title='Longitude'),
        yaxis=dict(title='Latitude'),
        zaxis=dict(title='Altitude')
    ),
    width=800,
    height=600,
    margin=dict(r=20, l=20, b=20, t=20)
)

fig = go.Figure(data=[rocket_path], layout=layout)
fig.show()