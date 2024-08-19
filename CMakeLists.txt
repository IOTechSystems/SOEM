cmake_minimum_required (VERSION 3.10)
if ("${LINUX_SYS}" STREQUAL "AzureSphere")
  azsphere_configure_tools (TOOLS_REVISION "20.11")
  azsphere_configure_api (TARGET_API_SET "${AZURE_SPHERE_TARGET_API_SET}")
  set (AZURESPHERE_API "${AZURE_SPHERE_TARGET_API_SET}")
  set (AZURESPHERE_SDK_DIR /opt/azurespheresdk/Sysroots/${AZURESPHERE_API})
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")
file (STRINGS VERSION DOT_VERSION)
string (REGEX REPLACE "..$" "" API_VERSION ${DOT_VERSION})

project(SOEM
    DESCRIPTION "Simple Open EtherCAT Master"
    VERSION ${DOT_VERSION}
    LANGUAGES C)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # Default to installing in SOEM source directory
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/install)
endif()

set(SOEM_INCLUDE_INSTALL_DIR include/soem-${API_VERSION})
set(SOEM_LIB_INSTALL_DIR lib)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(BUILD_TESTS FALSE)
else()
  message(STATUS "SOEM: not building tests when built as dependency")
  set(BUILD_TESTS FALSE)
endif()

if(WIN32)
  set(OS "win32")
  include_directories(oshw/win32/wpcap/Include)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/oshw/win32/wpcap/Lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/oshw/win32/wpcap/Lib)
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_NO_WARNINGS")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  /WX")
  set(OS_LIBS wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(GENERIC OR (UNIX AND NOT APPLE))
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(OS_LIBS pthread rt)
elseif("${LINUX_SYS}" STREQUAL "AzureSphere")
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -DAZURESPHERE -D_GNU_SOURCE")
elseif(APPLE)
  # This must come *before* linux or MacOSX will identify as Unix.
  set(OS "macosx")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread pcap)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
  set(OS "rtk")
  message(STATUS "ARCH is ${ARCH}")
  message(STATUS "BSP is ${BSP}")
  include_directories(oshw/${OS}/${ARCH})
  file(GLOB OSHW_EXTRA_SOURCES oshw/${OS}/${ARCH}/*.c)
  set(OSHW_SOURCES "${OS_HW_SOURCES} ${OSHW_ARCHSOURCES}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format")
  set(OS_LIBS "-Wl,--start-group -l${BSP} -l${ARCH} -lkern -ldev -lsio -lblock -lfs -lusb -llwip -leth -li2c -lrtc -lcan -lnand -lspi -lnor -lpwm -ladc -ltrace -lc -lm -Wl,--end-group")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rtems")
  message(STATUS "Building for RTEMS")
  set(OS "rtems")
  set(SOEM_LIB_INSTALL_DIR ${LIB_DIR})
  set(BUILD_TESTS FALSE)
endif()

message(STATUS "OS is ${OS}")

file(GLOB SOEM_SOURCES soem/*.c)
file(GLOB OSAL_SOURCES osal/${OS}/*.c)
file(GLOB OSHW_SOURCES oshw/${OS}/*.c)

file(GLOB SOEM_HEADERS soem/*.h)
file(GLOB OSAL_HEADERS osal/osal.h osal/${OS}/*.h)
file(GLOB OSHW_HEADERS oshw/${OS}/*.h)

set (SOURCES ${SOEM_SOURCES} ${OSAL_SOURCES} ${OSHW_SOURCES})
set (HEADERS ${SOEM_HEADERS} ${OSAL_HEADERS} ${OSHW_HEADERS})

include_directories (soem)
include_directories (osal)
include_directories (osal/${OS})
include_directories (oshw/${OS})

add_library (soem-${API_VERSION} ${SOURCES})
target_link_libraries (soem-${API_VERSION} PUBLIC ${OS_LIBS})
set_target_properties (soem-${API_VERSION} PROPERTIES OUTPUT_NAME soem-${API_VERSION} CLEAN_DIRECT_OUTPUT 1 POSITION_INDEPENDENT_CODE ON)

target_include_directories(soem-${API_VERSION} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/soem>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(soem-${API_VERSION} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/osal>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(soem-${API_VERSION} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/osal/${OS}>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(soem-${API_VERSION}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/oshw/${OS}>
  $<INSTALL_INTERFACE:include/soem>
  )

message(STATUS "LIB_DIR: ${SOEM_LIB_INSTALL_DIR}")

install(TARGETS soem-${API_VERSION} EXPORT soemConfig DESTINATION ${SOEM_LIB_INSTALL_DIR})

install(FILES
  ${SOEM_HEADERS}
  ${OSAL_HEADERS}
  ${OSHW_HEADERS}
  DESTINATION ${SOEM_INCLUDE_INSTALL_DIR})

install(FILES LICENSE DESTINATION share/doc/libsoem-${API_VERSION})

if(BUILD_TESTS) 
  add_subdirectory(test/simple_ng)
  add_subdirectory(test/linux/slaveinfo)
  add_subdirectory(test/linux/eepromtool)
  add_subdirectory(test/linux/simple_test)
endif()
