#include "pseudocolor.h"

pseudocolor::pseudocolor(QObject *parent) : QObject(parent)
{

    uint8_t i;
    RGB.clear();
    //part.1    RED
     uint8_t R=255,G=0,B=0;
    //part.2 B= 0->255
    for(i=0;i < 255;i++)
    {
        RGB.append(B++);          //BLUE
        RGB.append(G);
        RGB.append(R);
    }
    //part.3  R = 255 -> 0
    //R=255;G=0;B=255;
    for(i=255;i > 0;i--)
    {
        RGB.append(B);
        RGB.append(G);
        RGB.append(R--);        //RED
    }
    //part.4 G = 0 -> 255
    //R=0;G=0;B=255;
    for(i=0;i < 255;i++)
    {
        RGB.append(B);
        RGB.append(G++);      //GREEN
        RGB.append(R);
    }
    //part.5 B = 255 -> 0
    //R=0;G=255;B=255;
    for(i=255;i > 0;i--)
    {
        RGB.append(B--);        //BLUE
        RGB.append(G);
        RGB.append(R);
    }
    //part.6 R = 0 -> 255
    //R=0;G=255;B=0;
    for(i=0;i < 255;i++)
    {
        RGB.append(B);
        RGB.append(G);
        RGB.append(R++);
    }
    //part.7 B = 0->128,R&G = 255 -> 128
    //R=255;G=255;B=0;
    for(i=0;i < 128;i++)
    {
        RGB.append(B++);
        RGB.append(G--);
        RGB.append(R--);
    }
    //part.8 R&G&B = 128 -> 0
    //R=127;G=127;B=127;
    for(i=0;i < 128;i++)
    {
        RGB.append(B--);
        RGB.append(G--);
        RGB.append(R--);
    }
    //R=0;G=0;B=0;          black
}

QByteArray pseudocolor::getColorCard()
{
    return  RGB;
}

QByteArray  pseudocolor::getColorCard(uint16_t index)
{
    uint16_t t = colorCardSize,base;
    QByteArray  ba;
    if( index < t )
    {
        base = (t - index)*3;
        ba.append( RGB.at(base) );           /* blue   */
        ba.append(RGB.at(base + 1));         /* green */
        ba.append(RGB.at(base + 2));         /* red  */
        return ba;
    }
    else{
       ba.clear();
       return ba;
    }
}
