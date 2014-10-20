#ifndef log_h
#define log_h

#include <string>

class Log{
public:

    static void clear_error_log();

    static void add_error(std::string message);

    static void add_log(std::string message);
};

#endif
