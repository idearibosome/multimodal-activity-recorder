import { settingsStorage } from 'settings';
import * as messaging from 'messaging';

let address = JSON.parse(settingsStorage.getItem('address'))['name'];

let isSendingData = false;

messaging.peerSocket.onopen = () => {
  console.log('Ready');
};

messaging.peerSocket.onerror = (err) => {
  console.log(`Connection error: ${err.code} - ${err.message}`);
};

messaging.peerSocket.onmessage = (event) => {
  sendDataToClient(event.data);
};

function fetchWithTimeout(url, options, timeout=500) {
  return Promise.race([
    fetch(url, options),
    new Promise((_, reject) =>
      setTimeout(() => reject(new Error('timeout')), timeout)
    )
  ]);
}

function sendDataToClient(data) {
  if (isSendingData) {
    return;
  }
  isSendingData = true;
  
  let jsonData = JSON.stringify(data);
  fetchWithTimeout(address, {method: 'POST', mode: 'no-cors', body: jsonData})
    .then((res) => {
      isSendingData = false;
    })
    .catch((error) => { 
      console.log(error); 
      isSendingData = false;
    });
}

console.log('Companion started');


