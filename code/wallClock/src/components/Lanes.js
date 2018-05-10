import React from 'react';

import Lane from './Lane';

const Lanes = ({ raceData, lanesFinished, winner }) => {
  return (
    <div className="lanes">
      <Lane
        lane="1"
        raceData={raceData}
        lanesFinished={lanesFinished}
        bounce="bounce-1"
      />
      <Lane
        lane="2"
        raceData={raceData}
        lanesFinished={lanesFinished}
        bounce="bounce-2"
      />
    </div>
  );
};

export { Lanes };
