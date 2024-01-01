# app.py

import dash
from dash import dcc, html, Input, Output, callback
import plotly.graph_objs as go
from dummy_factory import DummyRocket

external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']

app = dash.Dash(__name__, external_stylesheets=external_stylesheets)
rocket = DummyRocket()

app.layout = html.Div(
    html.Div([
        html.H4('Rocket Launch Simulation'),
        html.Div(id='live-update-text'),
        dcc.Graph(id='live-update-graph'),
        dcc.Interval(
            id='interval-component',
            interval=1 * 1000,  # in milliseconds
            n_intervals=0
        )
    ])
)

@app.callback(Output('live-update-text', 'children'),
              Output('live-update-graph', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_metrics_and_graph(n):
    data = rocket.time_series_data

    text_children = [
        html.Span('Time: {0:.2f}'.format(data['Time'][-1])),
        html.Span('Altitude: {0:.2f}'.format(data['Altitude'][-1])),
        html.Span('Velocity: {0:0.2f}'.format(data['Velocity'][-1]))
    ]

    fig = go.Figure()
    fig.add_trace(go.Scatter(x=data['Time'], y=data['Altitude'], mode='lines+markers', name='Altitude'))
    fig.add_trace(go.Scatter(x=data['Time'], y=data['Velocity'], mode='lines+markers', name='Velocity'))

    fig.update_layout(title_text='Rocket Altitude and Velocity',
                      xaxis_title='Time',
                      yaxis_title='Value',
                      title_x=0.5,
                      height=600,
                      showlegend=True)

    return text_children, fig

if __name__ == '__main__':
    app.run(debug=True)
