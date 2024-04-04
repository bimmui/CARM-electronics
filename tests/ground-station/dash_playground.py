import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import dash_bootstrap_components as dbc
import dash_daq as daq

app = dash.Dash(external_stylesheets=[dbc.themes.BOOTSTRAP])


gauge = daq.Gauge(
    size=100,
    showCurrentValue=True,
    color={
        "gradient": True,
        "ranges": {"green": [0, 6], "yellow": [6, 8], "red": [8, 10]},
    },
    units="MPH",
    value=5,
    label="Default",
    max=10,
    min=0,
)
row = html.Div(
    [
        dbc.Row(dbc.Col(html.Div("A single column"))),
        dbc.Row(
            [
                dbc.Col(
                    [
                        html.Div("One of three columns"),
                        dcc.Graph(id="graph1"),
                        dcc.Graph(id="graph2"),
                        dcc.Graph(id="graph3"),
                    ]
                ),
                dbc.Col(
                    [
                        html.Div("One of three columns"),
                        dbc.Col(
                            [
                                html.Div(
                                    [
                                        dbc.Row(
                                            gauge,
                                            class_name="gauge1",
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                        dbc.Row(
                                            gauge,
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                        dbc.Row(
                                            gauge,
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                    ],
                                    id="gauge-row-1",
                                ),
                                html.Div(
                                    [
                                        dbc.Row(
                                            gauge,
                                            class_name="gauge1",
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                        dbc.Row(
                                            gauge,
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                        dbc.Row(
                                            gauge,
                                            style={
                                                "width": "33%",
                                                "display": "inline-block",
                                            },
                                        ),
                                    ],
                                    id="gauge-row-2",
                                ),
                            ]
                        ),
                    ]
                ),
                dbc.Col(html.Div("One of three columns")),
            ]
        ),
    ]
)
# Create a graph that updates based on the dropdown selection
graph = dcc.Graph(id="graph")

# Layout the components
app.layout = dbc.Container(
    [
        html.H1("Simple Dashboard Example"),
        row,
    ],
    fluid=True,
)


# Define callback to update the graph based on the dropdown selection
# @app.callback(Output("graph", "figure"), [Input("city-dropdown", "value")])
# def update_graph(selected_city):
#     # In a real application, you would use the selected_city value to update the graph data
#     return {
#         "data": [{"x": [1, 2, 3], "y": [4, 1, 2], "type": "bar", "name": "City"}],
#         "layout": {"title": f"Graph for {selected_city}"},
#     }


if __name__ == "__main__":
    app.run_server(debug=True, port=8060)
