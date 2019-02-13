import { Accelerometer } from "accelerometer";
import { Barometer } from "barometer";
import { BodyPresenceSensor } from "body-presence";
import { display } from "display";
import document from "document";
import { Gyroscope } from "gyroscope";
import { HeartRateSensor } from "heart-rate";
import { OrientationSensor } from "orientation";
import { vibration } from "haptics";
import * as messaging from "messaging";

display.autoOff = false;
display.on = true;

const accel = new Accelerometer();
const bar = new Barometer();
const bps = new BodyPresenceSensor();
const gyro = new Gyroscope();
const hrm = new HeartRateSensor();
const orientation = new OrientationSensor();

accel.start();
bar.start();
bps.start();
gyro.start();
hrm.start();
orientation.start();

const accelData = document.getElementById("accel-data");
const barData = document.getElementById("bar-data");
const bpsData = document.getElementById("bps-data");
const gyroData = document.getElementById("gyro-data");
const hrmData = document.getElementById("hrm-data");
const orientationData = document.getElementById("orientation-data");
const connectionData = document.getElementById("connection-data");
const timingData = document.getElementById("timing-data");

let displayCount = 0;
let isConnected = false;
let initialTime = Date.now();
let companionTiming = 0;
let responseWaitingTimer = null;

messaging.peerSocket.onopen = () => {
  console.log("Ready");
};

messaging.peerSocket.onerror = (err) => {
  console.log(`Connection error: ${err.code} - ${err.message}`);
};

messaging.peerSocket.onmessage = (event) => {
  console.log(event.data);
  const newTiming = Number(event.data);
  companionTiming = Math.max(0, newTiming * 0.9);
  isConnected = true;
  refreshResponseWaitingTimer();
};

function sendDataToCompanion(data) {
  if (messaging.peerSocket.readyState === messaging.peerSocket.OPEN) {
    console.log('send');
    data.ts = (Date.now()-initialTime);
    messaging.peerSocket.send(data);
  }
}

function refreshResponseWaitingTimer() {
  if (responseWaitingTimer) {
    clearTimeout(responseWaitingTimer);
    responseWaitingTimer = null;
  }
  responseWaitingTimer = setTimeout(onResponseWaitingTimer, 5000);
}
function onResponseWaitingTimer() {
  vibration.start('ping');
  isConnected = false;
  companionTiming = 0;
  refreshResponseWaitingTimer();
}

function refreshData() {
  const data = {
    accel: {
      x: accel.x ? accel.x.toFixed(3) : 0,
      y: accel.y ? accel.y.toFixed(3) : 0,
      z: accel.z ? accel.z.toFixed(3) : 0
    },
    bar: {
      pressure: bar.pressure ? parseInt(bar.pressure) : 0
    },
    bps: {
      presence: bps.present
    },
    gyro: {
      x: gyro.x ? gyro.x.toFixed(3) : 0,
      y: gyro.y ? gyro.y.toFixed(3) : 0,
      z: gyro.z ? gyro.z.toFixed(3) : 0,
    },
    hrm: {
      heartRate: hrm.heartRate ? hrm.heartRate : 0
    },
    orientation: orientation.quaternion ? orientation.quaternion.map(n => n.toFixed(3)) : null,
  }
  
  displayCount += 1;
  if (displayCount % 5 == 0) {
    accelData.text = JSON.stringify(data.accel);
    barData.text = JSON.stringify(data.bar);
    bpsData.text = JSON.stringify(data.bps);
    gyroData.text = JSON.stringify(data.gyro);
    hrmData.text = JSON.stringify(data.hrm);
    orientationData.text = JSON.stringify(data.orientation);
    connectionData.text = isConnected ? 'O' : 'X';
    timingData.text = companionTiming + ' ms';
  }
  
  sendDataToCompanion(data);
  
  let refreshPeriod = 1000;
  if (companionTiming > 0) {
    refreshPeriod = Math.max(300, companionTiming);
  }
  refreshPeriod = Math.min(2000, refreshPeriod);
  setTimeout(refreshData, refreshPeriod);
}

setTimeout(function() {
  refreshData();
  refreshResponseWaitingTimer();
}, 5000);
