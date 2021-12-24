function floor_lamp_state() {
    var request = new XMLHttpRequest()

    request.onload = function() {
        if (request.response == "L1") {
            document.getElementById("floor-lamp-image").src = "/static/images/floor-lamp-on.jpg"
            document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>on</b>"
        } else if (request.response == "L0") {
            document.getElementById("floor-lamp-image").src = "/static/images/floor-lamp-off.jpg"
            document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>off</b>"
        }
    }

    // Send a request
    request.responseType = 'json';
    request.open("GET", "/floor_lamp_state", true);
    request.send();
}

document.addEventListener('DOMContentLoaded', function() {
    setInterval(floor_lamp_state, 2500)
})