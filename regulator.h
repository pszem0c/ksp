#ifndef REGULATOR_H
#define REGULATOR_H
#include "SISOSystem.h"
#include "signalgeneratorinterface.h"
#include <map>
#include <memory>
#include <string>


/**
 * @brief The Regulator class
 * Interfejs klasy regulatora.
 */
class Regulator : public SISOSystem {
private:
protected:
    std::shared_ptr<SignalGenerator> setPoint;
    std::vector<double> setPointVector;
    std::vector<double> outputVector;
    std::vector<double> errorVector;
public:
    virtual ~Regulator() {}


    /**
     * @brief setSetPoint
     * Metoda służy do zmiany wartości jak i typu wartości zadanej
     * @param _pSignal
     */
    virtual void setSetPoint(std::shared_ptr<SignalGenerator> _pSignal)=0;

    /**
     * @brief setSettings
     * Metoda służaca do zmiany parametrów regulatora.
     * @param _settings
     * @throw runtime_error
     */
    virtual void setSettings(std::map<std::string,double> _settings)=0;

    /**
     * @brief getError
     * Zwraca uchyb.
     * @return
     */
    virtual double getError() const=0;

    /**
     * @brief getSetPoint
     * Zwarca wartość zadaną
     * @return
     */
    virtual double getSetPoint() const=0;

    /**
     * @brief getErrorVector
     * Zwraca wektor uchybów.
     * @return
     */
    virtual std::vector<double> getErrorVector() const=0;

    /**
     * @brief getSetPointVector
     * Zwraca wektor wartości zadanych.
     * @return
     */
    virtual std::vector<double> getSetPointVector() const=0;
};

#endif // REGULATOR_H
