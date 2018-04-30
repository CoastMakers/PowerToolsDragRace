import React from 'react';

const Banner = props => {
  return (
    <div className="banner-container">
      <h1 className="header">
        Sunshine Coast Maker Faire Power Tools Drag Race
      </h1>
      <div className="start-button-wrapper">
        <button className="start-button" onClick={props.onClick}>
          Start
        </button>
      </div>
    </div>
  );
};

export { Banner };
