#ifndef IOREADER_H
#define IOREADER_H

#include <QAbstractSocket>
#include "../../basictype.h"

namespace Network
{

static const bool IS_BIG_ENDIAN = (char)((short) 0x0061) != 'a';

class IOReader
{
public:
    
    IOReader(QAbstractSocket *device);
    ~IOReader();

    char read();
    int readInt();
    float readFloat();
    double readDouble();
    int64 readLong();
    int64 read(char *chars, int64 size);
    char* readString();

    void write(char b);
    void writeInt(int i);
    void writeFloat(float f);
    void writeDouble(double d);
    void writeLong(int64 i);
    void write(const char *chars, int64 size);
    void writeString(const char *chars);

    void flush();
    bool isClosed();
    int64 bytesAvailable() const;

    QAbstractSocket* socket();

private:
    void exchangeBytes(char *chars, int start, int end);
    void bigEndianToCurrentEndian(char *chars, int start, int end);
    void currentEndianToBigEndian(char *chars, int start, int end);

private:
    QAbstractSocket *device;
    bool closed = false;
};

}

#endif // IOREADER_H