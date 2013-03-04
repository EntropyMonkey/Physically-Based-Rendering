# FIND_PACKAGE(GLUT)  # sucks on windows so we have to do our own stuff
IF (WIN32)
  FIND_PATH(GLUT_INCLUDE_DIR GL/glut.h
    ${GLUT_DIR}
    ${GLUT_DIR}/include
    ${PROJECT_SOURCE_DIR}/3rdparty/include
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C/common/inc"
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 3.2/C/common/inc"
    "C:/Program Files/NVIDIA Corporation/Cg/include/"
    "C:/Program Files/NVIDIA Corporation/SDK 9.5/inc/"
    "C:/Program Files (x86)/NVIDIA Corporation/Cg/include/"
    "C:/Program Files (x86)/NVIDIA Corporation/SDK 9.5/inc/"
    "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Include/"
    DOC "What is the path where the file glut.h can be found"
    )

  FIND_LIBRARY(GLUT32D_LIB freeglut freeglutd
    ${GLUT_DIR}
    ${GLUT_DIR}/lib
    ${PROJECT_SOURCE_DIR}/3rdparty/lib/Debug
    DOC "What is the path where the file freeglut.lib can be found"
    )

  FIND_LIBRARY(GLUT32_LIB freeglut
    ${GLUT_DIR}
    ${GLUT_DIR}/lib
    ${PROJECT_SOURCE_DIR}/3rdparty/lib/Release
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C/common/lib"
    "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 3.2/C/common/lib"
    "C:/Program Files/NVIDIA Corporation/Cg/lib"
    "C:/Program Files (x86)/NVIDIA Corporation/Cg/lib"
    "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/lib"
    DOC "What is the path where the file freeglut.lib can be found"
    )


  IF(NOT GLUT_INCLUDE_DIR)
    MESSAGE(WARNING "Could not find GLUT on your system. Try setting GLUT_DIR to the path where you installed GLUT")
    RETURN()
  ENDIF()

  IF(GLUT32_LIB)
    IF(GLUT32D_LIB)
      SET( GLUT_LIBRARIES  "debug" ${GLUT32D_LIB} "optimized" ${GLUT32_LIB} )
    ELSE()
      SET( GLUT_LIBRARIES ${GLUT32_LIB} )
    ENDIF()
  ENDIF(GLUT32_LIB)

  MARK_AS_ADVANCED(GLUT_INCLUDE_DIR)
  MARK_AS_ADVANCED(GLUT_LIBRARIES)
  MARK_AS_ADVANCED(GLUT32_LIB)
  
ELSE()
  FIND_PACKAGE(GLUT)
ENDIF()

IF(GLUT_INCLUDE_DIR)
  SET( 
    GLUT_FOUND  1  
    CACHE STRING
    "Did the system have GLUT"
    )  
ELSE()
  SET( 
    GLUT_FOUND  0  
    CACHE STRING "Did the system have GLUT"
    )
ENDIF()
