#FIND_PACKAGE(GLEW) does not exists
#
# Try to get GLEW (http://glew.sourceforge.net/). Our demo application
# framework relies on GLEW for initialization of any OpenGL extentions.
# End-users are not required to use GLEW.
#
# As long as if they use any GPGPU stuff from OpenTissue they provide their
# own extention initialization. If no GPGPU stuff is used then this library
# dependency can be ignored.
#
IF(WIN32)
  
  FIND_LIBRARY(GLEW32_LIB glew32
    ${GLEW_DIR}
    ${GLEW_DIR}/lib
    ${PROJECT_SOURCE_DIR}/3rdparty/lib/Release
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C/common/lib"
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 3.2/C/common/lib"
    "C:/Program Files/NVIDIA Corporation/NVIDIA OpenGL SDK 10/common/GLEW/lib"
    "C:/Program Files/NVIDIA Corporation/Cg/lib"
    "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/lib"
    DOC "What is the path where the file glew32.lib can be found"
    )
  
  FIND_LIBRARY(GLEW32D_LIB glew32 glew32d
    ${GLEW_DIR}
    ${GLEW_DIR}/lib
    ${PROJECT_SOURCE_DIR}/3rdparty/lib/Debug
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C/common/lib"
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 3.2/C/common/lib"
    "C:/Program Files/NVIDIA Corporation/NVIDIA OpenGL SDK 10/common/GLEW/lib"
    "C:/Program Files/NVIDIA Corporation/Cg/lib"
    "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/lib"
    DOC "What is the path where the file glew32.lib can be found"
    )
  
  FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
    ${GLEW_DIR}
    ${GLEW_DIR}/include
    ${GLEW_DIR}/inc
    ${PROJECT_SOURCE_DIR}/3rdparty/include
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C/common/inc/"
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 3.2/C/common/inc/"
    "C:/Program Files/NVIDIA Corporation/Cg/include/"
    "C:/Program Files/NVIDIA Corporation/SDK 9.5/inc/"
    "C:/Program Files/NVIDIA Corporation/NVIDIA OpenGL SDK 10/common/GLEW/include/"
    "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Include/"
    DOC "What is the path where the file glew.h can be found"
    )

  IF(NOT GLEW_INCLUDE_DIR)
    MESSAGE(WARNING "Could not find GLEW on your system, try setting GLEW_DIR to your installation path")
    RETURN()
  ENDIF()
  
  MARK_AS_ADVANCED(GLEW32D_LIB)
  MARK_AS_ADVANCED(GLEW32_LIB)
  MARK_AS_ADVANCED(GLEW_INCLUDE_DIR)

  SET(TMP "")
  IF(GLEW32D_LIB)
    SET(TMP ${TMP} "debug" ${GLEW32D_LIB} )
  ENDIF()
  IF(GLEW32_LIB)
    IF(GLEW32D_LIB)
       SET(TMP ${TMP} "optimized" ${GLEW32_LIB})
    ELSE()
       SET(TMP ${TMP} ${GLEW32_LIB})
    ENDIF()
  ENDIF()
  SET( 
    GLEW_LIBRARIES
    ${TMP}
    CACHE STRING
    "The glew binaries to link with"
    )
  
ELSE()
  
  FIND_LIBRARY(GLEW32_LIB GLEW
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    DOC "What is the path where the file libGLEW.a can be found"
    )
  
  FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
    /usr/include
    /usr/local/include
    /opt/local/include
    DOC "What is the path where the file GL/glew.h can be found"
    )
  
  MARK_AS_ADVANCED(GLEW32_LIB)
  MARK_AS_ADVANCED(GLEW_INCLUDE_DIR)
  
  SET( 
    GLEW_LIBRARIES
    ${GLEW32_LIB}
    CACHE STRING
    "The glew binaries to link with"
    )
  
ENDIF()

IF(GLEW_INCLUDE_DIR)
  SET( 
    GLEW_FOUND  1  
    CACHE STRING
    "Did the system have GLEW"
    )  
ELSE()
  SET( 
    GLEW_FOUND  0  
    CACHE STRING "Did the system have GLEW"
    )
ENDIF()
