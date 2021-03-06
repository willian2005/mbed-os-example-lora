/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Serial.h"
#include "rtos/Mutex.h"

/**
 * If we have tracing library available, we can see traces from within the
 * stack. The library could be made unavailable by removing FEATURE_COMMON_PAL
 * from the mbed_app.json to save RAM.
 */
#if defined(FEATURE_COMMON_PAL)
 #include "mbed_trace.h"
#else
//dummies if feature common pal is not added
 #define mbed_trace_mutex_wait_function_set(...) (void(0))
 #define mbed_trace_mutex_release_function_set(...) (void(0))
 #define mbed_trace_init(...) (void(0))
 #define mbed_trace_print_function_set(...) (void(0))
#endif //defined(FEATURE_COMMON_PAL)

using namespace mbed;
using namespace rtos;

/**
 * Serial object for console tracing
 */
static Serial pc(USBTX, USBRX);

/**
 * Local mutex object for synchronization
 */
static Mutex mutex;

static void serial_lock();
static void serial_unlock();
static void trace_printer(const char* str);

/**
 * Sets up trace for the application
 * Wouldn't do anything if the FEATURE_COMMON_PAL is not added
 * or if the trace is disabled using mbed_app.json
 */
void setup_trace()
{
    // setting up baudrate for console printer
    pc.baud(115200);
    // setting up Mbed trace. Doesn't do anything if
    // FEATURE_COMMON_PAL is not available
    mbed_trace_mutex_wait_function_set(serial_lock);
    mbed_trace_mutex_release_function_set(serial_unlock);
    mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);
}

/**
 * Lock provided for serial printing used by trace library
 */
static void serial_lock()
{
    mutex.lock();
}

/**
 * Releasing lock provided for serial printing used by trace library
 */
static void serial_unlock()
{
    mutex.unlock();
}

/**
 * Prints the Mbed trace, used by trace library.
 * Not intended for local use.
 */
static void trace_printer(const char* str)
{
    printf("%s\r\n", str);
}

