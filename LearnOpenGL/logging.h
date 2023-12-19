#ifndef LOGGING_H
#define LOGGING_H

#ifndef LOG_FATAL

#define LOG_FATAL(fmt, ...) do {                           \
    char begin[] = "\033[91;91m[ERROR] ";                  \
    char end[] = "\033[0m\n";                              \
    printf(begin);                                         \
    printf(fmt, ##__VA_ARGS__);                            \
    printf(" (%s: %d): %s", __FILE__, __LINE__, __func__); \
    printf(end);                                           \
    exit(1);                                               \
} while(0)

#endif

#ifndef LOG_DEBUG

#define LOG_DEBUG(fmt, ...) do {                           \
    char begin[] = "\033[36;36m[DEBUG] ";                  \
    char end[] = "\033[0m\n";                              \
    printf(begin);                                         \
    printf(fmt, ##__VA_ARGS__);                            \
    printf(" (%s: %d): %s", __FILE__, __LINE__, __func__); \
    printf(end);                                           \
} while(0)

#endif

#ifndef LOG_WARN

#define LOG_WARN(fmt, ...) do {                            \
    char begin[] = "\033[93;93m[WARNING] ";                \
    char end[] = "\033[0m\n";                              \
    printf(begin);                                         \
    printf(fmt, ##__VA_ARGS__);                            \
    printf(" (%s: %d): %s", __FILE__, __LINE__, __func__); \
    printf(end);                                           \
} while(0)

#endif

#ifndef LOG_TEST

#define LOG_TEST(fmt, ...) do {                            \
    char begin[] = "\033[33;33m[TEST] ";                   \
    char end[] = "\033[0m\n";                              \
    printf(begin);                                         \
    printf(fmt, ##__VA_ARGS__);                            \
    printf(" (%s: %d): %s", __FILE__, __LINE__, __func__); \
    printf(end);                                           \
} while(0)

#endif

#endif