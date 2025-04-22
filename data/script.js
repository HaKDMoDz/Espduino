document.addEventListener('DOMContentLoaded',onDocumentReady);

function fetchReadings() {
    fetch('/readings')
    .then(response => {
         if (!response.ok) {
              throw new Error('The network response may be incorrectly configured\nor may have connectivity issues!');
         }
        return response.json();
    })
    .then(data => {
        console.log('Temperature:', data.temperature);
        console.log('Humidity:', data.humidity);
        document.getElementById('temperature').innerText = data.temperature.toFixed(1) + ' °C';
        document.getElementById('humidity').innerText = data.humidity.toFixed(1) + ' %';
    })
    .catch(error => {
        console.error('Error fetching readings:', error);
    });
}

function handleButtonClick(buttonId, endpoint, lightId) {
    const lightElement = document.getElementById(lightId);
    const currentSrc = lightElement.getAttribute('src');
    const newSrc = currentSrc === 'off2.jpg' ? 'on2.jpg' : 'off2.jpg';
    lightElement.setAttribute('src', newSrc);
    myAudio.play();
    fetch(endpoint, { method: 'POST' })
    .then(response => response.json())
    .then(data => {console.log('Success:', data);})
    .catch(error => {console.error('Error:', error);});
}

function onDocumentReady() {
    document.getElementById('fanButton1').addEventListener('click', () => handleButtonClick('fanButton1', '/fan', 'fan'));
    document.getElementById('light1Button2').addEventListener('click', () => handleButtonClick('light1Button2', '/light1', 'lite1'));
    document.getElementById('light2Button3').addEventListener('click', () => handleButtonClick('light2Button3', '/light2', 'lite2'));
    document.getElementById('light3Button4').addEventListener('click', () => handleButtonClick('light3Button4', '/light3', 'lite3'));
    document.getElementById('heaterButton5').addEventListener('click', () => handleButtonClick('heaterButton5', '/heat', 'heat'));
    document.getElementById('pins').addEventListener('click', () => { 
        window.location.href = 'pins.html'; 
    });
    fetchReadings();
    setInterval(fetchReadings, 3000);
}