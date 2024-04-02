import multiprocessing
import time
import pdb
from queue import Empty
import dash
from dash import dcc, html, Input, Output, callback
import plotly.graph_objs as go


def dashboard(q):
    external_stylesheets = ["https://codepen.io/chriddyp/pen/bWLwgP.css"]

    app = dash.Dash(__name__, external_stylesheets=external_stylesheets)

    app.layout = html.Div(
        html.Div(
            [
                html.H4("Rocket Launch Simulation"),
                dcc.Graph(id="live-update-graph"),
                dcc.Interval(
                    id="interval-component",
                    interval=1 * 1000,  # in milliseconds
                    n_intervals=0,
                ),
            ]
        )
    )

    @app.callback(
        Output("live-update-graph", "figure"),
        Input("interval-component", "n_intervals"),
    )
    def update_graph_live(q):
        try:
            new_data = q.get(timeout=0.1)
            d["Time"].append(new_data["Time"])
            d["Altitude"].append(new_data["Altitude"])
            d["Velocity"].append(new_data["Velocity"])
        except Empty:
            pass

        fig = go.Figure()
        fig.add_trace(
            go.Scatter(
                x=d["Time"],
                y=d["Altitude"],
                mode="lines+markers",
                name="Altitude",
                hoverinfo="none",
            )
        )
        fig.add_trace(
            go.Scatter(
                x=d["Time"],
                y=d["Velocity"],
                mode="lines+markers",
                name="Velocity",
                hoverinfo="none",
            )
        )

        fig.update_layout(
            title_text="Rocket Altitude and Velocity",
            xaxis_title="Time",
            yaxis_title="Value",
            title_x=0.5,
            height=600,
            showlegend=True,
            hovermode="closest",
        )

    app.run_server(debug=True)


def add_values(q):
    def update_state(elapsed_time):
        if elapsed_time <= 5:
            velocity = 50 * elapsed_time
            altitude = 0.5 * 9.8 * elapsed_time**2
        else:
            time_since_burnout = elapsed_time - 5
            velocity = 50 * 5 - 9.8 * time_since_burnout
            altitude = 0.5 * 9.8 * 5**2 - 0.5 * 9.8 * time_since_burnout**2

            if altitude < 0:
                altitude = 0
                velocity = 0
        return altitude, velocity

    start_time = time.time()
    while time.time() - start_time <= 15:
        elapsed_time = time.time() - start_time
        new_alt, new_velo = update_state(elapsed_time)
        new_data = {
            "Time": elapsed_time,
            "Altitude": new_alt,
            "Velocity": new_velo,
        }
        for key, value in new_data.items():
            q[key].append(value)
            print("did it")
        time.sleep(0.1)  # simulating one-second interval


if __name__ == "__main__":
    manager = multiprocessing.Manager()
    d = manager.dict()
    d["Time"] = []
    d["Altitude"] = []
    d["Velocity"] = []

    p1 = multiprocessing.Process(target=add_values, args=(d,))
    p2 = multiprocessing.Process(target=dashboard, args=(d,))

    p1.start()
    p2.start()

    p1.join()
    p2.join()
