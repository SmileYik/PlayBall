#include "ioreader.h"
#include <cstdlib>
#include <cstring>

namespace Network
{

#define CHECK_IODEVICE_VALID_READ(RETURN) \
    if (!device->isOpen() || !device->isReadable()) \
    { \
        closed = true; \
        return RETURN; \
    }

#define CHECK_IODEVICE_VALID_WRITE \
    if (!device->isOpen() || !device->isWritable()) \
    { \
        closed = true; \
        return; \
    }

IOReader::IOReader(QAbstractSocket *_device)
    : device(_device)
{
    
}

IOReader::~IOReader()
{
    device->close();
    delete device;
}

char IOReader::read()
{
    CHECK_IODEVICE_VALID_READ(-1);
    while (device->bytesAvailable() < 1);
    char b;
    device->read(&b, 1);
    return b;
}

int IOReader::readInt()
{
    CHECK_IODEVICE_VALID_READ(-1);
    while (device->bytesAvailable() < 4);
    int i = 0;
    char *bytes = (char *) &i;
    device->read(bytes, 4);
    bigEndianToCurrentEndian(bytes, 0, 3);
    return i;
}

float IOReader::readFloat()
{
    CHECK_IODEVICE_VALID_READ(-1);
    while (device->bytesAvailable() < 4);
    float i = 0;
    char *bytes = (char *) &i;
    device->read(bytes, 4);
    bigEndianToCurrentEndian(bytes, 0, 3);
    return i;
}

double IOReader::readDouble()
{
    CHECK_IODEVICE_VALID_READ(-1);
    while (device->bytesAvailable() < 8);
    double i = 0;
    char *bytes = (char *) &i;
    device->read(bytes, 8);
    bigEndianToCurrentEndian(bytes, 0, 7);
    return i;
}

int64 IOReader::readLong()
{
    CHECK_IODEVICE_VALID_READ(-1);
    while (device->bytesAvailable() < 8);
    int64 i = 0;
    char *bytes = (char *) &i;
    device->read(bytes, 8);
    bigEndianToCurrentEndian(bytes, 0, 7);
    return i;
}

int64 IOReader::read(char *bytes, int64 size)
{
    CHECK_IODEVICE_VALID_READ(-1);
    return device->read(bytes, size);
}

char* IOReader::readString()
{
    CHECK_IODEVICE_VALID_READ(nullptr);
    int32 size = readInt();
    char *chars = (char *) malloc(size + 1);
    memset(chars, 0, size + 1);
    read(chars, size);
    return chars;
}

void IOReader::writeString(const char *chars)
{
    CHECK_IODEVICE_VALID_WRITE;
    int32 size = strlen(chars);
    writeInt(size);
    write(chars, size);
}

void IOReader::write(char b)
{
    CHECK_IODEVICE_VALID_WRITE;
    device->write(&b, 1);
}
void IOReader::writeInt(int i)
{
    CHECK_IODEVICE_VALID_WRITE;
    char *bytes = (char *) &i;
    currentEndianToBigEndian(bytes, 0, 3);
    device->write(bytes, 4);
}
void IOReader::writeFloat(float f)
{
    CHECK_IODEVICE_VALID_WRITE;
    char *bytes = (char *) &f;
    currentEndianToBigEndian(bytes, 0, 3);
    device->write(bytes, 4);
}
void IOReader::writeDouble(double d)
{
    CHECK_IODEVICE_VALID_WRITE;
    char *bytes = (char *) &d;
    currentEndianToBigEndian(bytes, 0, 7);
    device->write(bytes, 8);
}
void IOReader::writeLong(int64 i)
{
    CHECK_IODEVICE_VALID_WRITE;
    char *bytes = (char *) &i;
    currentEndianToBigEndian(bytes, 0, 7);
    device->write(bytes, 8);
}
void IOReader::write(const char *bytes, int64 size)
{
    CHECK_IODEVICE_VALID_WRITE;
    device->write(bytes, size);
}

void IOReader::exchangeBytes(char *bytes, int start, int end)
{
    char temp;
    while (start < end)
    {
        temp = *(bytes + start);
        *(bytes + start) = *(bytes + end);
        *(bytes + end) = temp;
        ++start;
        --end;
    }
}

void IOReader::bigEndianToCurrentEndian(char *bytes, int start, int end)
{
    if (!IS_BIG_ENDIAN)
    {
       exchangeBytes(bytes, start, end);
    }
}

void IOReader::currentEndianToBigEndian(char *bytes, int start, int end)
{
    if (!IS_BIG_ENDIAN)
    {
        exchangeBytes(bytes, start, end);
    }
}

bool IOReader::isClosed()
{
    CHECK_IODEVICE_VALID_READ(closed);
    return closed;
}

int64 IOReader::bytesAvailable() const
{
    device->waitForReadyRead(0);
    return device->bytesAvailable();
}

void IOReader::flush()
{
    device->flush();
}

QAbstractSocket* IOReader::socket()
{
    return device;
}

}