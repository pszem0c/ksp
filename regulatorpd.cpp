#include "regulatorpd.h"
#include "signalgenerator.h"
#include <limits>
RegulatorPD::RegulatorPD() {
    p = 0;
    d = 0;
    h = 1;
    n = 1;
    prepare();
}

RegulatorPD::RegulatorPD(double _p, double _d, double _h, double _n) {
    p = _p;
    d = _d;
    h = _h;
    n = _n;

    if (d <= 0) {
        d = 0;
    }
    if (h <= 0) {
        h = 1;
    }
    if (n <= 0) {
        n = std::numeric_limits<double>::max();
    }

    prepare();
}

void RegulatorPD::prepare() {
    previousInput = 0;
    derivativePart = 0;

    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}

RegulatorPD::~RegulatorPD() {

}

void RegulatorPD::setSetPoint(std::shared_ptr<SignalGenerator> _pSignal) {
    setPoint = _pSignal;
}

void RegulatorPD::setSettings(std::map<std::string, double> _settings) {
    if (_settings.find("P") != _settings.end() && _settings.find("D") != _settings.end() && _settings.find("h") != _settings.end() && _settings.find("N") != _settings.end()) {
        p = _settings["P"];
        d = _settings["D"];
        h = _settings["h"];
        n = _settings["N"];

        if (d <= 0) {
            d = 0;
        }
        if (h <= 0) {
            h = 1;
        }
        if (n <= 0) {
            n = std::numeric_limits<double>::max();
        }

    }
    else{ throw std::runtime_error("RegulatorPI: wrong settings");}
}

double RegulatorPD::getError() const {
    return errorVector.back();
}

double RegulatorPD::getSetPoint() const {
    return setPointVector.back();
}

double RegulatorPD::getOutput() const {
    return outputVector.back();
}

std::vector<double> RegulatorPD::getErrorVector() const {
    return errorVector;
}

std::vector<double> RegulatorPD::getSetPointVector() const {
    return setPointVector;
}

std::vector<double> RegulatorPD::getOutputVector() const {
    return outputVector;
}

double RegulatorPD::simulate(double _input) {
    setPointVector.push_back(setPoint->generate());                       // wygenerowanie SP i zapisanie wartosci do wektora SP
    errorVector.push_back(setPointVector.back() - _input);                // obliczenie uchybu

    double a = d/(d+n*h);
    double b = -p*d*n/(d+n*h);
    derivativePart = a*derivativePart + b*(_input - previousInput);
    previousInput = _input;

    outputVector.push_back( p * errorVector.back() + derivativePart );

    return outputVector.back();
}

void RegulatorPD::addChild(SISOSystem *) {
    throw std::runtime_error("Cannot add child!");
}

void RegulatorPD::reset() {
    previousInput = 0;
    derivativePart = 0;
    outputVector = std::vector<double>();
    setPointVector = std::vector<double>();
    errorVector = std::vector<double>();
    setPoint = std::shared_ptr<SignalGenerator>(new SignalConstant);
}
