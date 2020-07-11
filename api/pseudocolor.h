#ifndef PSEUDOCOLOR_H
#define PSEUDOCOLOR_H

#include <QObject>
#include <QByteArray>

/*
 * (1).  <<  >> 
 * (2). 7
 * (3). : 0  1530 0  409515304095
 * (4). y = 1530 / 4095 * Values
 * (5). 7,
*/

#define     colorCardSize       1530    //6 * 255 = 1530
#define     colorCardArraySize  colorCardSize*3 //RGB

class pseudocolor : public QObject
{
    Q_OBJECT
public:
    explicit pseudocolor(QObject *parent = 0);
    QByteArray  getColorCard();                     /* get mothed.return array[ 1530*3 ] */
    QByteArray  getColorCard(uint16_t index);       /* get values, by index  rang:0~1530*/
signals:

public slots:

private:
    QByteArray  RGB;            /*color card byte array*/
};

#endif // PSEUDOCOLOR_H
