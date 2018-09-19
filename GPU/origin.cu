#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <cstring>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

using namespace std;

#define PI 3.14159265358979323846
#define N 2048
#define M 256 //接收阵元到发射阵元的最大距离（阵元个数），所以接收孔径为2*M+1
#define ELE_NO 2048
#define OD 64
#define NSAMPLE 3750

//const int sample_point_num=5000;

__device__ float dev_ele_position_center[ELE_NO];
__device__ float dev_ele_position_height[ELE_NO]; //写到纹理内存里面
__device__ float dev_filter_data[OD];             //filter parameter

short uct_data_board0[NSAMPLE * ELE_NO] = {0}; //写到页锁定主机内存
// short before_convert_data;
//float trans_sdata[sample_point_num*ELE_NO]={0};
float allimage_data[N * N] = {0};
int allpoint_count[N * N] = {0};

//cudaError_t calcWithCuda(int i,float *dev_sumdata,int *dev_sumpoint,float *dev_filterdata);

//__device__ float da[31];
// float filter_data[OD];
//float a[31];
//int i;
// cudaError_t precalcWithCuda(short *uct_data_board0, int i, float *dev_sumdata, int *dev_sumpoint);

//short sample_data[sample_point_num][ELE_NO]={0};

//double v[3344][256]={0};;
//double u_test[1024]={0};
__global__ void kernel3(float *filterdata, short *data1_1024_output)
{
    int bid = blockIdx.x;
    int tid = threadIdx.x;
    int id = blockDim.x * bid + tid;
    short data[NSAMPLE];
    float fdata[NSAMPLE];

    if (id < ELE_NO)
    {
        //da[0]=0.0;
        memset(fdata, 0, NSAMPLE * sizeof(float));
        for (int i = 0; i < NSAMPLE; i++)
        {
            data[i] = data1_1024_output[i * ELE_NO + id];
            for (int j = 0; i >= j && j < OD; j++)
            {
                //fdata[i] += (dev_filter_data[j]*data[i-j]-da[j]*fdata[i-j]);
                fdata[i] += (dev_filter_data[j] * data[i - j]);
            }
        }
        //da[0]=1.0;

        for (int i = 0; i < NSAMPLE; i++)
        {
            filterdata[i * ELE_NO + id] = fdata[i];
        }
    }
}

__global__ void kernel(int i, float *image_data, int *point_count, float *trans_sdata)
{
    int c = 1520;
    float fs = 25e6;
    float image_width = 200.0 / 1000;
    float image_length = 200.0 / 1000;
    float data_diameter = 220.0 / 1000;
    int point_length = data_diameter / c * fs + 0.5;

    const int no_lines = N;
    const int no_point = N;

    float d_x = image_width / (no_lines - 1);
    float d_z = image_length / (no_point - 1);

    int middot = -160; //发射前1us开始接收，也就是约为12.5个点之后发射,数据显示约16个点
                       //const int ELE_NO=1024;

    int k_line = blockIdx.y;                       //线
    int nn = blockIdx.x;                           //点
                                                   //blockIdx.x+blockIdx.y * gridDimx.x
    int y = threadIdx.x;                           //接收阵元
    int j = i - 1 - M + y;                         //接收阵元
    int bid = blockIdx.x + blockIdx.y * gridDim.x; //线程块的索引
    int tid = blockDim.x * bid + threadIdx.x;      //线程的索引

    __shared__ float cache_image[2 * M];
    __shared__ int cache_point[2 * M];
    /*__shared__ float cache_image2[512];*/
    int cacheIndex = threadIdx.x;

    if (k_line < N && nn < N && y < 2 * M)
    {
        float u = 0;
        int point_count_1 = 0;
        float z1 = -image_length / 2 + d_z * nn;
        float x = -image_length / 2 + d_x * k_line;
        float xg = 0.0014;

        // for(int jj=1;jj<=ELE_NO/M;jj++)
        // {
        //  int j=y*ELE_NO/M+jj;
        j = (j + ELE_NO) % ELE_NO;

        //int num= (sqrt(pow((dev_ele_position_center[i-1]-x),2) + pow((z1-dev_ele_position_height[i-1]),2))+sqrt(pow((dev_ele_position_center[j-1]-x),2) + pow((z1-dev_ele_position_height[j-1]),2)))/c*fs+0.5;
        float disi = sqrt((dev_ele_position_center[i - 1] - x) * (dev_ele_position_center[i - 1] - x) + (z1 - dev_ele_position_height[i - 1]) * (z1 - dev_ele_position_height[i - 1]));
        float disj = sqrt((dev_ele_position_center[j] - x) * (dev_ele_position_center[j] - x) + (z1 - dev_ele_position_height[j]) * (z1 - dev_ele_position_height[j]));
        float ilength = 112.0 / 1000;
        float imagelength = sqrt(x * x + z1 * z1);
        float angle = acos((ilength * ilength + disi * disi - imagelength * imagelength) / 2 / ilength / disi);
        if ((disi >= 0.1 * 2 / 3 && (abs(i - j - 1) < 256 || abs(i - j - 1) > 1024 - 256)) || (disi >= 0.1 * 1 / 3 && (abs(i - j - 1) < 200 || abs(i - j - 1) > 1024 - 200)) || (disi >= 0 && (abs(i - j - 1) < 100 || abs(i - j - 1) > 1024 - 100)))
        {
            int num = (disi + disj) / c * fs + 0.5;
            //if (((num+middot)>200)&&(num<=point_length)&&(angle<PI/9))
            // if (((num+middot+(OD-1-1)/2)>100)&&((num+middot+(OD-1-1)/2)<=point_length))
            if (((num + middot + (OD - 1 - 1) / 2) > 100) && ((num + middot + (OD - 1 - 1) / 2) <= point_length) && (angle < PI / 9))
            {
                //u= trans_sdata[(num+middot)*ELE_NO+(j-1)];
                // u= trans_sdata[(num+middot)*ELE_NO+j]*exp(xg*(num-1));
                // u= trans_sdata[(num+middot)*ELE_NO+j];
                // u= trans_sdata[(num+middot+(OD-1-1)/2)*ELE_NO+j];
                u = trans_sdata[(num + middot + (OD - 1 - 1) / 2) * ELE_NO + j] * exp(xg * (num - 1));
                // u= trans_sdata[(num+middot+(127-1)/2)*ELE_NO+j]*exp(xg*(num-1));
                //u= trans_sdata[(num+middot+(127-1)/2)*ELE_NO+j]*exp(xg*(num-1));
                // cout<<u<<endl;
                point_count_1 = 1;
            }
        }

        //}
        cache_image[cacheIndex] = u;
        cache_point[cacheIndex] = point_count_1;

        __syncthreads();

        /*int jj=1;
        while((cacheIndex + jj)< blockDim.x){
        cache_image2[cacheIndex]+=cache_image[cacheIndex]*cache_image[cacheIndex + jj];
         __syncthreads();
                 jj= jj+1;
        }*/

        int i = blockDim.x / 2;
        while (i != 0)
        {
            if (cacheIndex < i)
            {
                cache_image[cacheIndex] += cache_image[cacheIndex + i];
                cache_point[cacheIndex] += cache_point[cacheIndex + i];
            }
            __syncthreads();
            i /= 2;
        }

        if (cacheIndex == 0)
        {
            image_data[bid] = cache_image[0];
            point_count[bid] = cache_point[0];
        }
    }
}

__global__ void add(float *sumdata, int *sumpoint, float *imagedata, int *point_count)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    while (tid < N * N)
    {
        sumdata[tid] += imagedata[tid];
        sumpoint[tid] += point_count[tid];
        tid += blockDim.x * gridDim.x;
    }
}

cudaError_t precalcWithCuda(short *uct_data_board0, int i, float *dev_sumdata, int *dev_sumpoint)
{
    cudaError_t cudaStatus;
    short *dev_uct_data_board0;
    float *dev_filterdata;

    cudaStatus = cudaMalloc((void **)(&dev_uct_data_board0), NSAMPLE * ELE_NO * sizeof(short));
    if (cudaStatus != cudaSuccess)
    {
        cout << "uct_data_board0 Fail to cudaMalloc on GPU" << endl;
        //goto Error;
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void **)(&dev_filterdata), NSAMPLE * ELE_NO * sizeof(float));
    if (cudaStatus != cudaSuccess)
    {
        cout << "uct_data_board0 Fail to cudaMalloc on GPU" << endl;
        //goto Error;
        return cudaStatus;
    }

    cudaStatus = cudaMemcpy(dev_uct_data_board0, uct_data_board0, NSAMPLE * ELE_NO * sizeof(short), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cout << "uct_data_board0 Fail to cudaMemcpy on GPU" << endl;
        //goto Error;
        return cudaStatus;
    }

    //kernel 1,kernel2 decode
    //kernel3 filter
    kernel3<<<8, 256>>>(dev_filterdata, dev_uct_data_board0);
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cout << "Kernel3 launch failed: " << cudaGetErrorString(cudaStatus);
        //goto Error;
        return cudaStatus;
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching Kernel3!\n", cudaStatus);
        //goto Error;
        return cudaStatus;
    }

    //cudaStatus= cudaMemcpy( data1_1024_output,dev_filterdata , 5000*1024 * sizeof(float),cudaMemcpyDeviceToHost ) ;
    //  if (cudaStatus != cudaSuccess) {
    // cout<<"data_output Fail to cudaMemcpy to CPU"<<endl;
    //  goto Error;
    //goto Error;
    //  }

    float *dev_imagedata;
    // float *dev_trans_sdata;

    int *dev_pointcount;
    //  int dev_i;

    cudaStatus = cudaMalloc((void **)(&dev_imagedata), N * N * sizeof(float));
    if (cudaStatus != cudaSuccess)
    {
        cout << "imagedata Fail to cudaMalloc on GPU" << endl;
        //goto Error;
        return cudaStatus;
    }
    cudaStatus = cudaMalloc((void **)(&dev_pointcount), N * N * sizeof(int));
    if (cudaStatus != cudaSuccess)
    {
        cout << "pointcount Fail to cudaMalloc on GPU" << endl;
        //goto Error;
        return cudaStatus;
    }

    dim3 gridimage(N, N);
    //dim3 threads(M);
    kernel<<<gridimage, 2 * M>>>(i, dev_imagedata, dev_pointcount, dev_filterdata); //启动一个二维的N*N个block，每个block里面M个thread

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cout << "calcKernel launch failed: " << cudaGetErrorString(cudaStatus);
        //goto Error;
        return cudaStatus;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching calcKernel!\n", cudaStatus);
        //goto Error;
        return cudaStatus;
    }
    // cudaDeviceSynchronize();

    //float *filter=(float*)malloc( N*N*sizeof(float));
    // cudaStatus= cudaMemcpy(filter,dev_imagedata ,  N*N * sizeof(float),cudaMemcpyDeviceToHost ) ;
    //  cout<<filter[128200]<<endl;
    //把所有的结果加到一起
    add<<<32, 32>>>(dev_sumdata, dev_sumpoint, dev_imagedata, dev_pointcount);
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cout << "addKernel launch failed: " << cudaGetErrorString(cudaStatus);
        //goto Error;
        return cudaStatus;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        //goto Error;
        return cudaStatus;
    }

    cudaFree(dev_uct_data_board0);
    cudaFree(dev_filterdata);
    cudaFree(dev_imagedata);
    cudaFree(dev_pointcount);
    return cudaStatus;
}

void get_ele_position(float *ele_position_center, float *ele_position_height)
{
    float rfocus = (float)112 / 1000;
    float ele_angle = (2 * PI * 43.4695 / (256 - 1)) / 360; //阵元间隔角度
    float first_one = 2 * PI * (45 - 43.4695) / 360;        //第一个阵元角度

    for (int i = 0; i < 256; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + i * ele_angle);
        ele_position_height[i] = -rfocus * sin(first_one + i * ele_angle);
    }
    for (int i = 256; i < 512; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 256) * ele_angle + PI / 4);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 256) * ele_angle + PI / 4);
    }
    for (int i = 512; i < 768; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 512) * ele_angle + PI / 2);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 512) * ele_angle + PI / 2);
    }
    for (int i = 768; i < 1024; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 768) * ele_angle + 3 * PI / 4);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 768) * ele_angle + 3 * PI / 4);
    }
    for (int i = 1024; i < 1280; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 1024) * ele_angle + PI);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 1024) * ele_angle + PI);
    }
    for (int i = 1280; i < 1536; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 1280) * ele_angle + 5 * PI / 4);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 1280) * ele_angle + 5 * PI / 4);
    }
    for (int i = 1536; i < 1792; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 1536) * ele_angle + 3 * PI / 2);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 1536) * ele_angle + 3 * PI / 2);
    }
    for (int i = 1792; i < 2048; i++)
    {
        ele_position_center[i] = rfocus * cos(first_one + (i - 1792) * ele_angle + 7 * PI / 4);
        ele_position_height[i] = -rfocus * sin(first_one + (i - 1792) * ele_angle + 7 * PI / 4);
    }
}

void write_txtfile(std::string output_path)
{
    ofstream outfile(output_path);
    if (!outfile.is_open())
    {
        cout << " the file open fail" << endl;
        exit(1);
    }

    for (int k = 0; k < N; k++)
    {
        for (int j = 0; j < N; j++)
        {
            if (allpoint_count[k * N + j] == 0)
                outfile << allimage_data[k * N + j] << " ";
            else
                outfile << allimage_data[k * N + j] / allpoint_count[k * N + j] << " ";
        }
        outfile << "\r\n";
    }

    outfile.close();
}

int main(int argc, char const *argv[])
{
    time_t start, over;
    start = time(NULL);

    std::string filter_path = "";
    std::string bin_path = "";
    std::string output_path = "";
    switch (argc)
    {
    case 3:
        filter_path = argv[1];
        bin_path = argv[2];
        output_path = "origin.txt";
        break;
    case 4:
        filter_path = argv[1];
        bin_path = argv[2];
        output_path = argv[3];
        break;
    default:
        std::cout << "Please input 2 or 3 paras" << std::endl;
        std::cout << "[filter path] [bin path]" << std::endl;
        std::cout << "[filter path] [bin path] [output path]" << std::endl;
        exit(-1);
        break;
    }

    cudaError_t cudaStatus;

    // Read filter data and put in GPU
    ifstream file_read;
    file_read.open(filter_path.c_str(), ios_base::in | ios::binary);
    if (!file_read.is_open())
    {
        cout << " the file filter open fail" << endl;
        return -1;
    }
    float filter_data[OD];
    for (int ii = 0; ii < OD; ii++)
    {
        file_read.read((char *)&filter_data[ii], sizeof(float));
    }
    file_read.close();
    cudaStatus = cudaMemcpyToSymbol(dev_filter_data, filter_data, sizeof(float) * OD);

    if (cudaStatus != cudaSuccess)
    {
        cout << "center Fail to cudaMemcpyToSymbol on GPU" << endl;
        return;
    }
    //cudaStatus=cudaMemcpyToSymbol(da,a,sizeof(float)*31);

    //if (cudaStatus != cudaSuccess) {
    //  cout<<"center Fail to cudaMemcpyToSymbol on GPU"<<endl;
    // return ;
    // }

    file_read.open(bin_path.c_str(), ios_base::in | ios::binary | ios::ate);
    if (!file_read.is_open())
    {
        cout << " the bin file open fail" << endl;
        return -1;
    }
    long long int filesize = file_read.tellg();
    file_read.seekg(0, file_read.beg);
    // 为 bin_buffer 申请空间，并把 filepath 的数据载入内存
    char *bin_buffer = (char *)std::malloc(filesize);
    if (bin_buffer == NULL)
    {
        std::cout << "ERROR :: Malloc data for buffer failed." << std::endl;
        return 0;
    }
    file_read.read(bin_buffer, filesize);
    if (file_read.peek() == EOF)
    {
        file_read.close();
    }
    else
    {
        std::cout << "ERROR :: Read bin file error." << std::endl;
        exit(-1);
    }

    //image line
    float ele_position_center[ELE_NO] = {0};
    float ele_position_height[ELE_NO] = {0};
    get_ele_position(&ele_position_center[0], &ele_position_height[0]);

    cudaStatus = cudaMemcpyToSymbol(dev_ele_position_center, ele_position_center, sizeof(float) * ELE_NO);

    if (cudaStatus != cudaSuccess)
    {
        cout << "center Fail to cudaMemcpyToSymbol on GPU" << endl;
        return -1;
    }

    cudaMemcpyToSymbol(dev_ele_position_height, ele_position_height, sizeof(float) * ELE_NO);
    if (cudaStatus != cudaSuccess)
    {
        cout << "height Fail to cudaMemcpyToSymbol on GPU" << endl;
        return -1;
    }

    float *dev_sumdata;
    int *dev_sumpoint;

    cudaStatus = cudaMalloc((void **)(&dev_sumdata), N * N * sizeof(float));
    if (cudaStatus != cudaSuccess)
    {
        cout << "sumdata Fail to cudaMalloc on GPU" << endl;
        return -1;
        //return -1;
    }
    cudaStatus = cudaMalloc((void **)(&dev_sumpoint), N * N * sizeof(int));
    if (cudaStatus != cudaSuccess)
    {
        cout << "sumpoint Fail to cudaMalloc on GPU" << endl;
        return -1;
        //return -1;
    }

    cudaStatus = cudaMemcpy(dev_sumdata, allimage_data, N * N * sizeof(float), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cout << "sumdata Fail to cudaMemcpy on GPU" << endl;
        return -1;
    }
    cudaStatus = cudaMemcpy(dev_sumpoint, allpoint_count, N * N * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cout << "sumpoint Fail to cudaMemcpy on GPU" << endl;
        return -1;
    }

    long long bin_buffer_index = 0;
    for (int trig_time = 0; trig_time < ELE_NO; trig_time++)
    //for (i=1;i<=1;i++)
    {
        cout << "Number of element : " << trig_time + 1 << endl;
        memcpy(&uct_data_board0[0], &bin_buffer[bin_buffer_index], NSAMPLE * ELE_NO * sizeof(short));
        bin_buffer_index = bin_buffer_index + NSAMPLE * ELE_NO * sizeof(short);
        // start=time(NULL);
        //for(int i=0;i<1024;i++){
        // if(trig_time%2==0)
        //{
        // continue;
        //}
        //cout<<uct_data_board0[0]<<endl;
        cudaStatus = precalcWithCuda(uct_data_board0, trig_time + 1, dev_sumdata, dev_sumpoint);
        //}
        // over=time(NULL);
        // cout<<"Running time is : "<<difftime(over,start)<<"s!"<<endl;
        if (cudaStatus != cudaSuccess)
        {
            fprintf(stderr, "calcWithCuda failed!");
            return -1;
        }

        //////////////////
        //线

        // cudaError_t cudaStatus = calcWithCuda( i,dev_sumdata,dev_sumpoint,dev_filterdata);
    }
    cudaStatus = cudaMemcpy(allimage_data, dev_sumdata, N * N * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess)
    {
        cout << "allimagedata Fail to cudaMemcpy to CPU" << endl;
        return -1;
        //goto Error;
    }

    cudaStatus = cudaMemcpy(allpoint_count, dev_sumpoint, N * N * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess)
    {
        cout << "allpointcount Fail to cudaMemcpy to CPU" << endl;
        return -1;
        //goto Error;
    }
    //cout<<allimage_data[128200]<<endl;
    // cout<<allpoint_count[128200]<<endl;

    write_txtfile(output_path);
    over = time(NULL);
    cout << "Running time is : " << difftime(over, start) / 60 << "min!" << endl;
    free(bin_buffer);
    cudaFree(dev_sumdata);
    cudaFree(dev_sumpoint);
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceReset failed!");
        return -1;
    }
}

//cudaStatus= cudaMemcpy( data1_1024_output,dev_filterdata , 5000*1024 * sizeof(float),cudaMemcpyDeviceToHost ) ;
// if (cudaStatus != cudaSuccess) {
//cout<<"data_output Fail to cudaMemcpy to CPU"<<endl;
// goto Error;
//goto Error;
// }
