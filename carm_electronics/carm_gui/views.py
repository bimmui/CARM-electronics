from django.shortcuts import render

# Create your views here.

from django.shortcuts import render
from .models import TelemetryData


def index(request):
    latest_data = TelemetryData.objects.last()
    return render(request, "telemetry/index.html", {"data": latest_data})
