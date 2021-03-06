//
// Created by user on 17-8-28.
//

#ifndef ZHIHUCPP_LOG_H
#define ZHIHUCPP_LOG_H


#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warning(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif //ZHIHUCPP_LOG_H