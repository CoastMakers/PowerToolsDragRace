import React, { Component } from 'react';

class Lane extends Component {
  state = {
    raceData: ''
  };

  shouldComponentUpdate(nextProps, nextState) {
    const { raceData, lane } = nextProps;
    const finishingLane = raceData.slice(0, 1);
    if (raceData !== this.state.raceData) {
      if (lane === finishingLane || finishingLane.match(/[a-z]/)) {
        return true;
      }
    }
    return false;
  }

  componentWillUpdate() {
    const { raceData } = this.props;
    this.setState({ raceData });
  }

  renderRaceData = () => {
    const { raceData, lanesFinished } = this.props;
    let className = 'lane-content';
    if (lanesFinished === '1') {
      className = 'winner';
    } else if (lanesFinished === '2') {
      className = 'loser';
    }
    return (
      <div className={className}>
        <p>{raceData}</p>
      </div>
    );
  };

  render() {
    return <div className="lane">{this.renderRaceData()}</div>;
  }
}

export default Lane;
