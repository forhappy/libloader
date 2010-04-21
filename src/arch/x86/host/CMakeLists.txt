SET(SRC
    procmaps.c
    ptrace.c
    )

ADD_LIBRARY(platform_host STATIC ${SRC})

