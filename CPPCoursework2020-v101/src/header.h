#if defined(_MSC_VER)

// Prevent getting warnings about unused inline functions etc
#pragma warning( disable : 4514 4710 4571 4774 5045 4668 4820 4625 4626 4996 5026 5027 )
// Temporary disable some warnings if building for WALL to find the other ones...
//#pragma warning( disable : 4100 4018 4365 5038 4355   )


// Only include in windows, when in debug:
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_surface.h"
#include "SDL_image.h"


#elif defined(__linux)

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_surface.h"
#include "SDL_image.h"

#else

#ifndef _SDL_H
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#endif


#endif




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include <iostream>
#include <string>

// These are used for backward compatibility with SDL 1.2 to get the keycodes to work
//#define SDL12_SDLK_FIRST 0
#define SDL12_SDLK_NORMAL 127
#define SDL12_SDLK_LAST 255


// If you compile on a platform where there are problems with SDL_BLIT then set this to 1 to avoid the blit operations (I hope)
#define AVOID_BLIT 0
// See drawingSurface.cpp for the code which checks this for copyRectangle()
// and Image2.cpp for the blit image code
// Note: At the moment, setting this may make the card player demo fail - cards may not appear. You should be fine as long as you avoid blits. 


// Checks for not making a mistake in surface locking when in debug compilation
#ifdef _DEBUG
#define CHECK_BLIT_SURFACE(a) a->checkBlitable()
#define CHECK_WRITE_SURFACE(a) a->checkWritable()
#else
#define CHECK_BLIT_SURFACE(a)
#define CHECK_WRITE_SURFACE(a)
#endif
