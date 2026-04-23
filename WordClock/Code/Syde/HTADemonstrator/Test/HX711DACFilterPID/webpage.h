const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>HTA Loadcell Communication Check</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f9; }
        .container { max-width: 1200px; margin: auto; background: white; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); border-radius: 10px; }
        .frame-container { display: flex; justify-content: space-around; flex-wrap: wrap; gap: 20px; }
        .frame { border: 2px solid #ccc; padding: 20px; border-radius: 10px; background-color: #fff; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); width: 300px; }
        .chart-wrapper { border: 2px solid #ccc; border-radius: 10px; margin-top: 30px; padding: 20px; background: #fff; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); }
        .chart-container { display: flex; justify-content: center; gap: 20px; flex-wrap: wrap; }
        button { background-color: #02aff3; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin: 4px; }
        button:hover { background-color: #6e6e6e; }
        input[type=range], input[type=number] { width: 60%; margin: 10px 0; }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
<div class="container">
    <img src="/image" alt="SYDE Logo" style="max-width: 200px;">
    <h1>HTA Loadcell Communication Check</h1>
    <div>
        <button onclick="ResetAll()">Reset All</button>
        <button onclick="startGraph()">Start Graph</button>
        <button onclick="stopGraph()">Stop Graph</button>
    </div>

    <div class="frame-container">
        <div class="frame">
            <h2>Loadcell 1</h2>
            <button onclick="Zero1()">Zero</button>
            <button onclick="Enter1()">Enter</button>
            <button onclick="Start1()">Start</button>
            <h3 id="status1">Status: Waiting...</h3>
            <h3 id="weight1">Weight: Waiting...</h3>
            <p>Set Kp1: <input type="number" step="0.01" id="kp1" value="1.0" onchange="setKp1(this.value)"></p>
            <p>Set Kd1: <input type="number" step="0.01" id="kd1" value="0.1" onchange="setKd1(this.value)"></p>
        </div>
        <div class="frame">
            <h2>Loadcell 2</h2>
            <button onclick="Zero2()">Zero</button>
            <button onclick="Enter2()">Enter</button>
            <button onclick="Start2()">Start</button>
            <h3 id="status2">Status: Waiting...</h3>
            <h3 id="weight2">Weight: Waiting...</h3>
            <p>Set Kp2: <input type="number" step="0.01" id="kp2" value="1.0" onchange="setKp2(this.value)"></p>
            <p>Set Kd2: <input type="number" step="0.01" id="kd2" value="0.1" onchange="setKd2(this.value)"></p>
        </div>
        <div class="frame">
            <h2>Filter Option</h2>
            <p>
                Moving Average (p):
                <input type="range" min="1" max="100" value="10" id="filterP" oninput="updateP(this.value)">
                <span id="pval">10</span>
            </p>
            <h3>Filter Behavior Guide</h3>
            <table style="margin:auto; border-collapse:collapse;">
                <tr><th>p-Wert</th><th>Verhalten</th></tr>
                <tr><td>1–5</td><td>Schnell, kaum Glättung</td></tr>
                <tr><td>6–20</td><td>Leichte Glättung</td></tr>
                <tr><td>21–60</td><td>Stark geglättet</td></tr>
                <tr><td>61–100</td><td>Sehr stabil, verzögert</td></tr>
            </table>
        </div>
    </div>

    <div class="chart-wrapper">
        <div class="chart-container">
            <canvas id="chart1" width="400" height="200"></canvas>
            <canvas id="chart2" width="400" height="200"></canvas>
        </div>
        <div>
            <button onclick="toggleTrace('Raw1')">Raw 1</button>
            <button onclick="toggleTrace('Raw2')">Raw 2</button>
            <button onclick="toggleTrace('Filtered1')">Filtered 1</button>
            <button onclick="toggleTrace('Filtered2')">Filtered 2</button><br>
            <button onclick="toggleTrace('DAC1')">DAC 1</button>
            <button onclick="toggleTrace('DAC2')">DAC 2</button>
        </div>
    </div>
</div>

<script>

let chart1, chart2;
let intervalID;
let chartData1 = {
    labels: [],
    datasets: [
        { label: 'Raw1', data: [], borderColor: 'red', hidden: false },
        { label: 'Raw2', data: [], borderColor: 'orange', hidden: false },
        { label: 'Filtered1', data: [], borderColor: 'blue', hidden: false },
        { label: 'Filtered2', data: [], borderColor: 'green', hidden: false }
    ]
};
let chartData2 = {
    labels: [],
    datasets: [
        { label: 'DAC1', data: [], borderColor: 'purple', hidden: false },
        { label: 'DAC2', data: [], borderColor: 'brown', hidden: false }
    ]
};

function initCharts() {
    const ctx1 = document.getElementById('chart1').getContext('2d');
    const ctx2 = document.getElementById('chart2').getContext('2d');
    chart1 = new Chart(ctx1, { type: 'line', data: chartData1, options: { responsive: true, animation: false, scales: { x: { display: false }, y: { beginAtZero: true }}} });
    chart2 = new Chart(ctx2, { type: 'line', data: chartData2, options: { responsive: true, animation: false, scales: { x: { display: false }, y: { beginAtZero: true }}} });
}

function updateCharts(data) {
    chartData1.labels.push('');
    chartData2.labels.push('');

    chartData1.datasets[0].data.push(data.raw1);
    chartData1.datasets[1].data.push(data.raw2);
    chartData1.datasets[2].data.push(data.filtered1);
    chartData1.datasets[3].data.push(data.filtered2);

    chartData2.datasets[0].data.push(data.dac1);
    chartData2.datasets[1].data.push(data.dac2);

    const maxPoints = 100;
    [chartData1, chartData2].forEach(chart => {
        chart.labels.length > maxPoints && chart.labels.shift();
        chart.datasets.forEach(ds => ds.data.length > maxPoints && ds.data.shift());
    });

    chart1.update();
    chart2.update();
}

function fetchData() {
    fetch('/status')
        .then(r => r.json())
        .then(updateCharts)
        .catch(console.error);
}

function startGraph() {
    if (!intervalID) intervalID = setInterval(fetchData, 200);
}

function stopGraph() {
    clearInterval(intervalID);
    intervalID = null;
}

function toggleTrace(label) {
    [chart1, chart2].forEach(chart => {
        const ds = chart.data.datasets.find(d => d.label === label);
        if (ds) ds.hidden = !ds.hidden;
    });
}

function Zero1() { fetch('/zero1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t); }
function Zero2() { fetch('/zero2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t); }
function Enter1() { fetch('/enter1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t); }
function Enter2() { fetch('/enter2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t); }
function Start1() { fetch('/start1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t); }
function Start2() { fetch('/start2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t); }
function ResetAll() { fetch('/resetall').then(r => r.text()).then(alert); }
function setKp1(val) { fetch('/setkp1?value=' + val); }
function setKd1(val) { fetch('/setkd1?value=' + val); }
function setKp2(val) { fetch('/setkp2?value=' + val); }
function setKd2(val) { fetch('/setkd2?value=' + val); }
function updateP(val) {
    document.getElementById("pval").innerText = val;
    fetch('/setp?value=' + val);
}

initCharts();
</script>

</body>
</html>
)rawliteral";
