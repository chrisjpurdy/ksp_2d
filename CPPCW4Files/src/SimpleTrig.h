#pragma once

#include "math.h"

class SimpleTrig
{
public:

	// Get cosine for integer angle to avoid the function call
	double cos(int anglex10) {
	    if (anglex10 < 0)
	        anglex10 = anglex10 * -1;
	    return cosCache[(anglex10 + 7200) % 3600];
	}
	double sin(int anglex10) {
        if (anglex10 < 0) {
            anglex10 = anglex10 * -1;
            return cosCache[(anglex10 + 6300) % 3600] * -1;
        }
	    return cosCache[(anglex10 + 6300) % 3600];
	} // cos of 90 degrees less

	static SimpleTrig* get() {
	    return singleton;
	}

	static void init() {
	    singleton = new SimpleTrig();
	}

	static void uninit() {
	    delete singleton;
	}

private:
    static SimpleTrig* singleton;
    double cosCache[3600];

    SimpleTrig()
    {
        for (int i = 0; i < 3600; i++) {
            cosCache[i] = ::cos((i / 10.0) * (3.1415927 / 180.0));
        }
    }
};