#!/usr/bin/env node

'use strict';

// handle command line arguments
// console.log(process.argv);
// console.log('LENGTH', process.argv.length);
// if (process.argv.length !== 3) {
//   console.log(`Usage: node server <serialport>`);
//   process.exit(1);
// }
// const port = process.argv[2];

// parse options section
if (process.argv.length !== 4) {
  console.log('Usage: ./server.js -p <serialport>');
  // console.log('args:', process.argv.length);
  process.exit(1);
}

const optionDefinitions = [{ name: 'port', alias: 'p', type: String }];

const commandLineArgs = require('command-line-args');
const options = commandLineArgs(optionDefinitions);

// test library
// const SerialPort = require('serialport/test');

// connect to serial port and socket
const SerialPort = require('serialport');
const io = require('socket.io')();

// test port for now, must change to actual port
// const portPath = 'COM_TEST';
// const MockBinding = SerialPort.Binding;

// grab port from command line arguments and set the binding
// const portPath = 'COM3';
const portPath = options.port;
// const portPath = port;
const binding = SerialPort.Binding;

// creates the mock binding, set behaviour to emulate an Arduino echo device
// MockBinding.createPort(portPath, { echo: true, record: false });

// binding.createPort(portPath);

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
  // disabled for now
  // client.on('start', () => {
  // this is the original test function
  // startTest();
  // here is the race function
  // startTestRace(7135, 3500);
  // });
});

// to keep track of what place they arrive in
let finishedCount = 0;

// listen for data coming from the mock serial port and send it to the React display
serialPort.on('data', data => {
  // const dataString = data.toString().trim();
  let tie = false;
  const dataStrings = data
    .toString()
    .split('\n')
    .filter(item => item !== '')
    .map(item => item.trim());
  if (dataStrings.length > 1) {
    tie = true;
  }

  dataStrings.forEach(dataString => {
    let message;
    console.log('Received:\t', dataString);

    if (dataString === 'READY') {
      finishedCount--;
    }

    // parse start signal
    if (dataString === 'START' || dataString === 'STOP') {
      message = dataString;
    } else if (dataString === 'SETUP') {
      message = dataString;
      finishedCount = 0;
    } else if (tie === true) {
      message = `${dataString} - T`;
    } else {
      message = `${dataString} - ${++finishedCount}`;
    }

    console.log('Emitting:\t', message);

    // io.sockets.emit('finish', `${data.toString()} - ${++finishedCount}`);
    // console.log(++finishedCount);

    // send to all connections
    io.sockets.emit('message', message);

    // if (finishedCount > 1) {
    //   finishedCount = 0;
    // }

    // send to a specific client
    // io.to(clientId).emit('test', data.toString());
    // note: can add encoding as an arg to .toString
  });
});

// writes data to the mock port; the port echoes data back and can be received by the server
// const sendRandomNumbers = () => {
//   let x = 0;
//   const intervalId = setInterval(() => {
//     x++;
//     x > 50 ? clearInterval(intervalId) : null;
//     const random = Math.floor(Math.random() * 10);
//     const message = Buffer.from(`random number: ${random}`);
//     serialPort.write(message, () => console.log('Wrote random number.'));
//   }, 100);
// };

// hack with global for sending simulated lane data
// let lane = 1;

// writes mock race data to the test port, which is received upon echo
// const sendTestRaceData = finishTime => {
//   console.log(`Sending to mock port: ${finishTime}`);
//   const message = Buffer.from(`${++lane} - ${finishTime}`);
//   serialPort.write(message, () => console.log('Data sent.'));
//
//   // don't let more than two lanes of data come through
//   if (lane > 1) {
//     lane = 0;
//   }
// };

// wait 500ms for the port to open and then start the test
// const startTest = () => {
//   setTimeout(() => {
//     console.log('Sending test data in 3s...');
//     setTimeout(sendRandomNumbers, 3000);
//   }, 500);
// };

// simulate a race by sending (and then echoing back) race time data
// const startTestRace = (winTime, timeBack) => {
//   console.log('Race has started');
//   setTimeout(() => {
//     console.log('Sending win time.');
//     sendTestRaceData(winTime);
//     setTimeout(() => {
//       console.log('Sending lose time.');
//       sendTestRaceData(winTime + timeBack);
//     }, timeBack);
//   }, winTime);
// };

// destroys the serial port; not sure if this is required
// MockBinding.reset();

// open the socket and listen for the client
const IOport = 8000;
io.listen(IOport);
console.log('Listening for React client on port', IOport);
