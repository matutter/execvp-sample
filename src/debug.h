#ifndef DEBUG_H
#define DEBUG_H

  #if !defined(DEBUG_COLOR_OFF)
    #define KRST  "\x1B[0m"
    #define KRED  "\x1B[31m"
    #define KGRN  "\x1B[32m"
    #define KYEL  "\x1B[33m"
    #define KBLU  "\x1B[34m"
    #define KMAG  "\x1B[35m"
    #define KCYN  "\x1B[36m"
    #define KWHT  "\x1B[37m"
  #else
    #define KRST
    #define KRED
    #define KGRN
    #define KYEL
    #define KBLU 
    #define KMAG
    #define KCYN
    #define KWHT
  #endif

  #if defined(DEBUG_OFF)
    #define debug(fmt, args...)
  #else
    #if defined(__GNUG__)
      #include <iostream>
      #include <cstdio>
    #else
      #include <stdio.h>
      #include <errno.h>
      #include <string.h>
    #endif

    #define debug(fmt, args...) \
      do { \
        printf(fmt KRST "\n", ##args); \
      } while(0)

  #endif

  #define debug_primary(fmt, args...) \
    debug(KBLU fmt, ##args)

  #define debug_info(fmt, args...) \
    debug(KCYN fmt, ##args)

  #define debug_danger(fmt, args...) \
    debug(KRED fmt, ##args)

  #define debug_warning(fmt, args...) \
    debug(KYEL fmt, ##args)

  #define debug_success(fmt, args...) \
    debug(KGRN fmt, ##args)

  #define debug_where(fmt, args...) \
    debug(KWHT "%s:%s:%d" fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

  #define debug_errno(fmt, args...) \
    debug(KMAG "[%d:%s] " fmt, errno, strerror(errno), ##args)

  #define debug_cxx(stream) \
    std::cout << DBG_KEY() << stream << KRST << std::endl;

  #define debug_danger_cxx(stream) \
    debug_cxx( KRED << stream )

#endif