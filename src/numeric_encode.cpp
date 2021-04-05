
#include "numeric_encode.h"

numeric_encode::numeric_encode(const char* text,int version, int mode, const char error_mode):base_encode(text,version,mode,error_mode)
{
    m_bit_code.push_back(0);
    m_bit_code.push_back(0);
    m_bit_code.push_back(0);
    m_bit_code.push_back(1);

    if(version<=9)
    {
        m_indicator_length = 10;
    } else if (version<=26)
    {
        m_indicator_length = 12;
    } else
    {
        m_indicator_length = 14;
    }
    stack<int> indicator = int_to_bit(m_text_length,m_indicator_length);
    while(!indicator.empty())
    {
        m_bit_code.push_back(indicator.top());
        indicator.pop();
    }
}

void numeric_encode::data_encoding()
{
    int i=0;

    while(m_text[i]!='\0')
    {
        int t=0,len=0,add = 0;
        if(m_text[i+1]=='\0')
        {
            t = m_text[i]-'0';
            len = 4;
            add = 1;
        } else if (m_text[i+2]=='\0')
        {
            t = (m_text[i]-'0')*10+m_text[i+1]-'0';
            len = 7;
            add = 2;
        } else
        {
            t = (m_text[i]-'0')*100+(m_text[i+1]-'0')*10+m_text[i+2]-'0';
            len = 10;
            add = 3;
        }
        stack<int> indicator = int_to_bit(t,len);
        while(!indicator.empty())
        {
            m_bit_code.push_back(indicator.top());
            indicator.pop();
        }
        i+=add;
    }

    fill_bit();
}
