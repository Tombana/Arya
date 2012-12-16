#pragma once
#include "../../ext/buffer.hpp"
#include <vector>
#include <string>
#include <vector>

using std::vector;
using std::vector;

using std::vector;
using std::string;

#define PACKETMAGICINT (('A' << 0) | ('r' << 8) | ('P' << 16) | ('k' << 24))

//The first int is always the magic int
//The second int is the size
//The thirs int is the event ID

class Packet
{
    private:
        friend class Connection;
        friend class Network;

        //For creating packets (for sending)
        Packet(int id) : data(12, 32), readPos(12), markedForSend(false) //allocate 32 bytes by default
        {
            *(int*)&data[0] = PACKETMAGICINT;
            *(int*)&data[4] = 12;
            *(int*)&data[8] = id;
        }

        //For receiving packets
        Packet(char* databuf, int size) : data(databuf, size), readPos(12), markedForSend(false) {};

        ~Packet(){};

        //When getting the data, for writing it to the network, we write the size in the buffer
        char* getData() { *(int*)&data[4] = getSize(); return data.data(); }

    public:
        int getId() { return *(int*)&data[8]; }

        int getSize() const { return data.size(); }

        void send() { markedForSend = true; }

        //READ functions

        inline Packet& operator>>(int val)
        {
            if(readPos + sizeof(int) <= data.size())
            {
                val = *(int*)&data[readPos];
                readPos += sizeof(int);
            }
            return *this;
        }

        //WRITE functions

        inline Packet& operator<<(unsigned int val)
        {
            data.append(&val, sizeof(unsigned int));
            return *this;
        }

        inline Packet& operator<<(int val)
        {
            data.append(&val, sizeof(int));
            return *this;
        }

        inline Packet& operator<<(char val)
        {
            data.append(&val, sizeof(char));
            return *this;
        }

        inline Packet& operator<<(const std::string& str)
        {
            data.append(str.c_str(), str.size()+1);
            return *this;
        }

        inline Packet& operator<<(const std::vector<int>& val)
        {
            *this << (unsigned int)val.size();
            if(!val.empty()) data.append(&val.front(), sizeof(int)*val.size());
            return *this;
        }

    private:
        buffer data;
        int readPos;
        bool markedForSend;
};
