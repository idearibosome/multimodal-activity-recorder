import { settingsStorage } from 'settings';
import * as messaging from 'messaging';

let address = JSON.parse(settingsStorage.getItem('address'))['name'];

messaging.peerSocket.onopen = () => {
  console.log('Ready');
};

messaging.peerSocket.onerror = (err) => {
  console.log(`Connection error: ${err.code} - ${err.message}`);
};

messaging.peerSocket.onmessage = (event) => {
  sendDataToClient(event.data);
};

function sendDataToClient(data) {
  let jsonData = JSON.stringify(data);
  fetch(address, {method: 'POST', mode: 'no-cors', body: jsonData})
    .then((res) => {})
    .catch((error) => { console.log(error); });
}

console.log('Companion started');


