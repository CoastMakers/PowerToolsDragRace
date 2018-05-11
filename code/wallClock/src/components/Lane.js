import React, { Component } from 'react';

class Lane extends Component {
  state = {
    raceData: '🏎️'
  };

  shouldComponentUpdate(nextProps, nextState) {
    const { raceData, lane } = nextProps;
    const finishingLane = raceData.slice(0, 1);
    // if (raceData === '🏎️') {
    //   return true;
    // }
    if (raceData !== this.state.raceData) {
      if (
        lane === finishingLane ||
        finishingLane.match(/[a-z]/) ||
        raceData === '🏎️'
      ) {
        return true;
      }
    }
    return false;
  }

  componentWillUpdate() {
    let { raceData } = this.props;
    this.setState({ raceData });
  }

  renderRaceData = () => {
    const { raceData, lanesFinished } = this.props;
    let className = 'lane-content';
    if (lanesFinished === '1') {
      className = 'winner';
      return (
        <div className={className}>
          <h1 className="winner-text">WINNER!</h1>
          <h1 className="race-data">{raceData.substr(4, 5)}</h1>
        </div>
      );
    } else if (lanesFinished === '2') {
      className = 'loser';
      return (
        <div className={className}>
          <h1 className="loser-text">RUNNER-UP</h1>
          <h1 className="race-data">{raceData.substr(4, 5)}</h1>
        </div>
      );
    } else {
      return (
        <div className={className}>
          <h1 className={`${this.props.bounce} race-data`}>
            <span role="img" aria-label="racecar">
              {this.state.raceData}️
            </span>
          </h1>
        </div>
      );
    }
  };

  render() {
    return <div className="lane-container">{this.renderRaceData()}</div>;
  }
}

export default Lane;
