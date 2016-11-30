#ifndef PENDULUM
#define PENDULUM

struct Point
{
    float x;
    float y;
};

struct Color
{
    float r;
    float g;
    float b;
};

struct Spring_Pendulum
{
    float m;

    float x;
    float y;

    float px;
    float py;

    float k;
    float rl;
};

#endif // ifndef PENDULUM
