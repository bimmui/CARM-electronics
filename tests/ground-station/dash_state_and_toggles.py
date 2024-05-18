from dash import Dash, html, dcc
import dash_bootstrap_components as dbc
import dash_daq as daq
from dash.dependencies import Input, Output

app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP])

# Another way to think of this:
# ground_state - True if launch ready, false if not (power on state)
# camera_state - True if on, false if not
# units        - True if they are stupid (imperial), false if not
STATE = {"ground_state": True, "camera_state": True, "units": True}


app.layout = html.Div(
    style={"backgroundColor": "#333"},  # Background for the entire app
    children=[
        dcc.Store(
            id="state-store", data={"ready": True, "camera": True, "units": True}
        ),
        dbc.Container(
            style={
                "backgroundColor": "transparent",
                "padding": "20px",
                "border": "2px solid red",
                "width": "300px",
            },
            children=[
                html.H4(
                    "Current State", style={"color": "white", "textAlign": "center"}
                ),
                dbc.Form(
                    [
                        dbc.Row(
                            [
                                dbc.Col(
                                    html.Label(
                                        "Y",
                                        style={"color": "white", "marginRight": "10px"},
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    daq.BooleanSwitch(
                                        id="ready-switch", on=True, color="#00cc96"
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    html.Label(
                                        "N",
                                        style={"color": "white", "marginLeft": "10px"},
                                    ),
                                    width="auto",
                                ),
                            ],
                            className="mb-3",
                            align="center",
                        ),
                        dbc.Row(
                            [
                                dbc.Col(
                                    html.Label(
                                        "On",
                                        style={"color": "white", "marginRight": "10px"},
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    daq.BooleanSwitch(
                                        id="camera-switch", on=True, color="#00cc96"
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    html.Label(
                                        "Off",
                                        style={"color": "white", "marginLeft": "10px"},
                                    ),
                                    width="auto",
                                ),
                            ],
                            className="mb-3",
                            align="center",
                        ),
                        dbc.Row(
                            [
                                dbc.Col(
                                    html.Label(
                                        "M",
                                        style={"color": "white", "marginRight": "10px"},
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    daq.BooleanSwitch(
                                        id="units-switch", on=True, color="#00cc96"
                                    ),
                                    width="auto",
                                ),
                                dbc.Col(
                                    html.Label(
                                        "I",
                                        style={"color": "white", "marginLeft": "10px"},
                                    ),
                                    width="auto",
                                ),
                            ],
                            className="mb-3",
                            align="center",
                        ),
                    ]
                ),
            ],
        ),
    ],
)


@app.callback(
    Output("ready-switch", "on"),
    Output("camera-switch", "on"),
    Output("units-switch", "on"),
    Input("ready-switch", "on"),
    Input("camera-switch", "on"),
    Input("units-switch", "on"),
)
def update_output(ready, camera, units):
    STATE["ground_state"] = ready
    STATE["camera_state"] = camera
    STATE["units"] = units
    return ready, camera, units


if __name__ == "__main__":
    app.run_server(debug=True)
