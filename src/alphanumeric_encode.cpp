
#include "alphanumeric_encode.h"
alphanumeric_encode::alphanumeric_encode(const char* text,int version, int mode, const char error_mode):base_encode(text,version,mode,error_mode)
{
    m_bit_code.push_back(0);
    m_bit_code.push_back(0);
    m_bit_code.push_back(1);
    m_bit_code.push_back(0);

    if(version<=9)
    {
        m_indicator_length = 9;
    } else if (version<=26)
    {
        m_indicator_length = 11;
    } else
    {
        m_indicator_length = 13;
    }
    stack<int> indicator = int_to_bit(m_text_length,m_indicator_length);
    while(!indicator.empty())
    {
        m_bit_code.push_back(indicator.top());
        indicator.pop();
    }
}
void alphanumeric_encode::data_encoding()
{
    int i=0;
    while(m_text[i]!='\0')
    {
        stack<int> indicater;
        if(m_text[i+1]=='\0')
       {
           int idx = get_index(m_text[i]);
           indicater = int_to_bit(idx,6);
           i++;
       } else{
            int idx = get_index(m_text[i])*45+get_index(m_text[i+1]);
            indicater = int_to_bit(idx,11);
            i+=2;
        }
        while(!indicater.empty())
        {
            m_bit_code.push_back(indicater.top());
            indicater.pop();
        }
    }
    fill_bit();
}
int alphanumeric_encode::get_index(const char ch) {
    int idx = 0;
    while(alphanumeric_char[idx]!=ch)
    {
        idx++;
    }
    return idx;
}