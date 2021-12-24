import os
import requests

from geopy import distance
from flask import redirect, request, session
from functools import wraps

HW_SERVER_ADR = "http://localhost:8080/"
HW_HOME_LOCATION = (53.878872, 27.553719)
DISTANCE_WHEN_NEED_DISABLE = float(150.0)

def login_required(f):
    """
    Decorate routes to require login.

    https://flask.palletsprojects.com/en/1.1.x/patterns/viewdecorators/
    """
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get("user_id") is None:
            return redirect("/login")
        return f(*args, **kwargs)
    return decorated_function


def get_floor_lamp_status():
    try:
        url = HW_SERVER_ADR + "state"
        response = requests.get(url)
        response.raise_for_status()
    except requests.RequestException:
        return None

    return response.json()


def do_floor_lamp(action):
    try:
        url = HW_SERVER_ADR + action
        response = requests.get(url)
        response.raise_for_status()
    except requests.RequestException:
        return None

    return response.json()


def lamp_off_if_needed(latitude, longitude):
    current = (float(latitude), float(longitude))

    status = get_floor_lamp_status()
    if status is not None:
        isok = status["hw-answer"]
    else:
        isok = "None"

    curr_dist = distance.distance(current, HW_HOME_LOCATION).m
    print("Loacation: " + str(curr_dist) + " m")

    if isok == "L1":
        if curr_dist >= DISTANCE_WHEN_NEED_DISABLE:
            do_floor_lamp("off")
            return  {
                "action" : "Lamp off done",
                "distance" : str(curr_dist)
                }
    return {"action" : "Not needed"}
