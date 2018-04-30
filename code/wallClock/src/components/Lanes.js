import React from 'react';

import Lane from './Lane';

const Lanes = ({ raceData, lanesFinished, winner }) => {
  return (
    <div className="lanes">
      <Lane
        lane="1"
        raceData={raceData}
        lanesFinished={lanesFinished}
        winner={winner}
      />
      <Lane
        lane="2"
        raceData={raceData}
        lanesFinished={lanesFinished}
        // winner={winner}
      />
    </div>
  );
};

export { Lanes };
