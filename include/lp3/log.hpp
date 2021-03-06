// ---------------------------------------------------------------------------
// Logging System
// ==============
//      Lp3 core uses a rudimentary logging system which has outlived most
//      dogs. This is disabled for non-debug builds, and can be disabled for
//      all builds by setting LP3_COMPILE_LOG_DISABLE.
//
//      By default this logs to stdout, but on Windows will send messages to
//      `the Squirrel Console <https://github.com/TimSimpson/SquirrelConsole>`_
//      if it is running (this only happens when the log system is explicitly
//      initialized using the ``LogSystem`` class before logging starts).
//
//      What log messages are printed to stdout can be controlled by setting
//      the environment variable LP3_LOG_LEVEL to the name of an ``LogLevel``
//      enumeration value.
//      For example "LP3_LOG_LEVEL=info" would cause messages with a LogLevel
//      of Info to be printed, but messages with a level of Debug to be
//      ignored.
//
// ~see-file "../../demos/LogDemo.cpp"
// ---------------------------------------------------------------------------/
// ~end-doc summary
#ifndef FILE_LP3_LOG_HPP
#define FILE_LP3_LOG_HPP
#pragma once

#include "core/config.hpp"

#include "le.hpp"

namespace lp3 { namespace core {

// ----------------------------------------------------------------------------
// enum class LogLevel
// ----------------------------------------------------------------------------
//      Defines the criticality of a logging message.
// ----------------------------------------------------------------------------
enum class LP3_CORE_API LogLevel
{
    Critical=0,
    Debug=4,
    Error=1,
    Info=3,
    Warning=2
};

// ----------------------------------------------------------------------------
// LP3_LOG_VAR(var)
// ----------------------------------------------------------------------------
//      Eliminate compiler warnings for variables that are used only by
//      log statements.
// ----------------------------------------------------------------------------

#define LP3_LOG_VAR(var) {(void)var;}

// ----------------------------------------------------------------------------
// class LogSystem
// ----------------------------------------------------------------------------
//      Create one of these in an application's main to initialize logging.
// ----------------------------------------------------------------------------
class
LP3_CORE_API
LogSystem
{
public:
    LogSystem();
    ~LogSystem();
};


} }


#ifdef LP3_COMPILE_LOG_DISABLE

    #define LP3_LOG_CRITICAL(...) /* */
    #define LP3_LOG_DEBUG(...) /* */
    #define LP3_LOG_ERROR(...) /* */
    #define LP3_LOG_INFO(...) /* */
    #define LP3_LOG_WARNING(...) /* */
    #define LP3_LOG_SET(name, value) /* name, value */

#else


#include <boost/format.hpp>

namespace lp3::core {

    template<typename LastType>
    void feedToFormat(boost::format & format, const LastType & arg)
    {
        format % arg;
    }

    template<typename HeadType, typename... TailTypes>
    void feedToFormat(boost::format & format, const HeadType & head,
                      const TailTypes... tail)
    {
        format % head;
        feedToFormat(format, tail...);
    }

    LP3_CORE_API
    void write_log(const char * const filename, const int lineNumber,
                  const LogLevel & level, const char * const message);


    template<typename... ParameterTypes>
    void write_log(const char * const filename, const int lineNumber,
                  const LogLevel & level, const char * const formatString,
                  const ParameterTypes... args)
    {
        auto format = boost::format(formatString);
        feedToFormat(format, args...);
        const auto message = str(format);
        write_log(filename, lineNumber, level, message.c_str());
    }

}

#define LP3_LOG(...) \
    lp3::core::write_log(__FILE__, __LINE__, __VA_ARGS__);

// ----------------------------------------------------------------------------
// LP3_LOG_CRITICAL("msg", [arg1, arg2, ...])
// ----------------------------------------------------------------------------
//      Logs a message about some error which will probably result in
//      termination. Sort of redundant since LP3_LOG_ERROR already exists.
// ----------------------------------------------------------------------------
#define LP3_LOG_CRITICAL(...) LP3_LOG(lp3::core::LogLevel::Critical , __VA_ARGS__);

// ----------------------------------------------------------------------------
// LP3_LOG_DEBUG("msg", [arg1, arg2, ...])
// ----------------------------------------------------------------------------
//      Logs an debug message. This may happen in a loop.
// ----------------------------------------------------------------------------
#define LP3_LOG_DEBUG(...) LP3_LOG(lp3::core::LogLevel::Debug, __VA_ARGS__);

// ----------------------------------------------------------------------------
// LP3_LOG_INFO("msg", [arg1, arg2, ...])
// ----------------------------------------------------------------------------
//      Logs out some information. Typically used for non-important messages
//      that happen in a loop.
// ----------------------------------------------------------------------------
#define LP3_LOG_INFO(...) LP3_LOG(lp3::core::LogLevel::Info, __VA_ARGS__);

// ----------------------------------------------------------------------------
// LP3_LOG_ERROR("msg", [arg1, arg2, ...])
// ----------------------------------------------------------------------------
//      Logs an error message.
// ----------------------------------------------------------------------------
#define LP3_LOG_ERROR(...) LP3_LOG(lp3::core::LogLevel::Error, __VA_ARGS__);

// ----------------------------------------------------------------------------
// LP3_LOG_WARNING("msg", [arg1, arg2, ...])
// ----------------------------------------------------------------------------
//      Called when something bad happens but, miraculously, no invariant is
//      violated so the program can continue to run. Basically this is never
//      used.
// ----------------------------------------------------------------------------
#define LP3_LOG_WARNING(...) LP3_LOG(lp3::core::LogLevel::Warning, __VA_ARGS__);

// Sets a variable in the squirrel console.
#define LP3_LOG_SET(name, value) \
{   \
    LP3_LOG_DEBUG("%s@%s", name, value); \
}


#endif
#endif
