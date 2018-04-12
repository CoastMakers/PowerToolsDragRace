import openSocket from 'socket.io-client';
const socket = openSocket('http://localhost:8000');

function testSocket(callback) {
  // when receiving a 'test' event from the socket, run the callback on the data received
  socket.on('test', random => callback(random));
  // when the function is called, send a 'start' event back to the server
  socket.emit('start');
}

export { testSocket };
