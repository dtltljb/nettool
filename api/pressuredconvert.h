#ifndef PRESSUREDCONVERT_H
#define PRESSUREDCONVERT_H

#include <QObject>
#include <QByteArray>

#define     ADC_MAX_Size        4095    //12 bit
#define     pressureMaxSize     10000   //10kg

class pressuredConvert
{
public:
    pressuredConvert();
   uint16_t getPressuredColor(uint16_t ADCValue);
   uint32_t getPressuredValue(uint16_t ADCValue);
};

#endif // PRESSUREDCONVERT_H
