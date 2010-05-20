# - Check if the function exists.
# CHECK_THREADDB_FUNCTION_EXISTS(THREADDB_FUNCTION VARIABLE)
# - macro which checks if the function exists
#  THREADDB_FUNCTION - the name of the function
#  VARIABLE - variable to store the result
#
# The following variables may be set before calling this macro to
# modify the way the check is run:
#
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link

#=============================================================================
# Copyright 2002-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

MACRO(CHECK_THREADDB_FUNCTION_EXISTS THREADDB_FUNCTION VARIABLE)
  IF("${VARIABLE}" MATCHES "^${VARIABLE}$")
    SET(MACRO_CHECK_THREADDB_FUNCTION_DEFINITIONS 
      "-DCHECK_THREADDB_FUNCTION_EXISTS=${THREADDB_FUNCTION} ${CMAKE_REQUIRED_FLAGS}")
    MESSAGE(STATUS "Looking for ${THREADDB_FUNCTION}")

    SET(THREADDB_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES} -lthread_db")

    IF(THREADDB_REQUIRED_LIBRARIES)
      SET(CHECK_THREADDB_FUNCTION_EXISTS_ADD_LIBRARIES
        "-DLINK_LIBRARIES:STRING=${THREADDB_REQUIRED_LIBRARIES}")
    ELSE(THREADDB_REQUIRED_LIBRARIES)
      SET(CHECK_THREADDB_FUNCTION_EXISTS_ADD_LIBRARIES)
    ENDIF(THREADDB_REQUIRED_LIBRARIES)
    IF(CMAKE_REQUIRED_INCLUDES)
      SET(CHECK_THREADDB_FUNCTION_EXISTS_ADD_INCLUDES
        "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
    ELSE(CMAKE_REQUIRED_INCLUDES)
      SET(CHECK_THREADDB_FUNCTION_EXISTS_ADD_INCLUDES)
    ENDIF(CMAKE_REQUIRED_INCLUDES)
    TRY_COMPILE(${VARIABLE}
      ${CMAKE_BINARY_DIR}
      ${CMAKE_MODULE_PATH}/CheckThreaddbFunctionExists.c
      COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_THREADDB_FUNCTION_DEFINITIONS}
      "${CHECK_THREADDB_FUNCTION_EXISTS_ADD_LIBRARIES}"
      "${CHECK_THREADDB_FUNCTION_EXISTS_ADD_INCLUDES}"
      OUTPUT_VARIABLE OUTPUT)
    IF(${VARIABLE})
      SET(${VARIABLE} 1 CACHE INTERNAL "Have function ${THREADDB_FUNCTION}")
      MESSAGE(STATUS "Looking for ${THREADDB_FUNCTION} - found")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log 
        "Determining if the function ${THREADDB_FUNCTION} exists in thread_db passed with the following output:\n"
        "${OUTPUT}\n\n")
    ELSE(${VARIABLE})
      MESSAGE(STATUS "Looking for ${THREADDB_FUNCTION} - not found")
      SET(${VARIABLE} "" CACHE INTERNAL "Have function ${THREADDB_FUNCTION}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log 
        "Determining if the function ${THREADDB_FUNCTION} exists in thread_db failed with the following output:\n"
        "${OUTPUT}\n\n")
    ENDIF(${VARIABLE})
  ENDIF("${VARIABLE}" MATCHES "^${VARIABLE}$")
ENDMACRO(CHECK_THREADDB_FUNCTION_EXISTS)

