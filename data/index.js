function updateBattery(level, charging = false) {
    var batteryPercent = document.getElementById('battery-percent');
    var batteryLevel = document.getElementById('battery-level');
    var batteryLoading = document.getElementById('loading');
    batteryPercent.innerHTML = level + '%';
    batteryLevel.style.width = level + '%';
    if (charging) {
        batteryLoading.classList.remove('hide');
    } else{
        batteryLoading.classList.add('hide');
    }
    if (level > 50) {
        batteryLevel.classList.add('high');
        batteryLevel.classList.remove('medium');
        batteryLevel.classList.remove('low');
    } else if (level >= 25) {
        batteryLevel.classList.add('medium');
        batteryLevel.classList.remove('high');
        batteryLevel.classList.remove('low');
    } else {
        batteryLevel.classList.add('low');
        batteryLevel.classList.remove('high');
        batteryLevel.classList.remove('medium');
    }
}

const arrayRange = (start, stop, step) =>
    Array.from(
    { length: (stop - start) / step + 1 },
    (value, index) => start + index * step
    );   

const maxDiagrammX = 10;
const options = {
    fullWidth: true,
    showPoint: false,
    axisX: {
        showLabel: false,
        type : Chartist.FixedScaleAxis,
        low : 0,
        high : 10,
        ticks : [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    },
    axisY: {
        offset: 0,
        showLabel: false,
        type : Chartist.FixedScaleAxis,
        low : -5,
        high : 5,
        ticks : [ -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]
    }
};

var dataCH1;
var dataCH2;

var data = {series: [[], [{ x: 0, y: 0}, { x: maxDiagrammX, y: 0}], [], [{ x: 0, y: 0}, { x: maxDiagrammX, y: 0}], [{ x: 0, y: 0}, { x: maxDiagrammX, y: 0}]]};

var chart = new Chartist.Line('.ct-chart', data, options);



var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

var showCH1 = true;
var showCH2 = true;
var showTrigger = true;

function initElements() {
    document.getElementById('selectCH1').addEventListener('change', function() {
        if (this.checked) {
            showCH1 = true;
            var val = document.getElementById('sliderCH1').value;
            var line = [{ x: 0, y: val}, { x: maxDiagrammX, y: val}];
            data.series[1] = line;
            if (showCH1){ updateCH1(); }
        } else {
            showCH1 = false; 
            data.series[0] = [];
            data.series[1] = [];
            chart.update();
        } 
    });

    document.getElementById('selectCH2').addEventListener('change', function() {
        if (this.checked) {
            showCH2 = true;
            var val = document.getElementById('sliderCH2').value;
            var line = [{ x: 0, y: val}, { x: maxDiagrammX, y: val}];
            data.series[3] = line;
            if (showCH2){ updateCH2(); }
        } else {
            showCH2 = false; 
            data.series[2] = [];
            data.series[3] = [];
            chart.update();
        } 
    });

    document.getElementById('selectTrigger').addEventListener('change', function() {
        if (this.checked) {
            showTrigger = true;
            var val = document.getElementById('sliderTrigger').value;
            var line = [{ x: 0, y: val}, { x: maxDiagrammX, y: val}];
        } else {
            showTrigger = false; 
            data.series[4] = [];
        }
        chart.update();
    });

    document.getElementById('sliderCH1').addEventListener('input', function() {
        if (showTrigger){
            data.series[1] = [{ x: 0, y: this.value}, { x: maxDiagrammX, y: this.value}];
            chart.update();
        }
        websocket.send(JSON.stringify({ id: "offset-CH1", data: this.value }));
    });

    document.getElementById('sliderCH2').addEventListener('input', function() {
        if (showTrigger){
            data.series[3] = [{ x: 0, y: this.value}, { x: maxDiagrammX, y: this.value}];
            chart.update();
        }
        websocket.send(JSON.stringify({ id: "offset-CH2", data: this.value }));
    });

    document.getElementById('sliderTrigger').addEventListener('input', function() {
        if (showTrigger){
            data.series[4] = [{ x: 0, y: this.value}, { x: maxDiagrammX, y: this.value}];
            chart.update();
        }
        websocket.send(JSON.stringify({ id: "trigger", data: this.value }));
    });

    document.getElementById('timeOffset').addEventListener('input', function() {
        updateCH1();
        updateCH2();
    });
    
    document.getElementById('start').addEventListener('click', function() {
        if (this.classList.contains('btn-success')){//start
            this.classList.remove('btn-success');
            this.classList.add('btn-danger');
            this.innerHTML = 'stop';
            websocket.send(JSON.stringify({ id: "trigger-run", data: "start" }));
        }
        else{
            this.classList.remove('btn-danger');
            this.classList.add('btn-success');
            this.innerHTML = 'start';
            websocket.send(JSON.stringify({ id: "trigger-run", data: "stop" }));
        }
    });

    document.getElementById('mode').addEventListener('change', function() {
        websocket.send(JSON.stringify({ id: "trigger-mode", data: this.value}));
    });

    document.getElementById('edge').addEventListener('change', function() {
        websocket.send(JSON.stringify({ id: "trigger-edge", data: this.value}));
    });

    document.getElementById('channel').addEventListener('change', function() {
        websocket.send(JSON.stringify({ id: "trigger-channel", data: this.value}));
    });

    document.getElementById('gainCH1').addEventListener('change', function() {
        websocket.send(JSON.stringify({ id: "gain-CH1", data: this.value}));
        if (showCH1){ updateCH1(); }
    });

    document.getElementById('gainCH2').addEventListener('change', function() {
        websocket.send(JSON.stringify({ id: "gain-CH2", data: this.value}));
        if (showCH2){ updateCH2(); }
    });
    
    document.getElementById('CH1_1:1').addEventListener('change', function() {
        if (showCH1){ updateCH1(); }
        console.log("trigger probe CH1");
    });
    document.getElementById('CH1_10:1').addEventListener('change', function() {
        if (showCH1){ updateCH1(); }
        console.log("trigger probe CH1");
    });

    document.getElementById('CH2_1:1').addEventListener('change', function() {
        if (showCH2){ updateCH2(); }
        console.log("trigger probe CH2");
    });
    document.getElementById('CH2_10:1').addEventListener('change', function() {
        if (showCH2){ updateCH2(); }
        console.log("trigger probe CH2");
    });
}

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
    //var msg = { data: "getAll" };
    //websocket.send(JSON.stringify(msg));
    document.getElementById('battery').classList.remove('hide');
    document.getElementById('error').classList.add('hide');
    updateBattery(50, true);
}

//gets called on websocket connection lost
function onClose(event) {
    console.log('Connection closed');
    //try to connect again after connection lost
    setTimeout(initWebSocket, 2000);
    document.getElementById('battery').classList.add('hide');
    document.getElementById('error').classList.remove('hide');
}

//recived new message through websocket
function onMessage(event) {
    console.log(event.data);
    var recivedData = JSON.parse(event.data);
    //console.log(recivedData);
    //console.log("id:" + recivedData.id + "|data:" + recivedData.data);
    //console.log(recivedData.CH1);
    //console.log(recivedData.CH2);
    if (recivedData.hasOwnProperty("CH1")) {
        dataCH1 = recivedData.CH1;
        if (showCH1){
            updateCH1();
        }
        console.log(dataCH1);
    }
    else if (recivedData.hasOwnProperty("CH2")) {
        dataCH2 = recivedData.CH2;
        if (showCH2){
            updateCH2();
        }
        console.log(dataCH1);
    }
    else if (recivedData.hasOwnProperty("battery")) {updateBattery(recivedData.battery[0], recivedData.battery[1]);}
}

const samplesPerSend = 100;
const timeDelta = maxDiagrammX / samplesPerSend;
function updateCH1(){
    const dataTimeOffset = Number(document.getElementById('timeOffset').value);
    const scale = Number(document.getElementById('gainCH1').value);
    var probe = Number(1);
    if (document.getElementById('CH1_10:1').checked){
        probe = 1/Number(10);
    }
    var val;
    for (let i = 0; i < samplesPerSend; i++) {
        val = dataCH1[Number(i) + Number(dataTimeOffset)] * scale * probe;
        if (val > 5){ val = 5; }
        if (val < -5){ val = -5; }
        data.series[0][i] = {"x": i*timeDelta, "y": val};
    }
    chart.update();
}
function updateCH2(){
    const dataTimeOffset = Number(document.getElementById('timeOffset').value);
    const scale = Number(document.getElementById('gainCH2').value);
    var probe = Number(1);
    if (document.getElementById('CH2_10:1').checked){
        probe = 1/Number(10);
    }
    var val;
    for (let i = 0; i < samplesPerSend; i++) {
        val = dataCH2[Number(i) + Number(dataTimeOffset)] * scale * probe;
        if (val > 5){ val = 5; }
        if (val < -5){ val = -5; }
        data.series[2][i] = {"x": i*timeDelta, "y": val};
    }
    //console.log("sample Array: ");
    //console.log(sampleArray);
    //console.log("chartist Data: ");
    //console.log(data);
    chart.update();
}

function updataChartData(channel, sampleArray){
    var val;
    for (let i = 0; i < samplesPerSend; i++) {
        if (channel == "CH1"){
            val = sampleArray[i + dataTimeOffset] * scaleCH1;
            //console.log("i: ");
            //console.log(i);
            //console.log("dataTimeOffset: ");
            //console.log(dataTimeOffset);
        }
        else{
            val = sampleArray[i + dataTimeOffset] * scaleCH2;
        }
        if (val > 5){ val = 5; }
        if (val < -5){ val = -5; }
        if (channel == "CH1"){
            data.series[0][i] = {"x": i*timeDelta, "y": val};
        }
        else{
            data.series[2][i] = {"x": i*timeDelta, "y": val};
        }
    }
    //console.log("sample Array: ");
    //console.log(sampleArray);
    //console.log("chartist Data: ");
    //console.log(data);
    chart.update();
}

//callback when website gets loaded and closed
window.addEventListener('load', onLoad);
window.addEventListener('close', disconect);

//init all when loading side
function onLoad(event) {
    initWebSocket();
    initElements();
}

//close websocket on closing side
function disconect(event) {
    websocket.close();
}