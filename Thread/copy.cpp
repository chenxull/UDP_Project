#include <stdio.h>
#include <string.h>
#include <iostream>

void copy_context(char *dest, char *src)
{
    char *temp;
    temp = dest;
    memcpy(&dest[7], src, 14);
    // *dest = '\0';
    std::cout << "temp = " << *temp << std::endl;
    std::cout << "dest = " << *dest << std::endl;
}

int main(int argc, char *argv[])
{
    char cptr1[20] = "123456789abcdef";
    char *cptr2 = (char *)std::malloc(20);
    cptr2[0] = '0';
    cptr2[1] = '1';
    cptr2[2] = '2';
    cptr2[3] = '\0';

    char *ptr;
    copy_context(cptr2, cptr1);
    ptr = &cptr2[1];
    *(ptr++) = '9';
    *(ptr++) = '8';
    *(ptr++) = 'x';
    *(ptr++) = 'x';
    *(ptr++) = 'x';
    *(ptr++) = 'x';

    std::cout << "cptr1 value address = " << static_cast<const void *>(cptr1) << std::endl;
    std::cout << "cptr1 address = " << &cptr1 << std::endl;
    std::cout << "cptr2 value address = " << static_cast<const void *>(cptr2) << std::endl;
    std::cout << "cptr2 address = " << &cptr2 << std::endl;
    std::cout << "ptr value address = " << static_cast<const void *>(ptr) << std::endl;
    std::cout << "ptr address = " << &ptr << std::endl;
    std::cout << "ptr value = " << ptr << std::endl;
    std::cout << "cptr2 value = " << cptr2 << std::endl;

    return 0;
}