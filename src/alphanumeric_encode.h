
#ifndef QRCODE_ALPHANUMERIC_ENCODE_H
#define QRCODE_ALPHANUMERIC_ENCODE_H
#include "base_encode.h"

class alphanumeric_encode :public base_encode{
public:
    alphanumeric_encode(const char* text,int version, int mode, const char error_mode);
    virtual void data_encoding();
private:
    int get_index(const char ch);
    const char * alphanumeric_char = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

};


#endif //QRCODE_ALPHANUMERIC_ENCODE_H
