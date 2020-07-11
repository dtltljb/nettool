#include  "pseudocolor.h"
#include "pressuredconvert.h"


pressuredConvert::pressuredConvert()
{

}

/*
 * return value: color card index
*/
uint16_t pressuredConvert::getPressuredColor(uint16_t ADCValue)
{
    uint16_t    index;
    index = uint32_t( colorCardSize * ADCValue ) / ADC_MAX_Size;
    return index;
}
/*
 * return value: xx g
*/
uint32_t pressuredConvert::getPressuredValue(uint16_t ADCValue)
{
    uint32_t    g;
    g = uint32_t( pressureMaxSize * ADCValue ) / ADC_MAX_Size;
    return g;
}
