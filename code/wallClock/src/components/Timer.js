import React from 'react';

const Timer = props => {
  return (
    <div className="timer">
      <h1 className="text">{props.time}</h1>
    </div>
  );
};

export { Timer };
