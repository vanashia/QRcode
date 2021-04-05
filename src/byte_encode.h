

#ifndef QRCODE_BYTE_ENCODE_H
#define QRCODE_BYTE_ENCODE_H

#include "base_encode.h"

class byte_encode: public base_encode {
public:
    byte_encode(const char* text,int version, int mode, const char error_mode);
    virtual void data_encoding();

};


#endif //QRCODE_BYTE_ENCODE_H
