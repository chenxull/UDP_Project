#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <cv.h>
#include <highgui.h>
#include "cvaux.h"

using namespace cv;

//#include "CImg.h"
//using namespace cimg_library;
using namespace std;
#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) > (y) ? (y) : (x))
int no_lines = 2048;

void medianfilter(float *image, float *result, int N);
void _medianfilter(float *image, float *result, int N);
void lashenhanshu(float *logdata2, float *image, int fa, int fb, int ga, int gb);
// m=1;
//float *trans_e=(float*)malloc(sizeof(float ));
//trans_e[0]=d_e*(m-1);
//template<typename T>  struct cimg_library::CImg;
int main(int argc, char const *argv[])
{
    //fstream file;
    // file.open("1024.txt");
    //Mat Ky1_Data = Mat::zeros(no_lines, no_lines, CV_32FC1);
    string infile = argv[1];

    if (argc == 3)
    {
        no_lines = atoi(argv[2]);
    }

    float *data = (float *)malloc(sizeof(float) * no_lines * no_lines);
    float *logdata = (float *)malloc(sizeof(float) * no_lines * no_lines);
    float *logdata2 = (float *)malloc(sizeof(float) * no_lines * no_lines);
    float *image = (float *)malloc(sizeof(float) * no_lines * no_lines);
    float *result = (float *)malloc(sizeof(float) * no_lines * no_lines);
    float maxdata = 0;
    //float maxdata=0;
    float image_width = 200.0 / 1000;
    float image_length = 200.0 / 1000;

    float d_x = image_width / (no_lines - 1);
    float d_z = image_length / (no_lines - 1);
    float z1, x, imagelength;

    float maxlogdata = 0, minlogdata = 255;
    //ss<<"E://image_reconstruction//code//CUDA//052A_cuda//512_imagedata125M_dmas_nochangeaperture.txt";
    ifstream is(infile);
    for (int i = 0; i < no_lines; i++)
    {
        for (int s = 0; s < no_lines; s++)
        {
            is >> data[i * no_lines + s];
            //file>>Ky1_Data.at<float>(i, s);
            z1 = -image_length / 2 + d_z * i;
            x = -image_length / 2 + d_x * s;
            imagelength = sqrt(x * x + z1 * z1);
            if (imagelength < 0.1)
            {
                data[i * no_lines + s] = abs(data[i * no_lines + s]);
            }
            else
                data[i * no_lines + s] = 0;
            maxdata = max(data[i * no_lines + s], maxdata);
        }
    }
    cout << data[100] << endl;
    is.close();
    //Ky1_Data=abs(Ky1_Data);
    //cout<<maxdata<<endl;
    for (int j = 0; j < no_lines * no_lines; j++)
    {
        logdata[j] = 15 * log10f((data[j] / maxdata) + 1e-3);
        maxlogdata = max(logdata[j], maxlogdata);
        minlogdata = min(logdata[j], minlogdata);
    }

    //cout<<maxlogdata<<endl;
    //cout<<minlogdata<<endl;
    for (int j = 0; j < no_lines * no_lines; j++)
    {
        logdata2[j] = (logdata[j] - minlogdata) / (maxlogdata - minlogdata) * 255;
    }

    //cout<<logdata2[512797]<<endl;
    int fa = 60, fb = 210; //1:40 210 2:40 220 0:50 190
    int ga = 50, gb = 230;
    lashenhanshu(logdata2, image, fa, fb, ga, gb);
    medianfilter(image, result, no_lines);

    Mat Ky1_Data(no_lines, no_lines, CV_32FC1, result);

    imwrite("160.png", Ky1_Data); //range 0-255
                                  //imshow("src", Ky1_Data);//range 0-1

    free(logdata);
    free(data);
    free(image);
    free(result);
    free(logdata2);
    waitKey(0);
    //cvReleaseImage(&img);
    //system("pause");
}
void lashenhanshu(float *logdata2, float *image, int fa, int fb, int ga, int gb)
{
    float k1 = ga / fa;
    float k2 = (gb - ga) / (fb - fa);
    float k3 = (255 - gb) / (255 - fb);
    for (int i = 0; i < no_lines; i++)
    {
        for (int j = 0; j < no_lines; j++)
        {
            if (logdata2[i * no_lines + j] <= fa)
                image[i * no_lines + j] = (k1 * logdata2[i * no_lines + j]);
            else if (fa < logdata2[i * no_lines + j] && logdata2[i * no_lines + j] <= fb)
                image[i * no_lines + j] = (k2 * (logdata2[i * no_lines + j] - fa) + ga);
            else
                image[i * no_lines + j] = (k3 * (logdata2[i * no_lines + j] - fb) + gb);
        }
    }
}
void _medianfilter(float *image, float *result, int N)
{
    const int nwindow = 1;
    //float* extension = new float[(1024 + (nwindow-1)) * (1024 + nwindow-1)];
    //   Move window through all elements of the image
    for (int m = (nwindow - 1) / 2; m < N - (nwindow - 1) / 2; ++m)
        for (int n = (nwindow - 1) / 2; n < N - (nwindow - 1) / 2; ++n)
        {
            //   Pick up window elements
            int k = 0;
            float window[nwindow * nwindow];
            for (int j = m - (nwindow - 1) / 2; j < m + (nwindow + 1) / 2; ++j)
                for (int i = n - (nwindow - 1) / 2; i < n + (nwindow + 1) / 2; ++i)
                    window[k++] = image[j * N + i];
            //   Order elements (only half of them)
            for (int j = 0; j < (nwindow * nwindow + 1) / 2; ++j)
            {
                //   Find position of minimum element
                int min = j;
                for (int l = j + 1; l < nwindow * nwindow; ++l)
                    if (window[l] < window[min])
                        min = l;
                //   Put found minimum element in its place
                const float temp = window[j];
                window[j] = window[min];
                window[min] = temp;
            }
            //   Get result - the middle element
            result[(m - (nwindow - 1) / 2) * (N - nwindow + 1) + n - (nwindow - 1) / 2] = window[(nwindow * nwindow - 1) / 2];
            // cout<<result[(m - (nwindow-1)/2) * (N -nwindow+1 ) + n - (nwindow-1)/2]<<endl;
        }
}
void medianfilter(float *image, float *result, int N)
{
    const int nwindow = 1;
    float *extension = new float[(N + nwindow - 1) * (N + nwindow - 1)];
    //   Create image extension
    for (int i = 0; i < N; ++i)
    {
        memcpy(extension + (N + nwindow - 1) * (i + (nwindow - 1) / 2) + (nwindow - 1) / 2,
               image + N * i,
               N * sizeof(float));
        for (int j = 0; j < (nwindow - 1) / 2; j++)
        {
            extension[(N + nwindow - 1) * (i + (nwindow - 1) / 2) + j] = image[N * i];
            extension[(N + nwindow - 1) * (i + (nwindow + 1) / 2) - 1 - j] = image[N * (i + 1) - 1];
        }
    }
    //   Fill first line of image extension
    for (int j = 0; j < (nwindow - 1) / 2; j++)
    {
        memcpy(extension + j * (N + nwindow - 1),
               extension + (N + nwindow - 1) * ((nwindow - 1) / 2),
               (N + nwindow - 1) * sizeof(float));
        //   Fill last line of image extension
        memcpy(extension + (N + nwindow - 1) * (N + (nwindow - 1) / 2 + j),
               extension + (N + nwindow - 1) * (N + (nwindow - 1) / 2 - 1),
               (N + nwindow - 1) * sizeof(float));
    }
    //   Call median filter implementation
    _medianfilter(extension, result, N + nwindow - 1);
    //   Free memory
    delete[] extension;
}
