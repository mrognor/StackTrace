#include <link.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>

#include <sstream>
#include <iostream>


#define TRACE_MAX 1024


void print_backtrace()
{
    void* array[TRACE_MAX];
    size_t size;

    size = backtrace(array, TRACE_MAX);

    for (int i = 1; i < size; ++i)
    {
        Dl_info info;
        link_map *extra_info;
        dladdr1(array[i], &info, reinterpret_cast<void**>(&extra_info), RTLD_DL_LINKMAP);
        info.dli_fbase = (void *) extra_info->l_addr;


	    if (info.dli_sname == NULL)
		    info.dli_saddr = info.dli_fbase;

        ptrdiff_t offset = static_cast<char*>(array[i]) - static_cast<char*>(info.dli_saddr) - 1;

        char buf[BUFSIZ];
        std::size_t readed = readlink("/proc/self/exe", buf, BUFSIZ);
        std::stringstream sstream;
        sstream << std::hex << offset;
        std::string command =  "addr2line -e " + std::string(buf) + " " + sstream.str();
        system(command.c_str());
    }
}

void one()
{
    print_backtrace();
}

// Required addr2line
// g++ -g main.cpp
int main(int argc, char **argv) 
{
    one();
}

