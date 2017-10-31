#ifndef IMGINIT_H
#define IMGINIT_H


#include <SDL.h>
#include <SDL_image.h>
#define SDL_MESSAGEBOX_EXIT 16

struct Imginit
{
    Imginit();
    ~Imginit();
const int PNGON = 0;
const int JPGON = 0;
//int INITPNG = IMG_Init(IMG_INIT_PNG);
void PNGInit()const;
void JPGInit()const;



};
#endif
