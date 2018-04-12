'use strict';
// test lib for now, must change to actual lib
const SerialPort = require('serialport/test');
const io = require('socket.io')();

// test port for now, must change to actual port
const portPath = 'COM_TEST';
const MockBinding = SerialPort.Binding;

// creates the mock binding, set behaviour to emulate an Arduino echo device
MockBinding.createPort(portPath, { echo: true, record: false });

// open the serial port
const serialPort = new SerialPort(portPath);
serialPort.on('open', () => {
  console.log('Port opened:\t', serialPort.path);
});

// store the client id to send a specific response
let clientId = '';

// sets the socket connection to send responses from the serial port to the React display
io.on('connection', client => {
  // set the client id
  clientId = client.id;
  console.log('id', clientId);
  // let the client start the test by sending a 'start' event
  client.on('start', () => {
    startTest();
  });
});

// listen for data coming from the mock serial port and send it to the React display
serialPort.on('data', data => {
  console.log('Received:\t', data.toString());
  // send to all connections
  io.sockets.emit('test', data.toString());
  // send to a specific client
  // io.to(clientId).emit('test', data.toString());
  // note: can add encoding as an arg to .toString
});

// writes data to the mock port; the port echoes data back and can be received by the server
const sendRandomNumbers = () => {
  let x = 0;
  const intervalId = setInterval(() => {
    x++;
    x > 50 ? clearInterval(intervalId) : null;
    const random = Math.floor(Math.random() * 10);
    const message = Buffer.from(`random number: ${random}`);
    serialPort.write(message, () => console.log('Wrote random number.'));
  }, 100);
};

// wait 500ms for the port to open and then start the test
const startTest = () => {
  setTimeout(() => {
    console.log('Sending test data in 3s...');
    setTimeout(sendRandomNumbers, 3000);
  }, 500);
};

// destroys the serial port; not sure if this is required
// MockBinding.reset();

// open the socket and listen for the client
const IOport = 8000;
io.listen(IOport);
console.log('Listening for React client on port', IOport);
