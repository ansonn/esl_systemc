
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#include "ext_mem_model.h"


ExtMemModel::ExtMemModel(unsigned int size_in_bytes)
{
    cout << "ExtMemModel::ExtMemModel() : Constructing Model" << endl;

    memory_size_in_bytes = size_in_bytes;
    memory = (unsigned char *) malloc(memory_size_in_bytes);

    if (memory == NULL)
        cout << "ExtMemModel::ExtMemModel() : ERROR! malloc failed" << endl;
}

ExtMemModel::~ExtMemModel()
{
    if (memory != NULL)
        delete memory;
}

unsigned int ExtMemModel::read8(unsigned int addr)
{
    unsigned int data;
    data = (unsigned int) memory[addr];
    return data;
}

unsigned int ExtMemModel::read16(unsigned int addr)
{
    unsigned int data;
    data = (((unsigned int) memory[addr+0]) << 0) | (((unsigned int) memory[addr+1]) << 8);
    return data;
}

unsigned int ExtMemModel::read32(unsigned int addr)
{
    unsigned int data;
    data = (((unsigned int) memory[addr+0]) << 0)  | (((unsigned int) memory[addr+1]) << 8) |
           (((unsigned int) memory[addr+2]) << 16) | (((unsigned int) memory[addr+3]) << 24);
    return data;
}

void ExtMemModel::write8(unsigned int addr, unsigned int data)
{
    memory[addr+0] = (unsigned char) (data >> 0);
}

void ExtMemModel::write16(unsigned int addr, unsigned int data)
{
    memory[addr+0] = (unsigned char) (data >> 0);
    memory[addr+1] = (unsigned char) (data >> 8);
}

void ExtMemModel::write32(unsigned int addr, unsigned int data)
{
    memory[addr+0] = (unsigned char) (data >> 0);
    memory[addr+1] = (unsigned char) (data >> 8);
    memory[addr+2] = (unsigned char) (data >> 16);
    memory[addr+3] = (unsigned char) (data >> 24);
}

void ExtMemModel::reset()
{
    cout << "ExtMemModel::reset() : Clearing contents of memory" << endl;

    if (memory != NULL)
        memset(memory, 0, memory_size_in_bytes);
}


