var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

//callbacks for socket communication
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

//request all data when socket opens
function onOpen(event) {
    console.log('Connection opened');
    //anfrage für IO werte stellen
    var msg = { data: "getAll" };
    websocket.send(JSON.stringify(msg));
}

//gets called on websocket connection lost
function onClose(event) {
    console.log('Connection closed');
    //try to connect again after connection lost
    setTimeout(initWebSocket, 2000);
}

//recived new message through websocket
function onMessage(event) {
    console.log(event.data);
    var data = JSON.parse(event.data);
    //new temperature value
    if (data.id == 'temp') {
        document.getElementById('tempVal').innerHTML = data.data + '°C';
        console.log('new temp=' + data.data + ';');
    }
}

//callback to change output, when button gets pushed
function initButtons() {
    //document.getElementById('wifi').addEventListener('click', disconectSocket);
}

//send request to toggle output
function toggle() {
    var msg = { id: this.id, data: "toggle" };
    websocket.send(JSON.stringify(msg));
}

//callback when website gets loaded and closed
window.addEventListener('load', onLoad);
window.addEventListener('close', disconect);

//init all when loading side
function onLoad(event) {
    initWebSocket();
    initButtons();
}

//close websocket on closing side
function disconect(event) {
    websocket.close();
}