import React, { Component } from 'react';
import './App.css';
import { testSocket } from './api';

class App extends Component {
  state = {
    random: 'waiting for random number'
  };

  onClick = () => {
    testSocket(random => {
      this.setState({ random });
    });
  };

  render() {
    return (
      <div className="app">
        <p className="app-intro">Test data: {this.state.random}</p>
        <button onClick={this.onClick}>Start</button>
      </div>
    );
  }
}

export default App;
