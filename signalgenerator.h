#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H
#include <random>
#include "signalgeneratorinterface.h"

/**
 * @brief The SignalConstant class
 * Generuje zadaną stałą wartość.
 */
class SignalConstant: public SignalGenerator {
private:
    double value;
public:
    /**
     * @brief SignalConstant
     * Generuje 0;
     */
    SignalConstant();

    /**
     * @brief SignalConstant
     * @param _value
     */
    SignalConstant(double _value);
    ~SignalConstant();
    double generate();
};

/**
 * @brief The SignalDecorator class
 * Klasa do obsługi wzorca projektowego Dekorator.
 */
class SignalDecorator: public SignalGenerator {
protected:
    SignalGenerator* signal;
public:
    /**
     * @brief SignalDecorator
     * @param _pSignal
     */
    SignalDecorator(SignalGenerator* _pSignal);
    ~SignalDecorator();
    double generate();
};
/**
 * @brief The SignalStep class
 * Generuje skok Heaviside'a o zadanej wartości i opóźnieniu.
 */
class SignalStep: public SignalDecorator {
private:
    double value;
    unsigned long delay;
    unsigned long step;
public:
    /**
     * @brief SignalStep
     * @param _value
     * @param _delay
     * @param _pSignal
     */
    SignalStep(double _value, unsigned long _delay, SignalGenerator* _pSignal);
    ~SignalStep();
    double generate();
};

/**
 * @brief The SignalDiracDelta class
 * Generuje deltę Diraca o zadanej wartości i opóźnieniu.
 */
class SignalDiracDelta: public SignalDecorator {
private:
    double value;
    unsigned long delay;
    unsigned long step;
public:
    /**
     * @brief SignalDiracDelta
     * @param _value
     * @param _delay
     * @param _pSignal
     */
    SignalDiracDelta(double _value, unsigned long _delay, SignalGenerator* _pSignal);
    ~SignalDiracDelta();
    double generate();
};

/**
 * @brief The SignalRectangular class
 * Generuje sygnał prostokątny o zadanej amplitudzie, okresie i wypełnieniu.
 */
class SignalRectangular: public SignalDecorator {
private:
    double amplitude;
    unsigned int period;
    double dutyCycle;
    unsigned long step;
public:
    /**
     * @brief SignalRectangular
     * @param _amplitude
     * @param _period
     * @param _dutycycle
     * @param _pSignal
     */
    SignalRectangular(double _amplitude, unsigned int _period, double _dutycycle, SignalGenerator* _pSignal);
    ~SignalRectangular();
    double generate();
};

/**
 * @brief The SignalSinusoidal class
 * Generuje sygnał sinusoidalny o zadanej amplitudzie i okresie.
 */
class SignalSinusoidal: public SignalDecorator {
private:
    double amplitude;
    unsigned int period;
    unsigned long step;
public:
    /**
     * @brief SignalSinusoidal
     * @param _amplitude
     * @param _period
     * @param _pSignal
     */
    SignalSinusoidal(double _amplitude, unsigned int _period, SignalGenerator* _pSignal);
    ~SignalSinusoidal();
    double generate();
};


/**
 * @brief The SignalTriangular class
 * Generuje sygnał trójkątny o zadanej amplitudzie i okresie.
 */
class SignalTriangular: public SignalDecorator {
private:
    double amplitude;
    unsigned int period;
    unsigned long step;
public:
    /**
     * @brief SignalTriangular
     * @param _amplitude
     * @param _period
     * @param _pSignal
     */
    SignalTriangular(double _amplitude, unsigned int _period, SignalGenerator* _pSignal);
    ~SignalTriangular();
    double generate();
};

/**
 * @brief The SignalWhiteNoise class
 * Generuje biały szum o zadyn odchyleniu standardowym.
 */
class SignalWhiteNoise: public SignalDecorator {
private:
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
public:
    /**
     * @brief SignalWhiteNoise
     * @param _stddev
     * @param _pSignal
     */
    SignalWhiteNoise(double _stddev, SignalGenerator* _pSignal);
    ~SignalWhiteNoise();
    double generate();
};

#endif // SIGNALGENERATOR_H

