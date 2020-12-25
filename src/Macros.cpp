#pragma once

#if defined(__DEBUG__)
  #define log_(str, ...) printf(str, ##__VA_ARGS__);
#else
  #define log_(str, ...) 
#endif
