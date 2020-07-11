
#include    "bitmappic.h"
#include    <QDate>
#include    <QTime>
#include    <QDateTime>
#include    <QCoreApplication>
#include    <QStandardPaths>

BitmapClass::BitmapClass()
{

}
BitmapClass::~BitmapClass()
{

}

quint32 BitmapClass::DWORDtoQuint32(DWORD n)
{
    quint32 r = 0x00000000, temp = 0x00000000;
    temp = n >> 24; //取出第一个字节
    r = r + temp;
    temp = (n & (0x00ff0000)) >> 8; //取出第二个字节
    r = r + temp;
    temp = (n & (0x0000ff00)) << 8; //取出第三个字节
    r = r + temp;
    temp = (n & (0x000000ff)) << 24; //取出第四个字节
    r = r + temp;
    return r;
}

quint16 BitmapClass::WORDtoQuint16(WORD n)/* wait for repair code */
{
    quint32 r = 0x00000000, temp = 0x00000000;
    temp = n >> 24; //取出第一个字节
    r = r + temp;
    temp = (n & (0x00ff0000)) >> 8; //取出第二个字节
    r = r + temp;

    temp = (n & (0x0000ff00)) << 8; //取出第三个字节
    r = r + temp;
    temp = (n & (0x000000ff)) << 24; //取出第四个字节
    r = r + temp;
    return r;
}

qint32 BitmapClass::LONGtoQint32(LONG n)
{
    qint32 r = 0x00000000, temp = 0x00000000;
    temp = n >> 24; //取出第一个字节
    temp = temp & 0x000000ff; //为了实现逻辑右移
    r = r + temp;
    temp = (n & (0x00ff0000)) >> 8; //取出第二个字节
    temp = temp & 0x00ffffff; //实现右移的逻辑化
    r = r + temp;
    temp = (n & (0x0000ff00)) << 8; //取出第三个字节
    r = r + temp;
    temp = (n & (0x000000ff)) << 24; //取出第四个字节
    r = r + temp;
    return r;
}

QString BitmapClass::ReadBitmapFile(QString filename)
{
    QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
        {
            return "File can not be opened!";
        }
        else
        {
            QDataStream in(&file); //创建输入流
            in.setVersion(QDataStream::Qt_4_6);

            /*开始读取文件头信息*/
            /*===================================*/
            in >> BitMapFileHeader.bfType; //读取文件类型
            in >> BitMapFileHeader.bfSize; //读取文件大小
            in >> BitMapFileHeader.bfReserved1; //读取两个保留字
            in >> BitMapFileHeader.bfReserved2;
            in >> BitMapFileHeader.bfOffBits; //读取偏移量
            /*===================================*/
            /**文件头信息读取结束*/

            /*开始读取位图信息头*/
            /*====================================*/
            in >> BitMapInfoHeader.biSize;
            in >> BitMapInfoHeader.biWidth;
            in >> BitMapInfoHeader.biHeight;
            in >> BitMapInfoHeader.biPlanes;
            in >> BitMapInfoHeader.biBitCount;
            in >> BitMapInfoHeader.biCompression;
            in >> BitMapInfoHeader.biSizeImage;
            in >> BitMapInfoHeader.biXPelsPerMeter;
            in >> BitMapInfoHeader.biYPelsPerMeter;
            in >> BitMapInfoHeader.biClrUsed;
            in >> BitMapInfoHeader.biClrImportant;
            /*====================================*/
            /*位图信息头读取结束*/

            /*开始读取颜色表*/
            /*====================================*/
            //获得图像的颜色位数
            format = WORDtoQuint16(BitMapInfoHeader.biBitCount);
            if (format == 1) //对于黑白图
            {
                pColorTable = new RGBQUAD[2];
                //读入颜色表
                for (int i = 0; i < 2; i++)
                {
                    in >> pColorTable[i].rgbBlue;
                    in >> pColorTable[i].rgbGreen;
                    in >> pColorTable[i].rgbRed;
                    //in >> pColorTable[i].rgbReserved;
                }
            }
            if (format == 8) //对于灰度图，共有256种颜色
            {
                pColorTable = new RGBQUAD[256];
                //读入颜色表
                for (int i = 0; i < 256; i++)
                {
                    in >> pColorTable[i].rgbBlue;
                    in >> pColorTable[i].rgbGreen;
                    in >> pColorTable[i].rgbRed;
                   // in >> pColorTable[i].rgbReserved;
                }
            }
            //24位真彩色图没有颜色表
            /*====================================*/
            /*颜色表读取结束*/

            /*开始读取位图数据*/
            /*====================================*/

            //求得图像数据的字节数
            quint32 length = DWORDtoQuint32(BitMapFileHeader.bfSize)
                    - DWORDtoQuint32(BitMapFileHeader.bfOffBits);
            bmpData = new BYTE[length];
            for (quint32 i = 0; i < length; i++)
            {
                in >> bmpData[i];
            }
            /*====================================*/
            /*位图数据读取结束*/

            fileSize = DWORDtoQuint32(BitMapFileHeader.bfSize);
            bitCount = WORDtoQuint16(BitMapInfoHeader.biBitCount);
            dataSize = DWORDtoQuint32(BitMapFileHeader.bfSize)
                    - DWORDtoQuint32(BitMapFileHeader.bfOffBits);
            width = LONGtoQint32(BitMapInfoHeader.biWidth);
            height = LONGtoQint32(BitMapInfoHeader.biHeight);
            flag = true;

        }
        file.close();
        return "Success";
}

/*
 * input para:  int width, int height,
 *              unsigned char *pData:data context,
 *              int nDatasize: data  size
 * output para: fix path output *.bmp file
*/
void BitmapClass::saveBitmapFile(int w, int h,unsigned char *pData)
{

    // Define BMP Size
    const int height = w;
    const int width = h;
    //const int size = nDatasize;
    const int size = width * height * 3 ;
    double x, y;
    int index;

    // Part.1 Create Bitmap File Header
    BITMAPFILEHEADER fileHeader = { 0 };

    fileHeader.bfType = 0x4D42;//!'BM'
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Part.2 Create Bitmap Info Header
    BITMAPINFOHEADER bitmapHeader = { 0 };

    bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
   // bitmapHeader.biHeight = -height;
    bitmapHeader.biHeight = height;
    bitmapHeader.biWidth = width;
    bitmapHeader.biPlanes = 1;
    bitmapHeader.biBitCount = 24;
    bitmapHeader.biSizeImage = size;
    bitmapHeader.biCompression = 0; //BI_RGB

    //Part.3 get default file path & name
    QByteArray ba = filePathName.toLocal8Bit();
    char *fpn=ba.data();
    // Write to file
    FILE *output = fopen(fpn, "wb");

    if (output == NULL)
    {
        printf("Cannot open file!\n");
    }
    else
    {
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);
        fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);
        fwrite(pData, size, 1, output);
        fclose(output);
    }
}

QString BitmapClass::getPathName( )
{
    return filePathName;
}

void BitmapClass::setPathName(QString &fPN)
{
    filePathName = fPN;
}


/*
 *  @bref : test save bmp file
*/
void BitmapClass::saveBitMap()
{
    // Define BMP Size
    const int height = Max_y;
    const int width = Max_x;
    const int size = height * width * 3;
    double x, y;
    int index;

    // Part.1 Create Bitmap File Header
    BITMAPFILEHEADER fileHeader;

    fileHeader.bfType = 0x4D42/*0x424D*/;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Part.2 Create Bitmap Info Header
    BITMAPINFOHEADER bitmapHeader = { 0 };

    bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapHeader.biHeight = height;
    bitmapHeader.biWidth = width;
    bitmapHeader.biPlanes = 1;//3;              /* =3 image don't open, =1 open okay */
    bitmapHeader.biBitCount = 24;
    bitmapHeader.biSizeImage = size;
    bitmapHeader.biCompression = 0; //BI_RGB

    // Part.3 Create Data
    BYTE *bits = (BYTE *)malloc(size);

    // Clear
    memset(bits, 0x00, size);  //0xff = white,0x00 = black

    // Sin Graph
    for (x = 0; x < Max_x; x += 0.5)
    {
        y = sin(x / 100.0) * 100 + Max_x/3;
        index = (int)y * Max_x * 3 + (int)x * 3;

        bits[index + 0] = 100;  // Blue 255
        bits[index + 1] = 100;   // Green
        bits[index + 2] = 200;   // Red
    }
    //part.4 create forlder
    QDir    dir;
    QString currentPath = QCoreApplication::applicationDirPath();
    QDateTime datetime =QDateTime::currentDateTime();
    QString str = datetime.toString("yyyy-MM-dd");  //
    dir.mkdir(str);
    str = datetime.toString("hh:mm:ss.zzz");
    str.append(".bmp");
    char *fp;
    QByteArray ba = str.toLatin1();
    fp = ba.data();
    // Write to file

    FILE *output = fopen(fp, "wb");

    if (output == NULL)
    {
        printf("Cannot open file!\n");
    }
    else
    {
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);
        fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);
        fwrite(bits, size, 1, output);
        fclose(output);
    }
}
