#pragma once

#include "math.h"


class SimpleTrig
{
	double cosCache[360];

	SimpleTrig()
	{
		for (int i = 0; i < 360; i++)
			cosCache[i] = ::cos(i);
	}

	// Get cosine for integer angle to avoid the function call
	double cos(int angle) { return cosCache[(angle + 720) % 360]; }
	double sin(int angle) { return cosCache[(angle + 630) % 360]; } // cos of 90 degrees less
};
