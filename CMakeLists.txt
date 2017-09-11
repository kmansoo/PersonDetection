cmake_minimum_required(VERSION 2.8)

project( PersonDetection )

find_package( OpenCV REQUIRED )

SET(LUNA_SW_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/dependency/Luna)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-std=c++11")

if(OPENGL_FOUND)
  include_directories( ${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} )
else()
  include_directories( ${OpenCV_INCLUDE_DIRS} )
endif()

include_directories(
	./ 
	${LUNA_SW_PLATFORM}/src/Library/ccCore/include
	${LUNA_SW_PLATFORM}/src/Library/ccNetwork/include
	${LUNA_SW_PLATFORM}/src/FossLibrary/ccJsonParser/include
	${LUNA_SW_PLATFORM}/src/FossLibrary/ccRestClient/include
)

link_directories(
	${LUNA_SW_PLATFORM}/build/lib
  ${LUNA_SW_PLATFORM}/dependency/lib
)

#########################
# FOR LEARNING OPENCV 3 #
#########################

file(GLOB SRC_FILES 
	src/*.cpp
  src/Watson/*.cpp
  src/GCP/*.cpp
)

add_executable (SmartCamera
  ${SRC_FILES}
)

TARGET_LINK_LIBRARIES(SmartCamera
  ${OpenCV_LIBS}
  ccRestClient
  ccJsonParser
  ccNetwork
  ccCore
  curl
  mbedtls
  mbedcrypto
  mbedx509
  pthread
) 
