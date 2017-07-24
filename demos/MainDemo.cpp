// --------------------------------------------------------------------
// <lp3/main.hpp> example
// --------------------------------------------------------------------
//      This shows using `<lp3/main.hpp>` to quickly create a simple
//      app.
// --------------------------------------------------------------------
#include <lp3/main.hpp>
#include <iostream>

#include <lp3/log.hpp>
#include <lp3/core.hpp>


namespace core = lp3::core;


int _main(core::PlatformLoop & loop) {
    lp3::core::LogSystem log;

    LP3_LOG_DEBUG("Greetings from the MicroMain Demo.");
    for(const std::string & element : loop.command_line_args()) {
        LP3_LOG_DEBUG(element.c_str());
        #ifdef LP3_COMPILE_LOG_DISABLE
            std::cout << element << "\n";
        #endif
    }

    int iteration = 0;

    const auto result = loop.run([&]() {
        // This gets called each frame.

        LP3_LOG_DEBUG("iteration = %d", iteration);
		return false;
    });

    LP3_LOG_DEBUG("Good bye.");
    return result;
}

LP3_MAIN(_main)
