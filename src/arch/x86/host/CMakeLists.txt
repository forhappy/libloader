SET(SRC
    procmaps.c
    ptrace.c
    elf.c
    )

ADD_LIBRARY(platform_host STATIC ${SRC})
TARGET_LINK_LIBRARIES(platform_host "-lbfd")

