#pragma once
#include <vector>

/**
 * @brief The SISOSystem class
 * Klasa abstrakcyjna przedstawiająca układ typu SISO.
 */
class SISOSystem {
    private:
    public:
        virtual ~SISOSystem() {}

        /**
         * @brief Simulate
         * Funkcja wykonująca jeden krok symulacji na podane wymuszenie.
         * @param input
         * @return double
         * @throw std::exception
         */
        virtual double simulate(double _input)=0;

        /**
         * @brief addChild
         * @param pSys
         */
        virtual void addChild(SISOSystem* _pSys) = 0;

        /**
         * @brief Restart
         * Resetuje symulację.
         */
        virtual void reset()=0;

        /**
         * @brief getOutput
         * Zwraca ostatnią próbkę wyjścia.
         * @return
         */
        virtual double getOutput() const=0;

        /**
         * @brief getOutputVector
         * Zwraca wektor sterowań.
         * @return
         */
        virtual std::vector<double> getOutputVector() const=0;

         /**
          * @brief prepare
          * Wypełnienia obiekt danymi
          */
        virtual void prepare()=0;
};
