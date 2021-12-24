import os
import requests

from flask import redirect, request, session
from functools import wraps

HW_SERVER_ADR = "http://localhost:8080/"


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
