#include "display_serialiser.h"

namespace loss
{
    DisplaySerialiser &DisplaySerialiser::operator % (uint8_t value)
    {
        _data.push_back(value);
        return *this;
    }

    DisplaySerialiser &DisplaySerialiser::operator << (int32_t value)
    {
        if (value <= 0x07 && value >= -0x08)
        {
            if (value < 0)
            {
                auto v = ((value & 0x07) << 4) | 0x81;
                _data.push_back(v);
            }
            else
            {
                auto v = ((value & 0x07) << 4) | 0x01;
                _data.push_back(v);
            }
        }
        else if (value <= 0x7F && value >= -0x80)
        {
            _data.push_back(0x03);
            _data.push_back((int8_t)value);
        }
        else if (value <= 0x7FFF && value >= -0x8000)
        {
            _data.push_back(0x13);
            auto short_value = (int16_t)value;
            push_16uint(short_value);
        }
        else
        {
            _data.push_back(0x23);
            push_32uint(value);
        }
        return *this;
    }

    DisplaySerialiser &DisplaySerialiser::operator << (uint32_t value)
    {
        if (value <= 0x0F)
        {
            push_mixedbit(value, 0x02);
        }
        else if (value <= 0xFF)
        {
            _data.push_back(0x04);
            _data.push_back(value & 0xFF);
        }
        else if (value <= 0xFFFF)
        {
            _data.push_back(0x14);
            push_16uint(value);
        }
        else
        {
            _data.push_back(0x24);
            push_32uint(value);
        }
        return *this;
    }

    DisplaySerialiser &DisplaySerialiser::operator << (bool value)
    {
        _data.push_back(value ? 0x18 : 0x08);
        return *this;
    }

    DisplaySerialiser &DisplaySerialiser::operator << (const char *name)
    {
        return (*this << std::string(name));
    }
    DisplaySerialiser &DisplaySerialiser::operator << (const std::string &name)
    {
        auto size = name.size(); 
        if (size < 16)
        {
            push_mixedbit(size, 0x05);
            for (auto i = 0u; i < size; i++)
            {
                _data.push_back((uint8_t)name[i]);
            }
        }
        else
        {
            _data.push_back(0x06);
            push_32uint(size);
            for (auto i = 0u; i < size; i++)
            {
                _data.push_back((uint8_t)name[i]);
            }
        }
        return *this;
    }

    const std::vector<uint8_t> &DisplaySerialiser::data() const
    {
        return _data;
    }

    void DisplaySerialiser::display() const
    {
        std::cout << std::hex;
        for (auto &d : _data)
        {
            std::cout << (int)d << ' ';
        }
        std::cout << '\n';
    }

}
