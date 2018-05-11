import React, { Component } from 'react';

import { testSocket } from './api';
import { Timer, Lanes } from './components';

class App extends Component {
  state = {
    raceData: '0.00',
    raceTimer: 0,
    elapsedTime: 0,
    lanesFinished: 0
  };

  componentWillMount() {
    // start the race
    // set initial race condition
    this.setState({
      lanesFinished: 0
    });

    // start the clock
    testSocket(raceData => {
      // setup the reset runction here
      if (raceData === 'SETUP') {
        console.log('setup');
        this.setState({
          lanesFinished: 0,
          elapsedTime: 0,
          raceTimer: 0,
          raceData: '🏎️'
        });
        console.log('state', this.state);
      }

      // check for start/stop signal
      let time = 0;
      if (raceData === 'START') {
        const raceTimer = setInterval(() => {
          this.setState({ elapsedTime: ++time });
        }, 10);
        this.setState({ raceTimer });
      } else if (raceData === 'STOP') {
        clearInterval(this.state.raceTimer);
      }

      // record lane data
      const finishedCount = raceData.split('- ')[2];
      this.setState({ raceData, lanesFinished: finishedCount });

      // stop the clock if both lanes have come in
      if (finishedCount > 1) {
        clearInterval(this.state.raceTimer);
      }
    });
  }

  // onClick = () => {
  //   // disable the start button
  //   if (this.state.startDisabled) {
  //     return;
  //   }
  //
  //   // start the race
  //   this.setState({
  //     startDisabled: true,
  //     raceData: 'race underway',
  //     lanesFinished: 0,
  //     winner: false
  //   });
  //
  //   // start the clock
  //   testSocket(raceData => {
  //     // this.setState({ raceData, lanesFinished: this.state.lanesFinished + 1 });
  //     const finishedCount = raceData.split('- ')[2];
  //
  //     this.setState({ raceData, lanesFinished: finishedCount });
  //
  //     // stop the clock if both lanes have come in
  //     if (finishedCount > 1) {
  //       clearInterval(raceTimer);
  //       this.setState({ startDisabled: false });
  //     }
  //   });
  //
  //   let time = 0;
  //   const raceTimer = setInterval(() => {
  //     this.setState({ elapsedTime: ++time });
  //   }, 100);
  // };

  render() {
    const { raceData, lanesFinished } = this.state;
    return (
      <div className="app">
        <div className="display-container">
          <Timer
            time={Number.parseFloat(this.state.elapsedTime / 100).toFixed(2)}
          />
          <Lanes raceData={raceData} lanesFinished={lanesFinished} />
        </div>
      </div>
    );
  }
}

export default App;
