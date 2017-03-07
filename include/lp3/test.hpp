#ifndef FILE_LP3_TEST
#define FILE_LP3_TEST

#include <lp3/le.hpp>

#ifndef LP3_TEST_MODULE
    #error "LP3_TEST_MODULE must be defined at the top of the file."
#endif

// From this point forward, the code is compiled as if it's test code.
#define LP3_COMPILE_TARGET_TEST

#define ___STR_VALUE(arg) #arg
#define LP3_TEST_MODULE_C_STR ___STR_VALUE(LP3_TEST_MODULE)

#ifdef _arch_dreamcast
  #ifndef LP3_COMPILE_TARGET_DREAMCAST
    #define LP3_COMPILE_TARGET_DREAMCAST
  #endif
#endif


/*----------------------------------------------------------------------------
 *   LP3_TEST function registration
 *---------------------------------------------------------------------------*/
#ifdef LP3_COMPILE_TARGET_USE_BOOST_TEST
    /*------------------------------------------------------------------------
     *  PC- just use Boost!
     *-----------------------------------------------------------------------*/

    // If we're on the PC, just use Boost Test!

    // Sometimes this is needed, sometimes not.
    // Boost Test is the hardest libary to link with in the world.
    #ifdef LP3_COMPILE_TARGET_WINDOWS
       // #define BOOST_TEST_DYN_LINK
    #endif
    #define BOOST_TEST_MODULE LP3_TEST_MODULE

	#pragma warning(push, 0)
    #include <boost/test/unit_test.hpp>
	#pragma warning(pop)

    #define LP3_TEST BOOST_AUTO_TEST_CASE
#else
    /*------------------------------------------------------------------------
     *  Non-PC custom stuff
     *-----------------------------------------------------------------------*/
    // If we're not on the PC, get ready for some pain!
    #include <vector>

    using LP3_TEST_FUNCTION_TYPE = void(*)();
    std::vector<LP3_TEST_FUNCTION_TYPE> LP3_TEST_INTERNAL_LIST;

    struct LP3_TEST_REGISTER {
        LP3_TEST_REGISTER(LP3_TEST_FUNCTION_TYPE fn) {
            LP3_TEST_INTERNAL_LIST.push_back(fn);
        }
    };

    #define LP3_TEST_UNIQUE_NAME0(x) LP3_TEST_FUNCTION_INSTANCE ## x
    #define LP3_TEST_UNIQUE_NAME(x) LP3_TEST_UNIQUE_NAME0(x)

    // BOOST_AUTO_TEST_CASE style automatic function registration.

    #define LP3_TEST(function_name) \
        void function_name (); \
        namespace { \
            LP3_TEST_REGISTER LP3_TEST_UNIQUE_NAME(__LINE__) (function_name); \
        } \
        void function_name()

    #define LP3_TEST_RUN_ALL() \
        for(auto fn : LP3_TEST_INTERNAL_LIST) { fn(); }


    namespace lp3 { namespace asserts {
        int error_count = 0;
    }   }

    #ifdef LP3_COMPILE_TARGET_DREAMCAST
        /*--------------------------------------------------------------------
         *  Dreamcast Main
         *-------------------------------------------------------------------*/
        #include <kos.h>
        #include <time.h>

        int main(int argc, char **argv) {
            lp3::asserts::error_count = 0;
            printf("@DC_TEST_START %s\n", LP3_TEST_MODULE_C_STR);
            LP3_TEST_RUN_ALL();
            printf("@DC_TEST_COMPLETE\n");
            return lp3::asserts::error_count;
        }

    #else
        /*--------------------------------------------------------------------
         *  Emscripten Main
         *-------------------------------------------------------------------*/
        #include <iostream>

        int main(int argc, char **argv) {
            lp3::asserts::error_count = 0;
            std::cout << "@LP3_TEST_START "
                      << LP3_TEST_MODULE_C_STR << "\n";
            LP3_TEST_RUN_ALL();
            std::cout << "@LP3_TEST_COMPLETE\n";
            return lp3::asserts::error_count;
        }
    #endif

#endif

#endif
