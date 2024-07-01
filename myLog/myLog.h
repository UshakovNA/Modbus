
#pragma once
#ifndef MYLOG_H
#define MYLOG_H

void LOG_time();

enum class LOG_LEVEL{
    TRACE = 0,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};
#define LOG_INIT            extern void LOG_time(); static LOG_LEVEL log_level{LOG_LEVEL::TRACE};
// #define LOG_INIT(level)     extern void LOG_time(); static LOG_LEVEL log_level{level};
#define LOG_TRACE(...)      if(log_level<=LOG_LEVEL::TRACE){            \
    printf("\x1B[34m"); LOG_time();                                     \
    printf("[%s:%d:%s()]TRACE: ", __FILE__, __LINE__, __func__);        \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}
#define LOG_DEBUG(...)      if(log_level<=LOG_LEVEL::DEBUG){            \
    printf("\x1B[36m"); LOG_time();                                     \
    printf("[%s:%d:%s()]DEBUG: ", __FILE__, __LINE__, __func__);        \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}
#define LOG_INFO(...)       if(log_level<=LOG_LEVEL::INFO){             \
    printf("\x1B[32m"); LOG_time();                                     \
    printf("[%s:%d:%s()]INFO: ", __FILE__, __LINE__, __func__);         \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}
#define LOG_WARNING(...)    if(log_level<=LOG_LEVEL::WARNING){          \
    printf("\x1B[35m"); LOG_time();                                     \
    printf("[%s:%d:%s()]WARNING: ", __FILE__, __LINE__, __func__);      \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}
#define LOG_ERROR(...)      if(log_level<=LOG_LEVEL::ERROR){            \
    printf("\x1B[33m"); LOG_time();                                     \
    printf("[%s:%d:%s()]ERROR: ", __FILE__, __LINE__, __func__);        \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}
#define LOG_CRITICAL(...)   if(log_level<=LOG_LEVEL::CRITICAL){         \
    printf("\x1B[31m"); LOG_time();                                     \
    printf("[%s:%d:%s()]FATAL: ", __FILE__, __LINE__, __func__);        \
    printf(__VA_ARGS__);printf("\033[0m\n");                            \
}


#endif /*MYLOG_H*/
