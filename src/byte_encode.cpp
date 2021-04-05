
#include "byte_encode.h"

byte_encode::byte_encode(const char* text,int version, int mode, const char error_mode):base_encode(text,version,mode,error_mode)
{
    m_bit_code.push_back(0);
    m_bit_code.push_back(1);
    m_bit_code.push_back(0);
    m_bit_code.push_back(0);

    if(version<=9)
    {
        m_indicator_length = 8;
    }
    else if (version<=26)
    {
        m_indicator_length = 16;
    } else
    {
        m_indicator_length = 16;
    }
    stack<int> indicator = int_to_bit(m_text_length,m_indicator_length);
    while(!indicator.empty())
    {
        m_bit_code.push_back(indicator.top());
        indicator.pop();
    }
}
void byte_encode::data_encoding() {
    int i=0;
    while(m_text[i]!='\0')
    {
        stack<int> indicater =int_to_bit(m_text[i],8);
        while(!indicater.empty())
        {
            m_bit_code.push_back(indicater.top());
            indicater.pop();
        }
        i++;
    }
    fill_bit();
}