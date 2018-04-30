import React, { Component } from 'react';

import { testSocket } from './api';
import { Banner, Timer, Lanes } from './components';

class App extends Component {
  state = {
    raceData: 'waiting for race data',
    elapsedTime: 0,
    startDisabled: false,
    winner: false,
    lanesFinished: 0
  };

  onClick = () => {
    // disable the start button
    if (this.state.startDisabled) {
      return;
    }

    // start the race
    this.setState({
      startDisabled: true,
      raceData: 'race underway',
      lanesFinished: 0,
      winner: false
    });

    // start the clock
    testSocket(raceData => {
      // this.setState({ raceData, lanesFinished: this.state.lanesFinished + 1 });
      const finishedCount = raceData.split('- ')[2];

      this.setState({ raceData, lanesFinished: finishedCount });

      // stop the clock if both lanes have come in
      if (finishedCount > 1) {
        clearInterval(raceTimer);
        this.setState({ startDisabled: false });
      }
    });

    let time = 0;
    const raceTimer = setInterval(() => {
      this.setState({ elapsedTime: ++time });
    }, 100);
  };

  render() {
    const { raceData, lanesFinished } = this.state;
    return (
      <div className="app">
        <Banner onClick={this.onClick} />
        <div className="display-container">
          <Lanes raceData={raceData} lanesFinished={lanesFinished} />
          <Timer
            time={Number.parseFloat(this.state.elapsedTime / 10).toFixed(1)}
          />
        </div>
      </div>
    );
  }
}

export default App;
