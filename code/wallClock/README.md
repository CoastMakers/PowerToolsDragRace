#CoastMakers Tool Timer

A Node.js server using serialport and socket.io to send race results to a React display.

Currently, the app is configured to run the serialport portion in test mode.

Installation instructions:

You must have npm installed in order to install the dependencies.

Fork/Clone the repository, and then from the project root run:

```
npm install
```

Running the app:

You will need two terminal windows both open to the root of the project.

To start the React portion of the app, in one terminal run:

```
npm start
```

The server can be started by running:

```
node server
```

Clicking 'Start' in the browser window signals the test module to write data to the serial port. The test port then "echoes" the data back so it can be picked up by the server and transmitted to the client via the socket.

Road Map:

I would like to get a physical device that transmits an example of the data in its final format so I can build out the API to parse the results, and send them to different view elements in the React app.
