# Author(s):
#   Daniel Opara <hello@danielopara.me>

import dash
import random
from dash import Dash, html, dcc, callback_context, no_update
import dash_bootstrap_components as dbc
from dash_bootstrap_templates import load_figure_template
import dash_daq as daq
import dash_leaflet as dl
import plotly.graph_objects as go
from dash.dependencies import Input, Output, State


dbc_css = "https://cdn.jsdelivr.net/gh/AnnMarieW/dash-bootstrap-templates/dbc.min.css"
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP, dbc_css])
load_figure_template("morph")


# Initial states for switches
INITIAL_STATE = {
    "ground_state": True,
    "camera_state": True,
    "units": True,
    "coordinates": "N/A",
    "altitude": "N/A",
    "signal_quality": "N/A",
    "gps_fix": "N/A",
    "antenna_status": "N/A",
}

STARTING_COORDINATES = [
    37.7749,
    -122.4194,
]  # change this to the starting coordinate of the launch

coordinates = [STARTING_COORDINATES]
error_data = {"status": "OK", "errors": []}


# Fucntions to create components
#########################################################
def create_bool_switches(id, left_label, right_label, switch_state, button_label=""):
    switch_row = dbc.Row(
        [
            dbc.Col(
                html.Label(
                    left_label,
                ),
                width="auto",
            ),
            dbc.Col(
                daq.BooleanSwitch(
                    id=id,
                    on=switch_state,
                    label=button_label,
                    color="#00cc96",
                    persistence=True,
                    persisted_props=["on"],
                    labelPosition="bottom",
                ),
                width="auto",
            ),
            dbc.Col(
                html.Label(
                    right_label,
                ),
                width="auto",
            ),
        ],
        className="mb-3",
    )
    return switch_row


def create_gauge(gauge_id, label_text, value=5):
    fig = go.Figure(
        go.Indicator(
            mode="gauge+number",
            value=value,
            gauge={
                "shape": "angular",
                "axis": {"range": [0, 10]},
                "bar": {"color": "#FFA07A"},  # Use black to hide the bar
                "bgcolor": "#FFA07A",
                "threshold": {
                    "line": {"color": "#800020", "width": 6},
                    "thickness": 0.75,
                    "value": value,
                },
            },
            domain={"x": [0, 1], "y": [0, 1]},
        )
    )

    fig.update_layout(
        margin=dict(l=20, r=20, t=50, b=20),
        height=200,
    )

    return html.Div(
        [
            dcc.Graph(id=gauge_id, figure=fig, config={"staticPlot": True}),
            html.P(label_text, style={"text-align": "center", "margin-top": "5px"}),
        ]
    )


def create_gps_card():
    return dbc.Card(
        dbc.CardBody(
            [
                html.H4("GPS", className="card-title"),
                html.P("Coordinates: ", id="coordinates", className="card-text"),
                html.P("Altitude: ", id="altitude", className="card-text"),
                html.P("Signal Quality: ", id="signal_quality", className="card-text"),
                html.P("GPS Fix: ", id="gps_fix", className="card-text"),
                html.P("Antenna Status: ", id="antenna_status", className="card-text"),
            ]
        )
    )


# Actual layout objects
############################################
gauges = dbc.Container(
    [
        dbc.Row(
            [
                dbc.Col(
                    [
                        create_gauge("custom-gauge1", "Speed"),
                        create_gauge("custom-gauge2", "Temperature"),
                    ],
                    width=4,
                ),
                dbc.Col(
                    [
                        create_gauge("custom-gauge3", "Speed"),
                        create_gauge("custom-gauge4", "Temperature"),
                    ],
                    width=4,
                ),
                dbc.Col(
                    [
                        create_gauge("custom-gauge5", "Speed"),
                        create_gauge("custom-gauge6", "Temperature"),
                    ],
                    width=4,
                ),
            ]
        ),
    ],
)

state_and_switches = html.Div(
    children=[
        dcc.Store(id="state-store", storage_type="local", data=INITIAL_STATE),
        dbc.Container(
            children=[
                dbc.Stack(
                    [
                        dbc.Form(
                            style={
                                "display": "flex",
                                "flexDirection": "column",
                                "alignItems": "center",
                                "justifyContent": "center",
                                "width": "100%",
                            },
                            children=[
                                create_bool_switches(
                                    "ground-state-switch",
                                    "POW ON",
                                    "LNCH RDY",
                                    False,
                                ),
                                create_bool_switches(
                                    "camera-switch", "Camera Off", "Camera On", False
                                ),
                                create_bool_switches(
                                    "units-switch", "Metric", "Imperial", True
                                ),
                            ],
                        ),
                    ],
                )
            ],
        ),
    ],
)

rocket_map = html.Div(
    [
        dl.Map(
            [
                dl.TileLayer(),
                dl.FullScreenControl(),
                dl.GestureHandling(),
                dl.Polyline(id="path", positions=coordinates),
            ],
            center=STARTING_COORDINATES,
            zoom=5,
            style={"height": "50vh"},
            id="map",
        ),
        dcc.Interval(id="interval-component", interval=2000, n_intervals=0),
    ]
)

errorbox = dbc.Row(
    [
        dbc.Col(
            dbc.Card(
                [
                    dbc.CardHeader("System Status"),
                    dbc.CardBody(
                        [
                            html.Div(id="status-message"),
                            html.Ul(id="error-messages", style={"color": "red"}),
                            dcc.Interval(
                                id="interval-component",
                                interval=2000,  # Update every 2 seconds
                                n_intervals=0,
                            ),
                        ]
                    ),
                ]
            ),
            width=6,
        ),
    ],
    justify="center",
    style={"marginTop": "50px"},
)

app.layout = dbc.Container(
    [
        dbc.Row(
            [
                dbc.Col(state_and_switches),
                dbc.Col(
                    dbc.Container(
                        [create_gps_card()],
                        fluid=True,
                    )
                ),
                dbc.Col(gauges),
            ],
            className="g-0",
        ),
        dbc.Row(
            dbc.Stack(
                [
                    dbc.Col(rocket_map, width=6),
                    errorbox,
                ]
            )
        ),
        dcc.Interval(
            id="interval-component",
            interval=2000,  # Update every 2 seconds
            n_intervals=0,
        ),
    ],
    fluid=True,
)


# Handling switch changes
@app.callback(
    [
        Output("ground-state-switch", "on"),
        Output("camera-switch", "on"),
        Output("units-switch", "on"),
    ],
    [Input("state-store", "data")],
)
def load_initial_state(state_data):
    return state_data["ground_state"], state_data["camera_state"], state_data["units"]


@app.callback(
    Output("state-store", "data"),
    [
        Input("ground-state-switch", "on"),
        Input("camera-switch", "on"),
        Input("units-switch", "on"),
    ],
    State("state-store", "data"),
)
def update_state_store(ground_state, camera_state, units_state, state_data):
    ctx = callback_context

    if not ctx.triggered:
        return no_update

    triggered_id = ctx.triggered[0]["prop_id"].split(".")[0]

    if triggered_id == "ground-state-switch":
        state_data["ground_state"] = ground_state
    elif triggered_id == "camera-switch":
        state_data["camera_state"] = camera_state
    elif triggered_id == "units-switch":
        state_data["units"] = units_state

    return state_data


# Create a callback to update the gauges
@app.callback(
    [
        Output("custom-gauge1", "figure"),
        Output("custom-gauge2", "figure"),
        Output("custom-gauge3", "figure"),
        Output("custom-gauge4", "figure"),
        Output("custom-gauge5", "figure"),
        Output("custom-gauge6", "figure"),
    ],
    [Input("interval-component", "n_intervals")],
)
def update_gauges(n):
    # Simulate data update, replace with actual data retrieval
    new_values = [random.uniform(0, 10) for _ in range(6)]
    gauges = []
    for value in new_values:
        fig = go.Figure(
            go.Indicator(
                mode="gauge+number",
                value=value,
                gauge={
                    "shape": "angular",
                    "axis": {"range": [0, 10]},
                    "bar": {"color": "#FFA07A"},  # Use black to hide the bar
                    "bgcolor": "#FFA07A",
                    "threshold": {
                        "line": {"color": "#800020", "width": 4},
                        "thickness": 0.75,
                        "value": value,
                    },
                },
                domain={"x": [0, 1], "y": [0, 1]},
            )
        )

        fig.update_layout(
            margin=dict(l=20, r=20, t=50, b=20),
            height=200,
        )
        gauges.append(fig)

    return gauges


@app.callback(
    [
        Output("coordinates", "children"),
        Output("altitude", "children"),
        Output("signal_quality", "children"),
        Output("gps_fix", "children"),
        Output("antenna_status", "children"),
    ],
    [Input("interval-component", "n_intervals")],
)
def update_gps_status(n):
    # Simulate data update, replace with actual data retrieval
    new_values = {
        "coordinates": f"Lat: {random.uniform(-90, 90):.2f}, Lon: {random.uniform(-180, 180):.2f}",
        "altitude": f"{random.uniform(0, 10000):.2f} m",
        "signal_quality": f"{random.randint(0, 100)}%",
        "gps_fix": "Yes" if random.choice([True, False]) else "No",
        "antenna_status": (
            "Connected" if random.choice([True, False]) else "Disconnected"
        ),
    }

    return (
        f"Coordinates: {new_values['coordinates']}",
        f"Altitude: {new_values['altitude']}",
        f"Signal Quality: {new_values['signal_quality']}",
        f"GPS Fix: {new_values['gps_fix']}",
        f"Antenna Status: {new_values['antenna_status']}",
    )


## replace this with how we grab gps data
def generate_new_position(last_position):
    new_lat = last_position[0] + random.uniform(-0.5, 0.5)
    new_lon = last_position[1] + random.uniform(-0.5, 0.5)
    return [new_lat, new_lon]


@app.callback(
    Output("path", "positions"),
    Input("interval-component", "n_intervals"),
    State("path", "positions"),
)
def update_path(n, current_positions):
    # simulate receiving new coordinates
    new_position = generate_new_position(current_positions[-1])
    current_positions.append(new_position)
    return current_positions


@app.callback(
    [Output("status-message", "children"), Output("error-messages", "children")],
    [Input("interval-component", "n_intervals")],
)
def update_card(n_intervals):
    # simulate receiving data and updating status, this is gonna need a lot of changes
    if n_intervals % 5 == 0:  # Simulate an error every 5 intervals
        error_data["status"] = "Error"
        error_data["errors"].append("System overheating!")
    else:
        error_data["status"] = "OK"
        error_data["errors"].clear()

    status_message = f"Status: {error_data['status']}"
    error_messages = [html.Li(error) for error in error_data["errors"]]

    return status_message, error_messages


if __name__ == "__main__":
    app.run_server(debug=True)
