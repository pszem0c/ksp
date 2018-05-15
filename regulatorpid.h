#ifndef REGULATORPID_H
#define REGULATORPID_H
#include "regulator.h"
class RegulatorPID : public Regulator {
private:
    double p;
    double i;
    double d;
    double h;
    double n;
    double integralPart;
    double derivativePart;
    double previousInput;
public:
    /**
     * @brief RegulatorPID
     */
    RegulatorPID();

    /**
     * @brief RegulatorPID
     * Tworzy obiekt regulatora PID z podanymi nastawami.
     * W przypadku podania nieprawidłowej wartości nastawy części regulatora, wartość dobierana jest w taki sposób, aby wyłączyć daną część.
     * @param _p
     * @param _i
     * @param _d
     * @param _h
     * @param _n
     */
    RegulatorPID(double _p, double _i, double _d, double _h, double _n);
    ~RegulatorPID();

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
#endif // REGULATORPID_H
