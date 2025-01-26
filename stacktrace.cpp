#include <cstdint>
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <cxxabi.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>


#define TRACE_MAX 1024

void handler()
{
    void *array[TRACE_MAX];
    size_t size;
    const char msg[] = "failed with a signal\n";

    size = backtrace(array, TRACE_MAX);
    char** stack = backtrace_symbols(array, size);

    for (int i = 1; i < size; ++i)
    {
        std::cout << stack[i] << std::endl;
        
        std::string addr;
        bool f = false;
        for (char* c = stack[i]; *c != 0; ++c)
        {
            if (*c == ')') break;
            if (f) addr += *c;
            if (*c == '(') 
            {
                ++c;
                if (*c == '+')
                    f = true;
            }
        }
        
        char buf[BUFSIZ];
        std::size_t readed = readlink("/proc/self/exe", buf, BUFSIZ);
        std::uint64_t prevAddr = strtoul(addr.c_str(), nullptr, 16) - 1;
        std::stringstream ss;
        ss << std::hex << prevAddr;
        std::string command = "addr2line -e " + std::string(buf) + " " + ss.str();
        system(command.c_str());
        std::cout << std::endl;
    }
}

void one()
{
    handler();
}

// g++ -g main.cpp
int main(int argc, char **argv) 
{
    one();
}
