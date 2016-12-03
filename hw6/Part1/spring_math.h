/*
 This contains ugly output from Mathematica solutions, for use with the
 double spring pendulum problem.  No effort was made to prettify this code.
*/

#ifndef SPRING_MATH
#define SPRING_MATH

#include <math.h>

#include "pendulum.h"

static const float dt = 0.01;
static const float g = -9.8;

// three or four characters:
//    first:  Cartesian direction x or y, or P for momentum
//    second: Mass 1 or Mass 2
//    third:  irrelevant
//    fourth: 1 implies k+1
static float x1k = 0;
static float y1k = 0;
static float x2k = 0;
static float y2k = 0;
static float x1k1;
static float y1k1;
static float x2k1;
static float y2k1;
// momentums
static float P1X = 0;
static float P1Y = 0;
static float P2X = 0;
static float P2Y = 0;
static float P1X1;
static float P1Y1;
static float P2X1;
static float P2Y1;

// m -> mass
// k -> spring constant
// l -> resting length
// 1 or 2 -> mass 1 or 2 respectively
static float m1 = 0;
static float k1 = 0;
static float l1 = 0;
static float m2 = 0;
static float k2 = 0;
static float l2 = 0;

// Wrapper for Mathematica functions Sqrt and Power
static float Sqrt(float val) {
    return sqrt(val);
}
static float Power(float base, float exponent) {
    if (exponent == 2)
        return base * base;
    return pow(base, exponent);
}

// Populate global values
static void populateFromM1(Spring_Pendulum *sp) {
    x1k = sp->x;
    y1k = sp->y;
    m1  = sp->m;
    k1  = sp->k;
    l1  = sp->rl;
    P1X = sp->px;
    P1Y = sp->py;
}
static void populateFromM2(Spring_Pendulum *sp) {
    x2k = sp->x;
    y2k = sp->y;
    m2  = sp->m;
    k2  = sp->k;
    l2  = sp->rl;
    P2X = sp->px;
    P2Y = sp->py;
}

// Function to compute unknowns (anything with a 1 at the end) for double
// spring pendulum system
static void computeDouble(Spring_Pendulum *sp1, Spring_Pendulum *sp2) {
    // Populate based on m1 and m2
    populateFromM1(sp1);
    populateFromM2(sp2);

    // Mass 1 momentum at time k+1 in x direction
    sp1->px = P1X + dt*k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))) +
   (dt*k2*(-x1k + x2k)*(-l2 +
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/
    Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2));

    // Mass 1 momentum at time k+1 in y direction
    sp1->py = dt*g*m1 + P1Y + dt*k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))) +
   (dt*k2*(-l2 + Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)))*
      (-y1k + y2k))/Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2));

    // Mass 2 momentum at time k+1 in x direction
    sp2->px = P2X + (dt*k2*(x1k - x2k)*(-l2 +
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/
    Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2));

    // Mass 2 momentum at time k+1 in y direction
    sp2->py = dt*g*m2 + P2Y + (dt*k2*(-l2 +
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)))*(y1k - y2k))/
    Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2));

    // Mass 1 x positiong at time k+1
    sp1->x = (dt*(P1X + (m1*x1k)/dt + dt*k1*x1k*
        (-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))) +
       (dt*k2*(-x1k + x2k)*(-l2 +
            Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/m1;

    // Mass 1 y positiong at time k+1
    sp1->y = (dt*(dt*g*m1 + P1Y + (m1*y1k)/dt +
       dt*k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))) +
       (dt*k2*(-l2 + Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)))*
          (-y1k + y2k))/Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/m1;

    // Mass 2 x positiong at time k+1
    sp2->x = (dt*(P2X + (m2*x2k)/dt + (dt*k2*(x1k - x2k)*
          (-l2 + Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2))))/m2;

    // Mass 2 y positiong at time k+1
    sp2->y = (dt*(dt*g*m2 + P2Y + (dt*k2*(-l2 +
            Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)))*(y1k - y2k))/
        Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)) + (m2*y2k)/dt))/m2;
}

// Function to compute unknowns (anything with a 1 at the end) for single
// spring pendulum system
static void computeSingle(Spring_Pendulum *sp1) {
    // Populate based on m1
    populateFromM1(sp1);

    // Momentum in x direction at time k+1
    sp1->px = P1X + dt*k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2)));

    // Momentum in y direction at time k+1
    sp1->py = P1Y + dt*(g*m1 + k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))));

    // Mass x position at time k+1
    sp1->x = (dt*P1X + m1*x1k + Power(dt,2)*k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))))/m1;

    // Mass y position at time k+1
    sp1->y = (dt*P1Y + m1*y1k + Power(dt,2)*(g*m1 + k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2)))))/m1;
}

#endif // ifndef SPRING_MATH
