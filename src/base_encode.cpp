
#include "base_encode.h"
#include "data.h"
base_encode::base_encode(const char *text, int version, int mode ,const char error_mode) {
    if(version <=0||version>40)
    {
        runtime_error("version should set in range 1-40");
    }
    m_version = version;
    m_text = text;
    m_mode = mode;
    m_error_mode = error_mode;


    int i=0;
    while(m_text[i]!='\0')
    {
        i++;
    }
    m_text_length = i;

    switch(m_error_mode){
        case 'L':
            m_error_mode_int = 0;
            break;
        case 'M':
            m_error_mode_int = 1;
            break;
        case 'Q':
            m_error_mode_int = 2;
            break;
        case 'H':
            m_error_mode_int = 3;
            break;
    }

    if(max_len[m_error_mode_int][m_version-1][m_mode] < m_text_length)
    {
        for(int i=0;i+m_version<40;i++)
        {
            if(max_len[m_error_mode_int][m_version-1+i][m_mode] >= m_text_length)
            {
                m_version = m_version + i ;
                break;
            }
        }
    }

}
int base_encode::data_analysis(const char *text) {
    bool code_dict[256] = {false};
    const char *alphanum_list = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
    int i=0;
    while(alphanum_list[i]!='\0')
    {
        code_dict[alphanum_list[i]]=true;
        i++;
    }
    int mode=0;
    i=0;
    while(text[i]!='\0')
    {
        if(text[i]<'0'||text[i]>'9')
        {
            mode++;
            break;
        }
        i++;
    }
    if(mode==0)
        return mode;
    else {
        i = 0;
        while (text[i] != '\0') {
            if (!code_dict[text[i]]) {
                mode++;
                break;
            }
            i++;
        }
        return mode;
    }

}
stack<int> base_encode::int_to_bit(int number,int bin_len) {
    stack<int> res;
    for(int i=0;i<bin_len;i++)
    {
        if(number&(1<<i))
        {
            res.push(1);
        } else
        {
            res.push(0);
        }
    }
    return res;
}
void base_encode::fill_bit() {
    int n = m_bit_code.size();

    int max_bites = 8 * max_bytes[m_version-1][m_error_mode_int];
    n = max_bites-n;
    for(int i=0;i<n&&i<4;i++)
    {
        m_bit_code.push_back(0);
    }

    n = m_bit_code.size() % 8;
    while (n--)
    {
        m_bit_code.push_back(0);
    }

    int padding_bytes[] = {1,1,1,0,1,1,0,0,0,0,0,1,0,0,0,1};
    n = max_bites-m_bit_code.size();
    for(int i=0;i<n;i++)
    {
        m_bit_code.push_back(padding_bytes[i%16]);
    }
}
void base_encode::error_correction_coding() {
    for(int i=0;i<m_bit_code.size();i+=8)
    {
        int temp=0;
        for(int j=0;j<8;j++)
        {
            temp=temp*2+m_bit_code[i+j];
        }
        m_int_code.push_back(temp);
    }
    int block0_num = error_correction[m_version-1][m_error_mode_int][0];
    int block0_size = error_correction[m_version-1][m_error_mode_int][1];
    int block1_num = error_correction[m_version-1][m_error_mode_int][2];
    int block1_size = error_correction[m_version-1][m_error_mode_int][3];

    int en = ecc_num_per_block[m_version-1][m_error_mode_int];
    vector<int>  gp = gp_list[en];
    vector<vector<int> >error_code_list,reminder_list;
    for(int i=0;i<block0_num;i++)
    {
        vector<int> reminder;
        for(int j=0;j<block0_size;j++)
        {
            reminder.push_back(m_int_code[i*block0_size+j]);
        }
        reminder_list.push_back(reminder);
        for(int j=0;j<block0_size;j++)
        {
            reminder = divide(reminder,gp);
        }
        error_code_list.push_back(reminder);
    }
    for(int i=0;i<block1_num;i++)
    {
        vector<int> reminder;
        for(int j=0;j<block1_size;j++)
        {
            reminder.push_back(m_int_code[block0_num*block0_size+i*block1_size+j]);
        }
        reminder_list.push_back(reminder);
        for(int j=0;j<block1_size;j++)
        {
            reminder = divide(reminder,gp);
        }
        error_code_list.push_back(reminder);
    }
    vector<int> final_code,final_error_code;int max_len=0;
    for(int i=0;i<reminder_list.size();i++)
    {
        if(reminder_list[i].size()>max_len)
        {
            max_len = reminder_list[i].size();
        }
        if(error_code_list[i].size()>max_len)
        {
            max_len = error_code_list[i].size();
        }
    }
    for(int i=0;i<max_len;i++)
    {
        int list_size = reminder_list.size();
        for(int j=0;j<list_size;j++)
        {
            if(i<reminder_list[j].size())
            {
                final_code.push_back(reminder_list[j][i]);
            }
            if(i<error_code_list[j].size())
                final_error_code.push_back(error_code_list[j][i]);
        }
    }
    int final_error_code_len =final_error_code.size();
    for(int i=0;i<final_error_code_len;i++)
    {
        final_code.push_back(final_error_code[i]);
    }
    m_bit_code.clear();
    for(int i=0;i<final_code.size();i++)
    {
        stack<int> temp = int_to_bit(final_code[i],8);
        for(int j=0;j<8;j++)
        {
            m_bit_code.push_back(temp.top());
            temp.pop();
        }
    }
    int remainder_bits_len = remainder_bits[m_version-1];
    for(int i=0;i<remainder_bits_len;i++)
    {
        m_bit_code.push_back(0);
    }
}
vector<int> base_encode::divide(vector<int> reminder,vector<int> gp)
{
    if(reminder[0])
    {
        for(int i=0;i<gp.size();i++)
        {
            gp[i]+=log_log[reminder[0]];
            if(gp[i]>255)
            {
                gp[i]%=255;
            }
            gp[i]= po2[gp[i]];
        }
        return XOR(gp,reminder);
    }
    else{
        for(int i=0;i<gp.size();i++)
            gp[i]=0;
        return XOR(gp,reminder);
    }
}
vector<int> base_encode::XOR(vector<int> gp, vector<int> reminder) {
    int len = reminder.size() - gp.size();
    if(len>0)
    {
        while(len--)
            gp.push_back(0);
    } else
    {
        len =-1*len;
        while(len--)
            reminder.push_back(0);
    }
    vector<int> remin;
    for(int i=1;i<reminder.size();i++)
    {
        remin.push_back(reminder[i]^gp[i]);
    }
    return remin;
}
void base_encode::set_matrix() {
    int width = (m_version-1) * 4 + 21;
    for(int i=0;i<width;i++)
    {
        vector<int> temp;
        for(int j=0;j<width;j++)
        {
            temp.push_back(-1);
        }
        m_matrix.push_back(temp);
    }
    //position
    for(int i=0;i<7;i++)
    {
        m_matrix[0][i] = 1;
        m_matrix[0][width-1-i] = 1;
        m_matrix[6][i] = 1;
        m_matrix[6][width-1-i] = 1;
        m_matrix[width-1][i] = 1;
        m_matrix[width-7][i] = 1;

        m_matrix[i][0] = 1;
        m_matrix[width-i-1][0] = 1;
        m_matrix[i][6] = 1;
        m_matrix[width-i-1][6] = 1;
        m_matrix[i][width-7] = 1;
        m_matrix[i][width-1] = 1;

        m_matrix[i][7] = 0;
        m_matrix[width-1-i][7] = 0;
        m_matrix[i][width-8] = 0;

        m_matrix[7][i] = 0;
        m_matrix[7][width-1-i] = 0 ;
        m_matrix[width-8][i] = 0;
    }
    m_matrix[7][7] = 0;
    m_matrix[width-8][7] = 0;
    m_matrix[7][width-8] = 0;

    for(int i=0;i<5;i++)
    {
        m_matrix[1][i+1] = 0;
        m_matrix[1][width-2-i] = 0;
        m_matrix[5][i+1] = 0;
        m_matrix[5][width-2-i] = 0;
        m_matrix[width-2][i+1] = 0;
        m_matrix[width-6][i+1] =0;

        m_matrix[i+1][1] = 0;
        m_matrix[width-i-2][1] = 0;
        m_matrix[i+1][5] = 0;
        m_matrix[width-i-2][5] = 0;
        m_matrix[i+1][width-2] = 0;
        m_matrix[i+1][width-6] =0;
    }
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            m_matrix[i+2][j+2] = 1;
            m_matrix[i+2][width-3-j] =1 ;
            m_matrix[width-3-i][j+2] = 1;
        }
    }

    //alignment
    if(m_version>1){
        vector<int> align = align_location[m_version-2];
        for(int i=0;i<align.size();i++)
        {
            for(int j=0;j<align.size();j++)
            {
                if(m_matrix[align[i]][align[j]]==-1)
                {
                    add_alignment(align[i],align[j]);
                }
            }
        }
    }
    //Timing pattern
    for(int i=8;i<width-8;i+=2)
    {
        m_matrix[6][i] = 1;
        m_matrix[6][i+1]=0;
        m_matrix[i][6] = 1;
        m_matrix[i+1][6] = 0;
    }

    //Format Information
    //dark module reserved areas
    //dark Module
    m_matrix[4*m_version+9][8] = 1;
    //reserved areas
    for(int i=0;i<6;i++)
    {
        m_matrix[8][i] = 0;
        m_matrix[i][8] = 0;
        m_matrix[8][width-1-i] =0;
        m_matrix[width-1-i][8] = 0;
    }
    m_matrix[width-7][8] = m_matrix[8][7] = m_matrix[8][8] =m_matrix[7][8] =m_matrix[8][width-7]=m_matrix[8][width-8] = 0;
    //Reserve the Version Information Area
    if(m_version > 6)
    {
        for(int i=0;i<6;i++)
        {
            m_matrix[i][width-9]=m_matrix[i][width-10]=m_matrix[i][width-11]=0;
            m_matrix[width-9][i]=m_matrix[width-10][i]=m_matrix[width-11][i]=0;
        }
    }

    vector<vector<int> > mask_matrix;
    mask_matrix = m_matrix;
    mask_matrix[width-8][8] = -1;

    //place bits m_bit_code
    int t=0;
    bool up = true;
    for(int i=width-1;i>=0;i-=2)
    {
        i = (i == 6)?i-1:i;
        if(up)
        {
            for(int j= width-1;j>=0;j--)
            {
                if(m_matrix[j][i] == -1)
                {
                    m_matrix[j][i] = m_bit_code[t++];
                }
                if(m_matrix[j][i-1] == -1)
                {
                    m_matrix[j][i-1] = m_bit_code[t++];
                }
            }
        } else
        {
            for(int j= 0;j<width;j++)
            {
                if(m_matrix[j][i] == -1)
                {
                    m_matrix[j][i] = m_bit_code[t++];
                }
                if(m_matrix[j][i-1] == -1)
                {
                    m_matrix[j][i-1] = m_bit_code[t++];
                }
            }
        }
        up = !up;
    }

    //mask
    vector<vector<vector<int> > > mask_patterns_list = get_mask_patterns(mask_matrix);
    int min=0,mask_idx=0;
    for(int t=0;t<mask_patterns_list.size();t++)
    {
        for(int i=0;i<width;i++)
        {
            for (int j = 0; j < width; ++j) {
                mask_patterns_list[t][i][j] = mask_patterns_list[t][i][j]^m_matrix[i][j];
            }
        }

        int score = score1(mask_patterns_list[t]) + score2(mask_patterns_list[t]) + score3(mask_patterns_list[t]) + score4(mask_patterns_list[t]);
        if(t==0)
        {
            min = score;
        } else
        {
            if(min>score)
            {
                min = score;
                mask_idx = t;
            }
        }
    }
    m_matrix = mask_patterns_list[mask_idx];

    //format Information
    for(int i=0;i<6;i++)
    {
        m_matrix[width-1-i][8] = m_matrix[8][i] = format_string[m_error_mode_int][mask_idx][i];
        m_matrix[5-i][8] = m_matrix[8][width-6+i] = format_string[m_error_mode_int][mask_idx][9+i];
    }
    m_matrix[width-7][8] = m_matrix[8][7] = format_string[m_error_mode_int][mask_idx][6];
    m_matrix[8][width-8] = m_matrix[8][8] = format_string[m_error_mode_int][mask_idx][7];
    m_matrix[8][width-7] = m_matrix[7][8] = format_string[m_error_mode_int][mask_idx][8];
    //version Information
    int s = 0;
    if(m_version>6)
    {
        for(int i=5;i>=0;i--)
        {
            m_matrix[width-9][i] = m_matrix[i][width-9] = version_string[m_version-7][s++];
            m_matrix[width-10][i] = m_matrix[i][width-10] = version_string[m_version-7][s++];
            m_matrix[width-11][i] = m_matrix[i][width-11] = version_string[m_version-7][s++];
        }
    }
}
vector<vector<int> > base_encode::get_matrix() {
    return m_matrix;
}
void base_encode::add_alignment(int y, int x) {
    m_matrix[y][x] = 1;
    for(int i=0;i<5;i++)
    {
        m_matrix[y-2][x-2+i] = 1;
        m_matrix[y+2][x-2+i] = 1;
        m_matrix[y-2+i][x-2] = 1;
        m_matrix[y-2+i][x+2] = 1;
    }
    m_matrix[y-1][x-1] = 0;
    m_matrix[y-1][x] = 0;
    m_matrix[y-1][x+1] = 0;
    m_matrix[y][x-1] = 0;
    m_matrix[y][x+1] = 0;
    m_matrix[y+1][x-1] = 0;
    m_matrix[y+1][x] = 0;
    m_matrix[y+1][x+1] = 0;
}

vector<vector<vector<int> > > base_encode::get_mask_patterns(vector<vector<int> > mask_patterns) {
    int width = mask_patterns.size();
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<width;j++)
        {
            if(mask_patterns[i][j] != -1)
                mask_patterns[i][j] = 0;
        }
    }
    vector<vector<vector<int> > > mask_patterns_list;
    for(int t=0;t<8;t++)
    {
        vector<vector<int> >mask_patterns_copy = mask_patterns;
        for(int i=0;i<width;i++)
        {
            for(int j=0;j<width;j++)
            {
                if(mask_patterns_copy[i][j]==-1&&mask_formula(t,i,j))
                    mask_patterns_copy[i][j] = 1;
                else
                    mask_patterns_copy[i][j] = 0;
            }
        }
        mask_patterns_list.push_back(mask_patterns_copy);
    }
    return mask_patterns_list;
}
bool base_encode::mask_formula(int pattern, int y, int x) {
    switch (pattern)
    {
        case 0:
            return (y+x) % 2 == 0;
        case 1:
            return y % 2 == 0;
        case 2:
            return x % 3 == 0;
        case 3:
            return (x + y) % 3 == 0;
        case 4:
            return (y / 2 + x / 3) % 2 == 0;
        case 5:
            return (((y * x) % 2)+((x * y) % 3)) == 0;
        case 6:
            return (((y * x) % 2)+((x * y) % 3)) % 2 == 0;
        case 7:
            return (((y + x) % 2)+((x * y) % 3)) % 2 == 0;
    }
}
int base_encode::score1(vector<vector<int> > matrix) {
    int sc = 0,width = matrix.size();
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<width;j)
        {
            int temp = matrix[i][j],t = 1;
            for(t=1;j+t<width;t++)
            {
                if(temp != matrix[i][j+t])
                {
                    break;
                }
            }
            if(t>=5)
            {
                sc=sc+t-2;
            }
            j+=t;
        }
    }
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<width;j)
        {
            int temp = matrix[j][i],t = 1;
            for(t=1;j+t<width;t++)
            {
                if(temp != matrix[j+t][i])
                {

                    break;
                }
            }
            if(t>=5)
            {
                sc=sc+t-2;
            }
            j+=t;
        }
    }
    return sc;

}
int base_encode::score2(vector<vector<int> > matrix) {
    int sc =0,width = matrix.size();
    for(int i=0;i<width-1;i++)
    {
        for(int j=0;j<width-1;j++)
        {
            if((matrix[i][j]==matrix[i][j+1])&&(matrix[i][j]==matrix[i+1][j+1])&&(matrix[i][j]==matrix[i+1][j]))
            {
                sc+=3;
            }
        }
    }
    return sc;
}
int base_encode::score3(vector<vector<int> > matrix) {
    int sc =0,width = matrix.size();
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<width;j++)
        {
            if((j<width-10)&&((
                    (matrix[i][j]==1)&&(matrix[i][j+1]==0)&&(matrix[i][j+2]==1)&&(matrix[i][j+3]==1)&&(matrix[i][j+4]==1)
            &&(matrix[i][j+5]==0)&&(matrix[i][j+6]==1)&&(matrix[i][j+7]==0)&&(matrix[i][j+8]==0)&&(matrix[i][j+9]==0)&&(matrix[i][j+10]==0))||(
                    (matrix[i][j]==0)&&(matrix[i][j+1]==0)&&(matrix[i][j+2]==0)&&(matrix[i][j+3]==0)&&(matrix[i][j+4]==1)
                    &&(matrix[i][j+5]==0)&&(matrix[i][j+6]==1)&&(matrix[i][j+7]==1)&&(matrix[i][j+8]==1)&&(matrix[i][j+9]==0)&&(matrix[i][j+10]==1)
                    )))
            {
                sc+=40;
            }
            if((i<width-10)&&((
                      (matrix[i][j]==1)&&(matrix[i+1][j]==0)&&(matrix[i+2][j]==1)&&(matrix[i+3][j]==1)&&(matrix[i+4][j]==1)
                      &&(matrix[i+5][j]==0)&&(matrix[i+6][j]==1)&&(matrix[i+7][j]==0)&&(matrix[i+8][j]==0)&&(matrix[i+9][j]==0)&&(matrix[i+10][j]==0))||(
                      (matrix[i][j]==0)&&(matrix[i+1][j]==0)&&(matrix[i+2][j]==0)&&(matrix[i+3][j]==0)&&(matrix[i+4][j]==1)
                      &&(matrix[i+5][j]==0)&&(matrix[i+6][j]==1)&&(matrix[i+7][j]==1)&&(matrix[i+8][j]==1)&&(matrix[i+9][j]==0)&&(matrix[i+10][j]==1)
                      )))
            {
                sc+=40;
            }
        }

    }
    return sc;
}
int base_encode::score4(vector<vector<int> > matrix) {
    int sc =0,width = matrix.size(),black =0;
    double max_points = width*width;
    for(int i=0;i<width;i++) {
        for (int j = 0; j < width; j++) {
            if(matrix[i][j]==1)
            {
                black ++;
            }
        }
    }
    int percent =int((black / max_points)*100);
    int percent_big = (percent+5)/5*5;
    int percent_small = (percent)/5*5;
    if(percent_big<=50)
    {
        sc = (50-percent_big)*2;
    } else
    {
        sc = (percent_small-50)*2;
    }
    return sc;
}