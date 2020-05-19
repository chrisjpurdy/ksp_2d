#include "header.h"
#include "templates.h"
#include "KSP2D.h"

#include <ctime>

// These are passed to initialise to determine the window size
const int BaseScreenWidth = 1300;
const int BaseScreenHeight = 800;

int doProgram(int argc, char *argv[])
{
    int iResult = 0;

    KSP2D oMain;

    char buf[1024];
    sprintf(buf, "C++ Coursework Framework Program : Size %d x %d", BaseScreenWidth, BaseScreenHeight);
    iResult = oMain.initialise(buf, BaseScreenWidth, BaseScreenHeight, "kenpixel.ttf", 24);

    iResult = oMain.mainLoop();
    oMain.deinitialise();
    return iResult;
} // Main object gets destroyed at this point, so it will free its memory

/* Separate main function, so we can check for memory leaks after objects are destroyed */
int main(int argc, char *argv[])
{
    // Send random number generator with current time
    ::srand( (unsigned int)time(0));

    int iResult = doProgram( argc, argv );

    // Free the cached images by destroying the image manager
    // Ensure that you do this AFTER the main object and any other objects have been destroyed
    // The game object is a stack object inside doProgram() so will have been
    ImageManager::destroyImageManager();

// Detect memory leaks on windows if building for debug (not release!)
#if defined(_MSC_VER)
    #ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
#endif

    return iResult;
}
