#pragma once

#include <vector>
#include <stdint.h>
#include <iostream>

namespace loss
{
    class DisplaySerialiser
    {
        public:
            DisplaySerialiser &operator % (uint8_t value);
            DisplaySerialiser &operator << (int32_t value);
            DisplaySerialiser &operator << (uint32_t value);

            DisplaySerialiser &operator << (bool value);

            DisplaySerialiser &operator << (const char *name);
            DisplaySerialiser &operator << (const std::string &name);

            const std::vector<uint8_t> &data() const;

            void display() const;

        private:
            std::vector<uint8_t> _data;

            inline void push_mixedbit(uint8_t hibit, uint8_t lowbit)
            {
                _data.push_back(((hibit & 0x0F) << 4) | lowbit);
            }

            inline void push_32uint(uint32_t value)
            {
                _data.push_back((value >> 24) & 0xFF);
                _data.push_back((value >> 16) & 0xFF);
                _data.push_back((value >> 8)  & 0xFF);
                _data.push_back(value & 0xFF);
            }
            inline void push_16uint(uint16_t value)
            {
                _data.push_back((value >> 8) & 0xFF);
                _data.push_back(value & 0xFF);
            }
    };
}
