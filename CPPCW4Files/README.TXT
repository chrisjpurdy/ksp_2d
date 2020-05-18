G52CPP coursework parts 3 and 4
================================

This readme includes instructions for various different platforms since you suggested that you may want to use different platforms. 
Please note that we can support you on Windows, e.g. helping out if there is some configuration issue, but you should only use a different platform if you think you can do so without needing our support for it!
i.e. the different options are provided for your convenience, at your own risk.

You can demo the coursework using any platform you wish - just make sure it works when you demo it.
It should be possible to take your files to any other platform and they just run.
If you want to submit to the hall of fame, please recompile the code on the lab PCs at that time, and provide me an exe for windows.

IMPORTANT: If you change platforms or versions of Visual Studio after you have added files, you will need to remember to add them to the project / makefiles that you are using.


Windows:
========

If you are running on Visual Studio 2017 (e.g. lab pcs), that is the default, so just run the CourseworkBase2020_vs2017.sln file which is in this directory.
This is set up to use Visual Studio 2017. You can probably migrate it to later versions if you wish.

When you build, the output executables will be placed in the Debug and Release directories in this folder.
When you run the program from within visual studio, it will look for things like the fonts and images in the src directory.
You MUST have the SDL dll files in your debug and release directories if you want to be able to run it. e.g. copy everything from the SDL/SDL2_dlls_x86 directory into your debug and release directories and it will find the SDL dlls that it needs to run.
I put the files there so that you don't need to install SDL on your computer - it will find these files instead.


Proper Configuration for Mac or Linux
=====================================

Dr Steven Bagley set up a cmake system for me which should work to build you makefiles which are configured for your own system. 
In theory these should work for you regardless of your platform. His instructions follow:

This uses cmake to find the required libraries and build the relevant project files for your preferred build system. You will need to have installed cmake (cmake.org) along with the relevant SDL libraries (SDL2, SDL2_ttf and SDL2_image). These exist for Windows, Mac and Linux and should be installed in the appropriate manner.

1) Install the SDL development libraries mentioned above

2) Download and unzip the supplied zip file

3) Move to the build directory, e.g. cd <somepath>/CPPCoursework2020/build

4) Generate the build files for the project. On a Mac, assuming you've followed the steps above you would execute the following:

cmake -G Xcode ..

which would generate a G52CPP.xcodeproj that can be opened in XCode to build the project. 
(Note: the .. means to look in the parent directory for the CMakeLists.txt file and the cmake directory.)

Under linux, make files can be created using:

cmake -G 'Unix Makefiles' ..

instead (note the single quotation marks), which will generate a Makefile that can be used to build the project. 

You can then edit the files and project/Makefiles as normal to complete your coursework. 

Similar generators exist for cmake to create build files for VisualStudio, Eclipse etc.


Linux or Mac Quick Run
======================

In the Linux_Simplistic_Makefile and Mac_simplistic_makefile directories are two simplistic makefiles. Don't use these longterm, but if you have cmake issues, they may help you out until you fix cmake and generate a reasonable makefile.
These are simplistic because they will not take account of dependencies correctly. e.g. a good makefile should ensure that if you change a header file the files which include it are recompiled. This will not do that.
1) Install your sdl devel files - for SDL2, SDL2_ttf and SDL2_image.
2) Copy the relevant makefile to your src directory.
3) Run make.

Note: the files are set up to use ../SDL as the directory with header files in. You may want to change this to use the SDL header files that you installed instead.
The mac makefile works on the machines in the maclab - BUT it is a quick Hack to allow me to test the source code before cmake was installed. DO NOT USE THESE LONGTERM - instead use cmake to generate a better makefile.

