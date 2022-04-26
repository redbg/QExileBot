#ifndef BUFFER_H
#define BUFFER_H
#include <QByteArray>
#include <QDataStream>
#include <QDebug>

class Buffer : public QDataStream
{

  quint32 m_index;

public:
  Buffer(){};
  Buffer(QByteArray byteArray);

  template<typename T>
  T readData()
  {
    quint32 size = sizeof(T);
    m_index += size;
    QByteArray data(size, 0);
    readRawData(data.data(), size);
    qDebug() << data.toHex(' ');
    return *(T*)data.data();
  }

  QByteArray readData(quint64 size)
  {
    m_index += size;
    QByteArray data(size, 0);
    readRawData(data.data(), size);
    qDebug() << data.toHex(' ');
    return data;
  }

  // 无符号
  quint32 ReadVarint()
  {
    quint8 v = readData<quint8>();

    if ((v & 0x80) == 0) {
      return v;
    }

    if ((v & 0xC0) == 0x80) {
      return readData<quint8>() | (v & 0x3f) << 8;
    } else if ((v & 0xE0) == 0xC0) {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    } else if ((v & 0xF0) == 0xE0) {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      quint8 v3 = readData<quint8>();

      return v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
    } else {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      quint8 v3 = readData<quint8>();
      quint8 v4 = readData<quint8>();
      return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
  }

  // 有符号
  qint32 ReadVarint1()
  {
    quint8 v = readData<quint8>();

    if ((v & 0x80) == 0) {
      if ((v & 0x40) == 0) {
        return v;
      } else {
        return v | 0xffffff80;
      }
    }

    if ((v & 0xC0) == 0x80) {
      return readData<quint8>() | (v & 0x3f) << 8;
    } else if ((v & 0xE0) == 0xC0) {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    } else if ((v & 0xF0) == 0xE0) {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      quint8 v3 = readData<quint8>();

      qint64 result = v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
      result = v & 8 ? result : result | 0xF0000000;
      return result;
    } else {
      quint8 v1 = readData<quint8>();
      quint8 v2 = readData<quint8>();
      quint8 v3 = readData<quint8>();
      quint8 v4 = readData<quint8>();
      return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
  }
};

#endif // BUFFER_H
