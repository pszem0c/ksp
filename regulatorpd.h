#ifndef REGULATORPD_H
#define REGULATORPD_H
#include "regulator.h"
#include "deque"
/**
 * @brief The RegulatorPD class
 */
class RegulatorPD : public Regulator {
private:
    double p;
    double d;
    double h;
    double n;
    double derivativePart;
    double previousInput;
public:
    /**
     * @brief RegulatorPD
     */
    RegulatorPD();
    /**
     * @brief RegulatorPD
     * @param _p
     * @param _d
     * @param _h
     */
    RegulatorPD(double _p, double _d, double _h, double _n);
    ~RegulatorPD();

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

#endif // REGULATORPD_H
