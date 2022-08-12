#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xrender.h>

#ifndef __XTEMP_H
#define __XTEMP_H

struct RGB
{
    double r, g, b;
};

double getRfromTemp (unsigned short ncolor);
double getGfromTemp (unsigned short ncolor);
double getBfromTemp (unsigned short ncolor);
struct RGB getRGBfromTemp (unsigned short color);
void colortemp_set (unsigned short);
void xtemp_set(unsigned short);

#endif /* __XTEMP_H */
