#include "Buffer.h"

Buffer::Buffer(QByteArray byteArray)
  : QDataStream(byteArray)
{

}
