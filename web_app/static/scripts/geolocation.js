if ((window.innerWidth <= 800) || (window.innerHeight <= 600)) {
    // Get location to send it to server.
    // If you forgot to turn off your lamp,
    // we do it instead of you!

    // Request repeated updates.
    const watchId = navigator.geolocation.watchPosition(
      send_position, handle_error
    );
}

function send_position(position) {
    var request = new XMLHttpRequest()
    const { latitude, longitude } = position.coords;

    request.onload = function() {        
        if (request.response["action"] == "Lamp off done") {
            // show alert about this
            document.getElementById("floor-lamp-info").style.color = "red"
            document.getElementById("floor-lamp-info").innerHTML = "It seems, you are not at your home, therefore your lamp was turned off!" +
                " Your distance from home is " + request.response["distance"]
        }
    }

    request.responseType = 'json';
    request.open("GET", "/location?latitude=" + latitude + "&longitude=" + longitude, true);
    request.send();
}

function handle_error(error) {
    // Display error based on the error code.
    const { code } = error;

    switch (code) {
    case GeolocationPositionError.TIMEOUT:
        console.log("[GEO]: Handle timeout")
        break;
    case GeolocationPositionError.PERMISSION_DENIED:
        console.log("[GEO]: User denied the request")
        break;
    case GeolocationPositionError.POSITION_UNAVAILABLE:
        console.log("[GEO]: Position not available")
        break;
    }
}