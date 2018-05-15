#include "regulatorpi.h"
#include "signalgenerator.h"
#include <limits>
RegulatorPI::RegulatorPI() {
    p = 0;
    i = 1;
    h = 0;
    prepare();
}

RegulatorPI::RegulatorPI(double _p, double _i, double _h) {
    p = _p;
    i = _i;
    h = _h;

    if (i <= 0) {
        i = std::numeric_limits<double>::max();
    }
    if (h <= 0) {
        h = 1;
    }

    prepare();
}

void RegulatorPI::prepare() {
    integralPart = 0;
    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}

RegulatorPI::~RegulatorPI() {

}

void RegulatorPI::setSetPoint(std::shared_ptr<SignalGenerator> _pSignal) {
    setPoint = _pSignal;
}

void RegulatorPI::setSettings(std::map<std::string, double> _settings) {
    if (_settings.find("P") != _settings.end() && _settings.find("I") != _settings.end() && _settings.find("h") != _settings.end()) {
        p = _settings["P"];
        i = _settings["I"];
        h = _settings["h"];

        if (i <= 0) {
            i = std::numeric_limits<double>::max();
        }
        if (h <= 0) {
            h = 1;
        }
    } else {
        throw std::runtime_error("RegulatorPI: wrong settings");
    }
}

double RegulatorPI::getError() const {
    return errorVector.back();
}

double RegulatorPI::getSetPoint() const {
    return setPointVector.back();
}

double RegulatorPI::getOutput() const {
    return outputVector.back();
}

std::vector<double> RegulatorPI::getErrorVector() const {
    return errorVector;
}

std::vector<double> RegulatorPI::getSetPointVector() const {
    return setPointVector;
}

std::vector<double> RegulatorPI::getOutputVector() const {
    return outputVector;
}

double RegulatorPI::simulate(double _input) {
    setPointVector.push_back(setPoint->generate());                       // wygenerowanie SP i zapisanie wartosci do wektora SP
    errorVector.push_back(setPointVector.back() - _input);                // obliczenie uchybu

    integralPart += errorVector.back()*h/i;
    outputVector.push_back(p * ( errorVector.back() + integralPart ));

    return outputVector.back();
}

void RegulatorPI::addChild(SISOSystem*) {
    throw std::runtime_error("Cannot add child!");
}

void RegulatorPI::reset() {
    integralPart = 0;
    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}


