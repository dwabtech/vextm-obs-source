#pragma once

#include <util/base.h>

#define log(log_level, format, ...) blog(log_level, "[vextm-source] " format, ##__VA_ARGS__)

#define debug(format, ...) log(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) log(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) log(LOG_WARNING, format, ##__VA_ARGS__)
