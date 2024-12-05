#pragma once




class nocopy
{
public:
    nocopy() {};
    const nocopy& operator=(const nocopy &) = delete;
    nocopy(const nocopy &) = delete;
};