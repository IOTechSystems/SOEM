cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
project(SOEM C)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # Default to installing in SOEM source directory
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
endif()

if (NOT SOEM_INCLUDE_INSTALL_DIR)
  set(SOEM_INCLUDE_INSTALL_DIR include/soem)
endif ()

if (NOT SOEM_LIB_INSTALL_DIR)
  set(SOEM_LIB_INSTALL_DIR lib)
endif ()

if(WIN32)
  set(OS "win32")
  include_directories(oshw/win32/wpcap/Include)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${CMAKE_SOURCE_DIR}/oshw/win32/wpcap/Lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories(${CMAKE_SOURCE_DIR}/oshw/win32/wpcap/Lib)
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_NO_WARNINGS")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  /WX")
  set(OS_LIBS wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(UNIX AND NOT APPLE)
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread rt)
elseif(APPLE)
  # This must come *before* linux or MacOSX will identify as Unix.
  set(OS "macosx")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread pcap)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
  set(OS "rtk")
  message("ARCH is ${ARCH}")
  message("BSP is ${BSP}")
  include_directories(oshw/${OS}/${ARCH})
  file(GLOB OSHW_EXTRA_SOURCES oshw/${OS}/${ARCH}/*.c)
  set(OSHW_SOURCES "${OS_HW_SOURCES} ${OSHW_ARCHSOURCES}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format")
  set(OS_LIBS "-Wl,--start-group -l${BSP} -l${ARCH} -lkern -ldev -lsio -lblock -lfs -lusb -llwip -leth -li2c -lrtc -lcan -lnand -lspi -lnor -lpwm -ladc -ltrace -lc -lm -Wl,--end-group")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rtems")
  message("Building for RTEMS")
  set(OS "rtems")
  set(SOEM_LIB_INSTALL_DIR ${LIB_DIR})
  set(BUILD_TESTS FALSE)
endif()

message("OS is ${OS}")

file (GLOB SOEM_SOURCES soem/*.c)
file (GLOB OSAL_SOURCES osal/${OS}/*.c)
file (GLOB OSHW_SOURCES oshw/${OS}/*.c)
file (GLOB ADDITIONAL_SOURCES additional/*.c)
set (SOURCES ${SOEM_SOURCES} ${OSAL_SOURCES} ${OSHW_SOURCES} ${ADDITIONAL_SOURCES})

file (GLOB SOEM_HEADERS soem/*.h)
file (GLOB OSAL_HEADERS osal/osal.h osal/${OS}/*.h)
file (GLOB OSHW_HEADERS oshw/${OS}/*.h)
file (GLOB ADDITIONAL_HEADERS additional/*.h)
set (HEADERS ${SOEM_HEADERS} ${OSAL_HEADERS} ${OSHW_HEADERS} ${ADDITIONAL_HEADERS})

include_directories (soem)
include_directories (osal)
include_directories (osal/${OS})
include_directories (oshw/${OS})
include_directories (additional)

add_library (soem ${SOURCES})
target_link_libraries (soem PUBLIC ${OS_LIBS})
set_target_properties (soem PROPERTIES OUTPUT_NAME soem CLEAN_DIRECT_OUTPUT 1 POSITION_INDEPENDENT_CODE ON)
install(TARGETS soem DESTINATION ${SOEM_LIB_INSTALL_DIR})

install(FILES
  ${SOEM_HEADERS}
  ${OSAL_HEADERS}
  ${OSHW_HEADERS}
  ${ADDITIONAL_HEADERS}
  DESTINATION ${SOEM_INCLUDE_INSTALL_DIR})

if(BUILD_TESTS) 
  add_subdirectory(test/linux/slaveinfo)
  add_subdirectory(test/linux/eepromtool)
  add_subdirectory(test/linux/simple_test)
endif()
