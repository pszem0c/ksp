#include "signalgenerator.h"
#include <cmath>
#include <chrono>
#include <iostream>

////////////// CONSTANT ///////////////////////////
SignalConstant::SignalConstant() {
    value = 0;
}

SignalConstant::SignalConstant(double _value) {
    value = _value;
}

SignalConstant::~SignalConstant() {

}

double SignalConstant::generate() {
    return value;
}
////////////////////////////////////////////////////


//////////// DECORATOR /////////////////////////////
SignalDecorator::SignalDecorator(SignalGenerator *_pSignal) {
    signal = _pSignal;
}

SignalDecorator::~SignalDecorator() {

}

double SignalDecorator::generate() {
    return signal->generate();
}
/////////////////////////////////////////////////////


//////////// STEP //////////////////////////////////
SignalStep::SignalStep(double _value, unsigned long _delay, SignalGenerator *_pSignal):
    SignalDecorator(_pSignal) {
    value = _value;
    delay = _delay;
    step = 0;  
}

SignalStep::~SignalStep() {
    delete signal;
}

double SignalStep::generate() {
    return ( (step++ >= delay) ? value : 0.0 ) + signal->generate();
}
//////////////////////////////////////////////////////

///////////// DIRAC DELTA //////////////////////////////
SignalDiracDelta::SignalDiracDelta(double _value, unsigned long _delay, SignalGenerator *_pSignal):
    SignalDecorator(_pSignal) {
    value = _value;
    delay = _delay;
    step = 0;
}

SignalDiracDelta::~SignalDiracDelta() {
    delete signal;
}

double SignalDiracDelta::generate() {
    return ( (step++ == delay) ? value : 0.0 ) + signal->generate();
}
//////////////////////////////////////////////////////////

//////////// RECTANGULAR /////////////////////////////////
SignalRectangular::SignalRectangular(double _amplitude, unsigned int _period, double _dutycycle, SignalGenerator *_pSignal):
    SignalDecorator(_pSignal) {
    amplitude = _amplitude;
    period = _period;
    dutyCycle = _dutycycle;
    step = 0;

    if (  period == 0 || dutyCycle < 0 || dutyCycle > 1) {
        amplitude = 0;
        period = 1;
        dutyCycle = 0;
    }
}

SignalRectangular::~SignalRectangular() {
    delete signal;
}

double SignalRectangular::generate() {
    return ( ((step++ % period) < ( dutyCycle * period )) ? amplitude : 0 ) + signal->generate();
}
/////////////////////////////////////////////////////////

///////////// SINUSOIDAL ///////////////////////////////
SignalSinusoidal::SignalSinusoidal(double _amplitude, unsigned int _period, SignalGenerator *_pSignal):
    SignalDecorator(_pSignal) {
    amplitude = _amplitude;
    period = _period;
    step = 0;

    if (  period == 0 ) {
        amplitude = 0;
        period = 1;
    }
}

SignalSinusoidal::~SignalSinusoidal() {
    delete signal;
}

double SignalSinusoidal::generate() {
    return amplitude*std::sin((static_cast<double>(step++ % period) / period ) * 6.28) + signal->generate();
}
/////////////////////////////////////////////////////////////

//////////// TRIANGULAR ////////////////////////////////////
SignalTriangular::SignalTriangular(double _amplitude, unsigned int _period, SignalGenerator *pSignal):
    SignalDecorator(pSignal) {
    amplitude = _amplitude;
    period = _period;
    step = 0;

    if (  period == 0 ) {
        amplitude = 0;
        period = 1;
    }
}

SignalTriangular::~SignalTriangular() {
    delete signal;
}

double SignalTriangular::generate() {
    return amplitude*(4.0*fabs(static_cast<double>(step++ % period) - period/2.0)/period -1) + signal->generate();
}
////////////////////////////////////////////////////////////////

/////////// WHITE NOISE ///////////////////////////////////////////
SignalWhiteNoise::SignalWhiteNoise(double _stddev, SignalGenerator *_pSignal):
    SignalDecorator(_pSignal) {
    if ( _stddev < 0 ) {
        _stddev = 0;
    }
    generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    distribution = std::normal_distribution<double>(0,_stddev);
}

SignalWhiteNoise::~SignalWhiteNoise() {
    delete signal;
}

double SignalWhiteNoise::generate() {
    return distribution(generator) + signal->generate();
}
/////////////////////////////////////////////////////////////////////
