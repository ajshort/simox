# - Find Coin3D (Open Inventor)
# Coin3D is an implementation of the Open Inventor API.
# It provides data structures and algorithms for 3D visualization
# http://www.coin3d.org/
#
# This module defines the following variables
#  COIN3D_FOUND         - system has Coin3D - Open Inventor
#  COIN3D_INCLUDE_DIRS  - where the Inventor include directory can be found
#  COIN3D_LIBRARIES     - Link to this to use Coin3D
#
# The Coin directory can be defined by the environment variable <Coin3D_DIR>

#=============================================================================
# Copyright 2008-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

IF (WIN32)
  IF (CYGWIN)

    FIND_PATH(COIN3D_INCLUDE_DIRS Inventor/So.h)
    FIND_LIBRARY(COIN3D_LIBRARIES Coin)

  ELSE (CYGWIN)

    FIND_PATH(COIN3D_INCLUDE_DIRS Inventor/So.h
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/include"
      $ENV{Coin3D_DIR}/include
      $ENV{COINDIR}/include
    )

    FIND_LIBRARY(COIN3D_LIBRARY_DEBUG coin3d
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/lib"
      $ENV{Coin3D_DIR}/lib
      $ENV{COINDIR}/lib
    )

    FIND_LIBRARY(COIN3D_LIBRARY_RELEASE coin3
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/lib"
      $ENV{Coin3D_DIR}/lib
      $ENV{COINDIR}/lib
    )

    IF (COIN3D_LIBRARY_DEBUG AND COIN3D_LIBRARY_RELEASE)
      SET(COIN3D_LIBRARIES optimized ${COIN3D_LIBRARY_RELEASE}
                           debug ${COIN3D_LIBRARY_DEBUG})
    ELSE (COIN3D_LIBRARY_DEBUG AND COIN3D_LIBRARY_RELEASE)
      IF (COIN3D_LIBRARY_DEBUG)
        SET (COIN3D_LIBRARIES ${COIN3D_LIBRARY_DEBUG})
      ENDIF (COIN3D_LIBRARY_DEBUG)
      IF (COIN3D_LIBRARY_RELEASE)
        SET (COIN3D_LIBRARIES ${COIN3D_LIBRARY_RELEASE})
      ENDIF (COIN3D_LIBRARY_RELEASE)
    ENDIF (COIN3D_LIBRARY_DEBUG AND COIN3D_LIBRARY_RELEASE)

  ENDIF (CYGWIN)

ELSE (WIN32)
  IF(APPLE)
    FIND_PATH(COIN3D_INCLUDE_DIRS Inventor/So.h
     /Library/Frameworks/Inventor.framework/Headers
    )
    FIND_LIBRARY(COIN3D_LIBRARIES Coin
      /Library/Frameworks/Inventor.framework/Libraries
    )
    SET(COIN3D_LIBRARIES "-framework Coin3d" CACHE STRING "Coin3D library for OSX")
  ELSE(APPLE)

    find_path( COIN3D_INCLUDE_DIRS NAMES Inventor/So.h PATHS $ENV{Coin3D_DIR}/include /usr/include /usr/local/include NO_DEFAULT_PATH )
    find_library( COIN3D_LIBRARIES  NAMES Coin PATHS $ENV{Coin3D_DIR}/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES} /usr/local/lib NO_DEFAULT_PATH )
#FIND_PATH(COIN3D_INCLUDE_DIRS Inventor/So.h $ENV{COIN3D_DIR}/include)
#FIND_LIBRARY(COIN3D_LIBRARIES Coin $ENV{COIN3D_DIR}/lib)

  ENDIF(APPLE)

ENDIF (WIN32)

# handle the QUIETLY and REQUIRED arguments and set COIN3D_FOUND to TRUE if
# all listed variables are TRUE
#INCLUDE("${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake")
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Coin3D DEFAULT_MSG COIN3D_LIBRARIES COIN3D_INCLUDE_DIRS)

MARK_AS_ADVANCED(COIN3D_INCLUDE_DIRS COIN3D_LIBRARIES )


