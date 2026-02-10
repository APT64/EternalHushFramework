#pragma once
#include <Windows.h>
#include <iostream>
#include <dana.hpp>
#include <apidef.h>
#include <darknarrator.h>
#include <throttlestop.h>

static LAYER_ENTRY layer_set[] = {
	{DANA_THROTTLESTOP_LAYER, new ThrottleStopLayer()}
};