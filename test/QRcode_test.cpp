#include<vector>
#include "../src/base_encode.h"
#include "../src/numeric_encode.h"
#include "../src/alphanumeric_encode.h"
#include "../src/byte_encode.h"
#include <opencv2/opencv.hpp>
using namespace std;

void draw_point(cv::Mat& img,int val, int x,int y,int point_width)
{

    for(int i=0;i<point_width;i++)
    {
        uchar *p = img.ptr<uchar>(x+i);
        for(int j=0;j<point_width;j++)
        {
            p[y+j]=val;
        }
    }
}
void draw(const vector<vector<int> >matrix,const char * path)
{
    int width = matrix.size();
    int point_width=9,border_width = 24;
    cv::Mat img = cv::Mat(width*point_width+border_width*2,width*point_width+border_width*2,CV_8UC1,cv::Scalar(255));
    for(int i=0;i<width;i++)
    {
        for (int j = 0; j < width; j++)
        {
            int val = 255;
            if (matrix[i][j])
            {
                val = 0;
            }
            draw_point(img,val,i*point_width+border_width,j*point_width+border_width,point_width);
        }
    }
    cv::imshow("QRCode",img);
    cv::waitKey(0);
    cv::imwrite("q.jpg",img);
}
int main(int argc,const char** argv)
{
    const char* text = argv[1];
    int mode = base_encode::data_analysis(text);
    int version =1;
    char error_mode = 'H';
    base_encode* m_encode = NULL;
    switch(mode)
    {
        case 0:
            m_encode = new numeric_encode(text,version,mode,error_mode);
            break;
        case 1:
            m_encode = new alphanumeric_encode(text,version,mode,error_mode);
            break;
        case 2:
            m_encode = new byte_encode(text,version,mode,error_mode);
            break;
        default:
            m_encode = new byte_encode(text,version,mode,error_mode);
    }
    m_encode->data_encoding();
    m_encode->error_correction_coding();
    m_encode->set_matrix();
    draw(m_encode->get_matrix(),"");
    delete m_encode;
    return 0;
}

