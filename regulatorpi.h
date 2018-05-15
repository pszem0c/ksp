#ifndef REGULATORPI_H
#define REGULATORPI_H
#include "regulator.h"

class RegulatorPI : public Regulator {
private:
    double p;
    double i;
    double h;
    double integralPart;
public:

    /**
     * @brief RegulatorPI
     */
    RegulatorPI();

    /**
     * @brief RegulatorPI
     * @param _p
     * @param _i
     * @param _h
     */
    RegulatorPI(double _p, double _i, double _h);
    ~RegulatorPI();

    void setSetPoint(std::shared_ptr<SignalGenerator> _pSignal);
    void setSettings(std::map<std::string,double> _Settings);
    double getError() const;
    double getSetPoint() const;
    double getOutput() const;
    std::vector<double> getErrorVector() const;
    std::vector<double> getSetPointVector() const;
    std::vector<double> getOutputVector() const;
    double simulate(double _input);
    void addChild(SISOSystem* _pSys);
    void reset();
    void prepare();
};

#endif // REGULATORPI_H
