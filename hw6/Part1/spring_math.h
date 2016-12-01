/*
 This contains ugly output from Mathematica solutions, for use with the
 double spring pendulum problem.  No effort was made to prettify this code.
*/

#ifndef DOUBLE_SPRING
#define DOUBLE_SPRING

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
static void populateFromM1(Spring_Pendulum *m) {
    x1k = m.x;
    y1k = m.y;
    m1  = m.m;
    k1  = m.k;
    l1  = m.rl;
    P1X = m.px;
    P1Y = m.py;
}
static void populateFromM2(Spring_Pendulum *m1, Spring_Pendulum *m2) {
    populateFromM1(m1);
    x2k = m2.x;
    y2k = m2.y;
    m2  = m2.m;
    k2  = m2.k;
    l2  = m2.rl;
    P2X = m2.px;
    P2Y = m2.py;
}

// Function to compute unknowns (anything with a 1 at the end) for double
// spring pendulum system
static void computeDouble(Spring_Pendulum *m1, Spring_Pendulum *m2) {
    // Populate based on m1 and m2
    populateFromM1(m1);
    populateFromM2(m2);

    // Mass 1 momentum at time k+1 in x direction
    m1->px = P1X + dt*(k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))) - 
      (k2*(x1k - x2k)*(-l2 + Sqrt(Power(x1k,2) - 2*x1k*x2k + 
             Power(x2k,2) + Power(y1k - y2k,2))))/
       Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2)));

    // Mass 1 momentum at time k+1 in y direction
    m1->py = dt*g*m1 + P1Y - dt*k1*y1k - dt*k2*y1k + 
   (dt*k1*l1*y1k)/Sqrt(Power(x1k,2) + Power(y1k,2)) + 
   (dt*k2*l2*y1k)/
    Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2)) + 
   dt*k2*y2k - (dt*k2*l2*y2k)/
    Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2));

    // Mass 2 momentum at time k+1 in x direction
    m2->px = P2X + (dt*k2*(x1k - x2k)*(-l2 + 
        Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2))
        ))/Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + 
      Power(y1k - y2k,2));

    // Mass 2 momentum at time k+1 in y direction
    m2->py = P2Y + dt*(g*m2 + (k2*(-l2 + Sqrt(Power(x1k,2) - 2*x1k*x2k + 
             Power(x2k,2) + Power(y1k - y2k,2)))*(y1k - y2k))/
       Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2)));

    // Mass 1 x positiong at time k+1
    m1->x = (dt*(P1X + P2X) + (m1 + m2)*x1k + 
     Power(dt,2)*k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))))/
   (m1 + m2);

    // Mass 1 y positiong at time k+1
    m1->y = (dt*(P1Y + P2Y) + (m1 + m2)*y1k + 
     Power(dt,2)*(g*(m1 + m2) + 
        k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2)))))/(m1 + m2);

    // Mass 2 x positiong at time k+1
    m2->x = (Power(dt,2)*(k2*(m1 + m2)*(x1k - x2k)*Sqrt(Power(x1k,2) + Power(y1k,2))*
         (-l2 + Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + 
             Power(y1k - y2k,2))) + 
        k1*m2*x1k*(l1 - Sqrt(Power(x1k,2) + Power(y1k,2)))*
         Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + 
           Power(y1k - y2k,2))) + 
     dt*(m1*P2X + m2*(P1X + 2*P2X))*Sqrt(Power(x1k,2) + Power(y1k,2))*
      Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2))\
      + m2*(m1 + m2)*x2k*Sqrt(Power(x1k,2) + Power(y1k,2))*
      Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2)))/
   (m2*(m1 + m2)*Sqrt(Power(x1k,2) + Power(y1k,2))*
     Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)));

    // Mass 2 y positiong at time k+1
    m2->y = (Power(dt,2)*(m2*(2*g*(m1 + m2)*Sqrt(Power(x1k,2) + Power(y1k,2)) + 
           k1*y1k*(l1 - Sqrt(Power(x1k,2) + Power(y1k,2))))*
         Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + 
           Power(y1k - y2k,2)) + 
        k2*(m1 + m2)*Sqrt(Power(x1k,2) + Power(y1k,2))*
         (-l2 + Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + 
             Power(y1k - y2k,2)))*(y1k - y2k)) + 
     dt*(m1*P2Y + m2*(P1Y + 2*P2Y))*Sqrt(Power(x1k,2) + Power(y1k,2))*
      Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2))\
      + m2*(m1 + m2)*Sqrt(Power(x1k,2) + Power(y1k,2))*
      Sqrt(Power(x1k,2) - 2*x1k*x2k + Power(x2k,2) + Power(y1k - y2k,2))*
      y2k)/(m2*(m1 + m2)*Sqrt(Power(x1k,2) + Power(y1k,2))*
     Sqrt(Power(x1k - x2k,2) + Power(y1k - y2k,2)));
}

// Function to compute unknowns (anything with a 1 at the end) for single
// spring pendulum system
static void computeSingle(Spring_Pendulum *m1) {
    // Populate based on m1
    populateFromM1(m1);

    // Momentum in x direction at time k+1
    m1->px = -P1X + dt*k1*x1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2)));

    // Momentum in y direction at time k+1
    m1->y1 = -P1Y + dt*(g*m1 + k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2))));

    // Mass x position at time k+1
    m1->x = -((dt*P1X - m1*x1k + Power(dt,2)*k1*x1k*(1 - l1/Sqrt(Power(x1k,2) + Power(y1k,2))))/m1);

    // Mass y position at time k+1
    m1->y = (-(dt*P1Y) + m1*y1k + Power(dt,2)*(g*m1 + k1*y1k*(-1 + l1/Sqrt(Power(x1k,2) + Power(y1k,2)))))/m1;
}

#endif // ifndef DOUBLE_SPRING
