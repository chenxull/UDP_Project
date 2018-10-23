#include <iostream>
using namespace std;

//产生随机数据
class DataProduct{
    private:
    void rand_msg_buff(char **str,short * strlen);

};

void rand_msg_buff(char **str, short * strlen) {
    int i;
    int len;
    srand((int)time(0)); // 设置种子，不然每次rand执行结果一样
    len = rand() % 990;
    *strlen = len;
    char *p = *str;
    srand((int)time(0));
    for (i = 0; i < len; ++i){
        p[i] = 'A' + rand() % 26;
    } 
    p[i] = '\0';
}
