#include "regulatorpid.h"
#include "signalgenerator.h"
#include <limits>
RegulatorPID::RegulatorPID() {
    p = 0;
    i = 1;
    d = 0;
    h = 1;
    n = 1;
    prepare();
}

RegulatorPID::RegulatorPID(double _p, double _i, double _d, double _h, double _n) {
    p = _p;
    i = _i;
    d = _d;
    h = _h;
    n = _n;

    if (i <= 0) {
        i = std::numeric_limits<double>::max();
    }
    if (h <= 0) {
        h = 1;
    }
    if (d <= 0) {
        d = 0;
    }
    if (n <= 0) {
        n = std::numeric_limits<double>::max();
    }

    prepare();
}

void RegulatorPID::prepare() {
    previousInput = 0;
    derivativePart = 0;
    integralPart = 0;
    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}

RegulatorPID::~RegulatorPID() {

}

void RegulatorPID::setSetPoint(std::shared_ptr<SignalGenerator> _pSignal) {
    setPoint = _pSignal;
}

void RegulatorPID::setSettings(std::map<std::string, double> _settings) {
    if (_settings.find("P") != _settings.end()
            && _settings.find("I") != _settings.end()
            && _settings.find("D") != _settings.end()
            && _settings.find("h") != _settings.end()
            && _settings.find("n") != _settings.end()) {
        p = _settings["P"];
        i = _settings["I"];
        d = _settings["D"];
        h = _settings["h"];
        n = _settings["n"];

        if (i <= 0) {
            i = std::numeric_limits<double>::max();
        }
        if (h <= 0) {
            h = 1;
        }
        if (d <= 0) {
            d = 0;
        }
        if (n <= 0) {
            n = std::numeric_limits<double>::max();
        }
    } else {
        throw std::runtime_error("RegulatorPI: wrong settings");
    }
}

double RegulatorPID::getError() const {
    return errorVector.back();
}

double RegulatorPID::getSetPoint() const {
    return setPointVector.back();
}

double RegulatorPID::getOutput() const {
    return outputVector.back();
}

std::vector<double> RegulatorPID::getErrorVector() const {
    return errorVector;
}

std::vector<double> RegulatorPID::getSetPointVector() const {
    return setPointVector;
}

std::vector<double> RegulatorPID::getOutputVector() const {
    return outputVector;
}

double RegulatorPID::simulate(double _input) {
    setPointVector.push_back(setPoint->generate());                       // wygenerowanie SP i zapisanie wartosci do wektora SP
    errorVector.push_back(setPointVector.back() - _input);                // obliczenie uchybu

    double a = d/(d+n*h);                                                 // obliczenie współczynników do obliczenia części różniczkującej z filtracją
    double b = -p*d*n/(d+n*h);
    derivativePart = a*derivativePart + b*(_input - previousInput);         // obliczenie części różniczkującej
    previousInput = _input;

    integralPart += p*errorVector.back()*h/i;                                // obliczenie części całkującej

    outputVector.push_back(p * errorVector.back() + integralPart  + derivativePart); // obliczenie sterowania

    return outputVector.back();
}

void RegulatorPID::addChild(SISOSystem *) {
    throw std::runtime_error("Cannot add child!");
}

void RegulatorPID::reset() {
    previousInput = 0;
    derivativePart = 0;
    integralPart = 0;
    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}


