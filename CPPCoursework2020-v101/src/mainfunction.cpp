#include "header.h"

#include <ctime>

#include "templates.h"

// Needs one of the following #includes, to include the class definition
#include "SimpleDemo.h"
#include "BouncingBallMain.h"
#include "MazeDemoMain.h"
#include "DraggingDemo.h"
#include "ZoomingDemo.h"
#include "ImageObjectDemo.h"
#include "StarfieldDemo.h"
#include "FlashingDemo.h"
#include "ImageMappingDemo.h"
#include "JigsawDemo.h"
#include "PlayingCardsDemo.h"
#include "JasonsDemoA.h"

// These are passed to initialise to determine the window size
const int BaseScreenWidth = 1300;
const int BaseScreenHeight = 800;


// This was only moved outside of main so that I can do some memory checking once it ends.
// Main calls this then checks for errors before ending.
int doProgram(int argc, char *argv[])
{ 
	int iResult = 0;
	// Needs just one of the following lines - choose which object to create - ENSURE ONLY ONE IS CREATED.
	// Note these are from simplest to most complex demo.
	JasonsDemoA oMain; // Jason's version of MyDemoA, MyObjectA and MyTileManagerA
	//SimpleDemo oMain;
	//ImageObjectDemo oMain; // Shows drawing images to the foreground and background
	//BouncingBallMain oMain; // A ball bouncing around with images and shapes drawn
	//MazeDemoMain oMain; // Demonstrates a tile manager being used and updated
	//StarfieldDemo oMain; // Does a simple scrolling background
	
	//DraggingDemo oMain; // Includes dragable images and image objects
	//ImageMappingDemo oMain; // Demonstrates the use of coordinate mapping for image drawing
	//JigsawDemo oMain; // Demonstrates the use of coordinate mapping for image drawing
	//PlayingCardsDemo oMain; // The most advanced demo - you may want to ignore this one totally!

	// WARNING: DO NOT RUN THIS DEMO IF YOU ARE AFFECTED BY FLASHING COLOURS!!!
	//FlashingDemo oMain; // See WARNING above! Simple changing backgrounds
	//ZoomingDemo oMain;	// WARNING: More flashing here! Shows the use of a filter to shift and rescale the display - this is complex and you can ignore it!

	char buf[1024];
	sprintf(buf, "C++ Coursework Framework Program : Size %d x %d", BaseScreenWidth, BaseScreenHeight);
	iResult = oMain.initialise(buf, BaseScreenWidth, BaseScreenHeight, "Cornerstone Regular.ttf", 24);

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
