

#ifndef QRCODE_NUMERIC_ENCODE_H
#define QRCODE_NUMERIC_ENCODE_H
#include "base_encode.h"
#include <vector>
#include <iostream>
using namespace std;
class numeric_encode : public base_encode
{
public:
    numeric_encode(const char* text, int version,int mode,const char error_mode);

    virtual  void data_encoding();
private:
};


#endif //QRCODE_NUMERIC_ENCODE_H
