#pragma once

#ifndef TESTSUITE

// Real AVR build (not used in tests)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#else

// Test build
#include "arch/test/hw_platform.h"

#endif

