import multiprocessing
from pprint import pprint
from multiprocessing.managers import BaseManager
import time
import pdb
from queue import Empty
import dash
from dash import dcc, html, Input, Output, callback
import plotly.graph_objs as go


class DummyRocket:
    """Creates objects that simulates the velocity and altitude of a rocket when launched.

    The rocket simulation is very rough but follows that of
    a generic parabolic curve given the time (x-value) the rocket should apogee.
    The ``duration`` parameter of ``launch()`` should be changed to account for any change in
    the burnout_time and apogee_time, but it isn't necessary.

    TODO: Add more attributes to the rocket that would be transmitted down to the ground station
    """

    def __init__(self):
        self.time_series_data = {"Time": [], "Altitude": [], "Velocity": []}

    # Need to make getter and setter funcs bc proxy objs are spawned by the manager,
    #   so direct access is not possible
    def set(self, min_dict):
        for key, value in min_dict.items():
            self.time_series_data[key].append(value)
        # print(self.time_series_data)

    def get(self):
        return self.time_series_data


def dashboard(rocket):
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

    rocketo_proxy = rocket

    @app.callback(
        Output("live-update-graph", "figure"),
        Input("interval-component", "n_intervals"),
    )
    def update_graph_live(n):
        data = rocketo_proxy.get()

        fig = go.Figure()
        fig.add_trace(
            go.Scatter(
                x=data["Time"],
                y=data["Altitude"],
                mode="lines+markers",
                name="Altitude",
                hoverinfo="none",
            )
        )
        fig.add_trace(
            go.Scatter(
                x=data["Time"],
                y=data["Velocity"],
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

        return fig

    app.run_server(debug=True, port=8055)


def add_values(rocket):
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

    # pprint(vars(rocket))
    start_time = time.time()
    while time.time() - start_time <= 15:
        elapsed_time = time.time() - start_time
        new_alt, new_velo = update_state(elapsed_time)
        new_data = {
            "Time": elapsed_time,
            "Altitude": new_alt,
            "Velocity": new_velo,
        }
        rocket.set(new_data)
        time.sleep(0.1)  # simulating one-second interval


if __name__ == "__main__":
    BaseManager.register("DummyRocket", DummyRocket)
    manager = BaseManager()
    manager.start()
    inst = manager.DummyRocket()

    p1 = multiprocessing.Process(target=add_values, args=[inst])
    time.sleep(3)
    p2 = multiprocessing.Process(target=dashboard, args=[inst])

    p1.start()
    p2.start()

    p1.join()
    p2.join()
