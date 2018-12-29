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

struct RGB {
    double r, g, b;
};

double
getRfromTemp (unsigned short ncolor);

double
getGfromTemp (unsigned short ncolor);

double
getBfromTemp (unsigned short ncolor);

struct RGB
getRGBfromTemp (unsigned short color);

void colortemp_set (unsigned short);

void xtemp_set(unsigned short);

double
getRfromTemp (unsigned short ncolor) {
    double calcd_temp = 329.698727446 * pow(ncolor, -0.1332047592);

    return (ncolor <= 66 || calcd_temp > 255) ? 255 :
                            calcd_temp < 0    ? 0   : calcd_temp;
}

double
getGfromTemp (unsigned short ncolor) {
    double lcalcd = 99.4708025861  * log(ncolor) - 161.1195681661,
           ucalcd = 288.1221695283 * pow(ncolor, -0.0755148492);

    return ncolor <= 66 ? (lcalcd <= 0 ? 0 : lcalcd > 255 ? 255 : lcalcd)
                        : (ucalcd <= 0 ? 0 : ucalcd > 255 ? 255 : ucalcd);
}

double
getBfromTemp (unsigned short ncolor) {
    double calcd_temp = 138.5177312231 * log(ncolor) - 305.0447927307;

    return (ncolor <= 19 || calcd_temp < 0)   ? 0   :
           (ncolor >= 66 || calcd_temp > 255) ? 255 : calcd_temp;
}

// valid range: [1000..15000]
// effective range: [1000,1100..15000]
// original algorithm:
// http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
struct RGB
getRGBfromTemp (unsigned short color) {
    unsigned short ncolor = (color < 1000  ? 1000  :
                             color > 15000 ? 15000 : color) / 100;

    return (struct RGB ){ .r = getRfromTemp(ncolor)
                        , .g = getGfromTemp(ncolor)
                        , .b = getBfromTemp(ncolor)
                        };
}

void
xtemp_set (unsigned short temp) {

    Display * dpy = XOpenDisplay(NULL);
    signed screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    XRRScreenResources * res = XRRGetScreenResourcesCurrent(dpy, root);
    struct RGB rgb = getRGBfromTemp(temp);

    double gr = rgb.r / 255.0, gg = rgb.g / 255.0, gb = rgb.b / 255.0;

    for ( signed c = 0; c < res->ncrtc; ++ c ) {
        RRCrtc crtcxid = res->crtcs[c];
        signed size = XRRGetCrtcGammaSize(dpy, crtcxid);
        XRRCrtcGamma * crtc_gamma = XRRAllocGamma(size);
        for ( signed i = 0; i < size; ++ i ) {
            double g = 65535.0 * i / size;
            crtc_gamma->red[i]   = (unsigned short )(g * gr);
            crtc_gamma->green[i] = (unsigned short )(g * gg);
            crtc_gamma->blue[i]  = (unsigned short )(g * gb);
        } XRRSetCrtcGamma(dpy, crtcxid, crtc_gamma); XFree(crtc_gamma);
    }
}

static const char vos [] = "hrt:";

static struct option os [] = {
    { "help",       0, 0, 'h' },
    { "reset-temp", 0, 0, 'r' },
    { "set-temp",   1, 0, 't' },
    { 0,            0, 0, 0   }
};

static const char usage [] =
    "Usage: xtemp [options]\n"
    "xtemp -- A oneshot screen temperature adjustor for X.\n\n"
    "Options:\n"
    "  -h,     --help            List this help and exit\n"
    "  -r,     --reset-temp      Reset the color temperature to the default\n"
    "  -t NUM, --set-temp=NUM    Set the screen's color temperature to NUM: [1000..5000..15000]\n";

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fputs(usage, stderr);
        return EXIT_FAILURE;
    }

    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1;       c = getopt_long(argc, argv, vos, os, &oi)  ) {

        switch ( c ) {
            case 'h': fputs(usage, stdout); break;
            case 'r': xtemp_set(6600);  break;
            case 't': {
                unsigned short temp = 0;
                if ( !sscanf(optarg, "%hu", &temp) ) {
                    fputs("xtemp: Failed to read temperature\n", stderr);
                    return EXIT_FAILURE;
                }
                if ( temp < 1000 || temp > 15000 ) {
                    fputs("xtemp: Value out of range\n", stderr);
                    return EXIT_FAILURE;
                } xtemp_set(temp);
            } break;

            default: fputs(usage, stderr); return EXIT_FAILURE;
        }
    } return EXIT_SUCCESS;
}
