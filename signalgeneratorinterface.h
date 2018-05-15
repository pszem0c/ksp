#ifndef SIGNALGENERATORINTERFACE_H
#define SIGNALGENERATORINTERFACE_H


/**
 * @brief The SignalGenerator class
 * Klasa służąca do generacji sygnałów. Wykorzystuje wzorzec projektowy dekorator do łączenia sygnałów.
 * Klasa pozwala wygenerować następujące sygnały:
 *  - Wartość stała (SignalConst)
 *  - Skok Heaviside'a (SignalStep)
 *  - Delta Diraca (SignalDiracDelta)
 *  - Przebieg prostokątny (SignalRectangular)
 *  - Przebieg sinusoidalny (SignalSinusoidal)
 *  - Przebieg trójkątny (SignalTriangular)
 *  - Biały szum. (SignalWhiteNoise)
 */
class SignalGenerator {
public:
    virtual ~SignalGenerator() {}

    /**
     * @brief generate
     * Generuje jedną próbkę sygnału.
     * @return
     */
    virtual double generate()=0;
};


#endif // SIGNALGENERATORINTERFACE_H

