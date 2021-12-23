function floor_lamp(action) {
    var request = new XMLHttpRequest()

    request.onload = function() {
        if (request.response == "OK") {
            if (action == "on") {
                document.getElementById("floor-lamp-image").src = "/static/images/floor-lamp-on.jpg"
                document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>on</b>"
            } else if (action == "off") {
                document.getElementById("floor-lamp-image").src = "/static/images/floor-lamp-off.jpg"
                document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>off</b>"
            } else {
                document.getElementById("floor-lamp-image").src = "..."
                document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>error</b>"
            }
        } else {
            document.getElementById("floor-lamp-image").src = "..."
            document.getElementById("floor-lamp-card-text").innerHTML = "Status: <b>error</b>"
        }
    }

    // Send a request
    request.responseType = 'json';
    request.open("GET", "/floor_lamp?action=" + action, true);
    request.send();
}