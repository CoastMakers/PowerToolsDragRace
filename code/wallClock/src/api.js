import openSocket from 'socket.io-client';
const socket = openSocket('http://localhost:8000');

function testSocket(callback) {
  // when receiving a 'test' event from the socket, run the callback on the data received
  // socket.on('test', random => callback(random));

  // add some more events. first, a finishRace event
  socket.on('message', message => callback(message));

  // socket.on('start', startRace => callback(startRace));

  // not sure if we will do the processing on the server, or on the client.
  // these events may not be necessary
  // add a win event
  // socket.on('win', winTime => callback(winTime));
  // and a loss event
  // socket.on('loss', lossTime => callback(lossTime));

  // when the function is called, send a 'start' event back to the server
  // socket.emit('start');
}

export { testSocket };
