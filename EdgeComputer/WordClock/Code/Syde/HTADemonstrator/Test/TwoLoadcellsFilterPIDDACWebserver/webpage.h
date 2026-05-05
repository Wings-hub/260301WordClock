const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>SYDE HTA Loadcell Response</title>
  <style>
    body { font-family: Arial, sans-serif; background-color: #f4f4f9; text-align: center; }
    .container { max-width: 800px; margin: auto; background: white; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); border-radius: 10px; }
    .frame-container { display: flex; justify-content: space-around; }
    .frame { border: 2px solid #ccc; padding: 20px; border-radius: 10px; background-color: #fff; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); width: 45%; }
    button { background-color: #02aff3; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }
    button:hover { background-color: #6e6e6e; }

    input[type=range] {
      -webkit-appearance: none;
      width: 60%;
      height: 6px;
      background: #ddd;
      border-radius: 5px;
      outline: none;
    }

    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 18px;
      height: 18px;
      background: #02aff3;
      border-radius: 50%;
      cursor: pointer;
      border: none;
      box-shadow: 0 0 2px #666;
    }

    input[type=range]::-moz-range-thumb {
      width: 18px;
      height: 18px;
      background: #02aff3;
      border-radius: 50%;
      cursor: pointer;
      border: none;
      box-shadow: 0 0 2px #666;
    }
  </style>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <div class="container">
    <img src="/image" alt="SYDE Logo" style="max-width: 100%; height: auto;">
    <h1>HTA Loadcell Communication Check</h1>
    <div class="reset-container">
      <button onclick="ResetAll()">Reset All</button>
      <h2> </h2>
    </div>
    <div class="frame-container">
      <div class="frame">
        <h2>Loadcell 1</h2>
        <button onclick="Zero1()">Zero</button>
        <button onclick="Enter1()">Enter</button>
        <button onclick="Start1()">Start</button>
        <h2 id="status1">Status: Waiting...</h2>
        <h2 id="weight1">Waiting...</h2>
        <h4 id="K1">K1: waiting for calibration...</h4>
        <h4 id="C1">C1: waiting for calibration...</h4>
      </div>
      <div class="frame">
        <h2>Loadcell 2</h2>
        <button onclick="Zero2()">Zero</button>
        <button onclick="Enter2()">Enter</button>
        <button onclick="Start2()">Start</button>
        <h2 id="status2">Status: Waiting...</h2>
        <h2 id="weight2">Waiting...</h2>
        <h4 id="K2">K2: waiting for calibration...</h4>
        <h4 id="C2">C2: waiting for calibration...</h4>
      </div>
      <div class="frame">
        <h2>Costanti</h2>
        <label for="p">Filter depth (p):</label><br>
        <input type="number" id="p" name="p" value="10" min="1" max="100"><br><br>

        <label for="kp1">PID1 Kp1:</label><br>
        <input type="number" id="kp1" name="kp1" value="1.2" min="-10" max="10"><br><br>
        <label for="kd1">PID1 Kd1:</label><br>
        <input type="number" id="kd1" name="kd1" value="0.4" min="-10" max="10"><br><br>

        <label for="kp2">PID2 Kp2:</label><br>
        <input type="number" id="kp2" name="kp2" value="1.2" min="-10" max="10"><br><br>
        <label for="kd2">PID2 Kd2:</label><br>
        <input type="number" id="kd2" name="kd2" value="0.4" min="-10" max="10"><br><br>

        <label for="B">Banda morta B (mV):</label><br>
        <input type="number" id="B" name="B" value="0" min="0" max="2000"><br><br>
        <button onclick="sendPIDeBSettings()">Apply Settings</button>
      </div>
    </div>
    <h2>HX711 raw</h2>
    <canvas id="hxrawChart" width="800" height="250"></canvas>

    <h2>HX711 Fl</h2>
    <canvas id="hxFlChart" width="800" height="250"></canvas>

    <h2>Valori DAC</h2>
    <canvas id="dacChart" width="800" height="250"></canvas>

    <h2>Test Segnale per DAC</h2>
    <button onclick="StartWave()">Start Wave</button>
    <canvas id="waveChart" width="800" height="250"></canvas>
  </div>

  <script>

    let dataInterval = null;

    function startDataFetchingForConstants() {
      if (dataInterval !== null) return; // evita duplicati

      dataInterval = setInterval(() => {
        fetch('/data')
        .then(response => response.json())
        .then(data => {
          if (data.K1 !== undefined) {
            document.getElementById("K1").innerText = "K1 = " + Number(data.K1).toFixed(2);
          }
          if (data.C1 !== undefined) {
            document.getElementById("C1").innerText = "C1 = " + data.C1;
          }
          if (data.K2 !== undefined) {
            document.getElementById("K2").innerText = "K2 = " + Number(data.K2).toFixed(2);
          }
          if (data.C2 !== undefined) {
            document.getElementById("C2").innerText = "C2 = " + data.C2;
          }
        });
      }, 100);
    }

    function ResetAll() {
      fetch('/resetall')
      .then(response => response.text())
      .then(data => alert(data));
      if (dataInterval !== null) {
        clearInterval(dataInterval);
        dataInterval = null;
      }
    }

    function Zero1() {
      fetch('/zero1')
      .then(response => response.text())
      .then(data => document.getElementById("status1").innerText = data);
    }
    function Zero2() {
      fetch('/zero2')
      .then(response => response.text())
      .then(data => document.getElementById("status2").innerText = data);
    }
    function Enter1() {
      fetch('/enter1')
      .then(response => response.text())
      .then(data => document.getElementById("status1").innerText = data);
      startDataFetchingForConstants();
    }
    function Enter2() {
      fetch('/enter2')
      .then(response => response.text())
      .then(data => document.getElementById("status2").innerText = data);
      startDataFetchingForConstants();
    }

    function Start1() {
      fetch('/start1')
      .then(response => response.text())
      .then(data => document.getElementById("status1").innerText = data);
      fetchWeightContinuously1();
      startDataFetching();
    }

    function Start2() {
      fetch('/start2')
      .then(response => response.text())
      .then(data => document.getElementById("status2").innerText = data);
      fetchWeightContinuously2();
      startDataFetching();
    }

    function StartWave() {
      fetch('/startWave')
      startDataFetching();
    }

    function fetchWeightContinuously1() {
      setInterval(() => {
        fetch('/weight1')
        .then(response => response.text())
        .then(data => document.getElementById("weight1").innerText = data);
      }, 100);
    }

    function fetchWeightContinuously2() {
      setInterval(() => {
        fetch('/weight2')
        .then(response => response.text())
        .then(data => document.getElementById("weight2").innerText = data);
      }, 100);
    }

    const labels = [];
    const raw1 = [], Fl1 = [], raw2 = [], Fl2 = [], dac1 = [], dac2 = [], WaveData = [], vDAC1TEST = [], vDAC2TEST = [];
    let index = 0;

    const hxrawCtx = document.getElementById('hxrawChart').getContext('2d');
    const hxFlCtx = document.getElementById('hxFlChart').getContext('2d');
    const dacCtx = document.getElementById('dacChart').getContext('2d');
    const waveCtx = document.getElementById('waveChart').getContext('2d');

    const hxrawChart = new Chart(hxrawCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          { label: 'raw1', data: raw1, borderWidth: 1},
          { label: 'raw2', data: raw2, borderWidth: 1 },
        ]
      },
      options: {
        responsive: true,
        animation: false,
        scales: {
          y: { beginAtZero: false },
          x: {
            type: 'linear',
            min: 0,
            max: 100
          }
        },
        plugins: {
          legend: { display: true }
        }
      }

    });

    const hxFlChart = new Chart(hxFlCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          { label: 'Fl1', data: Fl1, borderWidth: 1 },
          { label: 'Fl2', data: Fl2, borderWidth: 1 }
        ]
      },
      options: {
        responsive: true,
        animation: false,
        scales: {
          y: { beginAtZero: false },
          x: {
            type: 'linear',
            min: 0,
            max: 100
          }
        },
        plugins: {
          legend: { display: true }
        }
      }

    });

    const dacChart = new Chart(dacCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          { label: 'vDAC1', data: dac1, borderWidth: 1, tension: 0.3 },
          { label: 'vDAC2', data: dac2, borderWidth: 1, tension: 0.3 }
        ]
      },
      options: {
        responsive: true,
        animation: false,
        scales: {
          y: { beginAtZero: false },
          x: {
            type: 'linear',
            min: 0,
            max: 100
          }
        },
        plugins: {
          legend: { display: true }
        }
      }
    });

    const waveChart = new Chart(waveCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          { label: 'WaveValues', data: WaveData, borderWidth: 1, tension: 0.3, yAxisID: 'y' },
          { label: 'vDAC1Values', data: vDAC1TEST, borderWidth: 1, tension: 0.3, yAxisID: 'y1' },
          { label: 'vDAC2Values', data: vDAC2TEST, borderWidth: 1, tension: 0.3, yAxisID: 'y1' }
        ]
      },
      options: {
        responsive: true,
        animation: false,
        scales: {
          y: {
            type: 'linear',
            position: 'left',
            min: 0,
            max: 4095,
            title: { display: true, text: 'WaveValues (0-4095)' }
          },
          x: {
            type: 'linear',
            min: 0,
            max: 100
          },
          y1: {
            type: 'linear',
            position: 'right',
            min: 0,
            max: 3500,
            grid: {
              drawOnChartArea: false
            },
            title: { display: true, text: 'vDACValues (0-3300 mV)' }
          }
        },
        plugins: {
          legend: { display: true }
        }
      }
    });

    let updateInterval = null;

    function startDataFetching() {
      if (updateInterval === null) {
        updateInterval = setInterval(() => {
          fetch('/data')
          .then(res => res.json())
          .then(d => updateCharts(d));
        }, 100);
      }
    }

    function updateCharts(d) {
      if (labels.length >= 100) {
        labels.shift(); raw1.shift(); Fl1.shift(); raw2.shift(); Fl2.shift(); dac1.shift(); dac2.shift(); WaveData.shift(); vDAC1TEST.shift(); vDAC2TEST.shift();
      }

      labels.push(index);
      raw1.push(d.raw1); Fl1.push(d.Fl1);
      raw2.push(d.raw2); Fl2.push(d.Fl2);
      dac1.push(d.vDAC1); dac2.push(d.vDAC2);
      WaveData.push(d.WaveData); vDAC1TEST.push(d.vDAC1TEST); vDAC2TEST.push(d.vDAC2TEST);

 
      hxrawChart.options.scales.x.min = index - 99;
      hxrawChart.options.scales.x.max = index;
      hxFlChart.options.scales.x.min = index - 99;
      hxFlChart.options.scales.x.max = index;
      dacChart.options.scales.x.min = index - 99;
      dacChart.options.scales.x.max = index;
      waveChart.options.scales.x.min = index - 99;
      waveChart.options.scales.x.max = index;

      hxrawChart.update();
      hxFlChart.update();
      dacChart.update();
      waveChart.update();

    index++;
    }


    function sendPIDeBSettings() {
      const p = document.getElementById("p").value;
      const kp1 = document.getElementById("kp1").value;
      const kd1 = document.getElementById("kd1").value;
      const kp2 = document.getElementById("kp2").value;
      const kd2 = document.getElementById("kd2").value;
      const B = document.getElementById("B").value;
      fetch(`/set?p=${p}&kp1=${kp1}&kd1=${kd1}&kp2=${kp2}&kd2=${kd2}&B=${B}`);
    }
  </script>
</body>
</html>
)rawliteral";