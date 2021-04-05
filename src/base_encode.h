
#ifndef QRCODE_BASE_ENCODE_H
#define QRCODE_BASE_ENCODE_H
#include <vector>
#include <stack>
#include <map>
#include <iostream>
using namespace std;
class base_encode {
public:
    base_encode(const char* text, int version,int mode,const char error_mode);
    static int data_analysis(const char* text);
    virtual void data_encoding(){};
    void error_correction_coding();

    void set_matrix();
    vector<vector<int> > get_matrix();

protected:
    stack<int> int_to_bit(int number,int bin_len);
    void fill_bit();
private:
    vector<int> divide(vector<int> reminder,vector<int> gp);
    vector<int> XOR(vector<int> gp,vector<int> reminder);
    void add_alignment(int y,int x);
    vector<vector<vector<int> > > get_mask_patterns(vector<vector<int> >  mask_patterns);
    bool mask_formula(int pattern,int y,int x);

    int score1(vector<vector<int> > matrix);
    int score2(vector<vector<int> > matrix);
    int score3(vector<vector<int> > matrix);
    int score4(vector<vector<int> > matrix);
protected:
    const char* m_text=NULL;
    int m_text_length;
    int m_indicator_length;
    int m_version;
     int m_mode;
    int m_error_mode_int;
    char m_error_mode;
    vector<vector<int> > m_matrix;
    vector<int> m_bit_code,m_int_code;
};

#endif //QRCODE_BASE_ENCODE_H
