project (LibOVR)

set ( INSTALL_BIN_PATH  bin )
set ( INSTALL_LIB_PATH  lib )
set ( INSTALL_AR_PATH   lib )
set ( INSTALL_HEADERS_PATH include )

foreach(p LIB BIN HEADERS AR)
  set(var INSTALL_${p}_PATH)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

include_directories(Include)

set(SOURCE_FILES
    Src/Kernel/OVR_Alg.cpp
    Src/Kernel/OVR_Allocator.cpp
    Src/Kernel/OVR_Atomic.cpp
    Src/Kernel/OVR_File.cpp
    Src/Kernel/OVR_FileFILE.cpp
    Src/Kernel/OVR_Log.cpp
    Src/Kernel/OVR_Math.cpp
    Src/Kernel/OVR_RefCount.cpp
    Src/Kernel/OVR_Std.cpp
    Src/Kernel/OVR_String.cpp
    Src/Kernel/OVR_String_FormatUtil.cpp
    Src/Kernel/OVR_String_PathUtil.cpp
    Src/Kernel/OVR_SysFile.cpp
    Src/Kernel/OVR_System.cpp
    Src/Kernel/OVR_Timer.cpp
    Src/Kernel/OVR_UTF8Util.cpp
    Src/OVR_DeviceHandle.cpp
    Src/OVR_DeviceImpl.cpp
    Src/OVR_JSON.cpp
    Src/OVR_LatencyTestImpl.cpp
    Src/OVR_Profile.cpp
    Src/OVR_SensorFilter.cpp
    Src/OVR_SensorFusion.cpp
    Src/OVR_SensorImpl.cpp
    Src/OVR_ThreadCommandQueue.cpp
    Src/Util/Util_LatencyTest.cpp
    Src/Util/Util_Render_Stereo.cpp
)

set(HEADER_FILES
    Include/OVR.h
    Include/OVRVersion.h
    Include/OVR/Kernel/OVR_Alg.h
    Include/OVR/Kernel/OVR_Allocator.h
    Include/OVR/Kernel/OVR_Array.h
    Include/OVR/Kernel/OVR_Atomic.h
    Include/OVR/Kernel/OVR_Color.h
    Include/OVR/Kernel/OVR_ContainerAllocator.h
    Include/OVR/Kernel/OVR_File.h
    Include/OVR/Kernel/OVR_Hash.h
    Include/OVR/Kernel/OVR_KeyCodes.h
    Include/OVR/Kernel/OVR_List.h
    Include/OVR/Kernel/OVR_Log.h
    Include/OVR/Kernel/OVR_Math.h
    Include/OVR/Kernel/OVR_RefCount.h
    Include/OVR/Kernel/OVR_Std.h
    Include/OVR/Kernel/OVR_String.h
    Include/OVR/Kernel/OVR_SysFile.h
    Include/OVR/Kernel/OVR_System.h
    Include/OVR/Kernel/OVR_Timer.h
    Include/OVR/Kernel/OVR_Threads.h
    Include/OVR/Kernel/OVR_Types.h
    Include/OVR/Kernel/OVR_UTF8Util.h
    Include/OVR/OVR_Device.h
    Include/OVR/OVR_DeviceConstants.h
    Include/OVR/OVR_DeviceHandle.h
    Include/OVR/OVR_DeviceImpl.h
    Include/OVR/OVR_DeviceMessages.h
    Include/OVR/OVR_HIDDevice.h
    Include/OVR/OVR_HIDDeviceBase.h
    Include/OVR/OVR_HIDDeviceImpl.h
    Include/OVR/OVR_JSON.h
    Include/OVR/OVR_LatencyTestImpl.h
    Include/OVR/OVR_Profile.h
    Include/OVR/OVR_SensorFilter.h
    Include/OVR/OVR_SensorFusion.h
    Include/OVR/OVR_SensorImpl.h
    Include/OVR/OVR_ThreadCommandQueue.h
    Include/OVR/Util/Util_LatencyTest.h
    Include/OVR/Util/Util_Render_Stereo.h
)

if(WIN32)
    list(APPEND SOURCE_FILES
        Src/Kernel/OVR_ThreadsWinAPI.cpp
        Src/OVR_Win32_DeviceManager.cpp
        Src/OVR_Win32_DeviceStatus.cpp
        Src/OVR_Win32_HIDDevice.cpp
        Src/OVR_Win32_HMDDevice.cpp
        Src/OVR_Win32_SensorDevice.cpp
    )
    list(APPEND HEADER_FILES
        Include/OVR/OVR_Win32_DeviceManager.h
        Include/OVR/OVR_Win32_DeviceStatus.h
        Include/OVR/OVR_Win32_HIDDevice.h
        Include/OVR/OVR_Win32_HMDDevice.h
        Include/OVR/OVR_Win32_SensorDevice.h
    )
elseif(APPLE)
    list(APPEND SOURCE_FILES
        Src/Kernel/OVR_ThreadsPthread.cpp
        Src/OVR_OSX_DeviceManager.cpp
        Src/OVR_OSX_HIDDevice.cpp
        Src/OVR_OSX_HMDDevice.cpp
        Src/OVR_OSX_SensorDevice.cpp
    )
    list(APPEND HEADER_FILES
        Include/OVR/OVR_OSX_DeviceManager.h
        Include/OVR/OVR_OSX_HIDDevice.h
        Include/OVR/OVR_OSX_HMDDevice.h
    )
else()
    list(APPEND SOURCE_FILES
        Src/Kernel/OVR_ThreadsPthread.cpp
        Src/OVR_Linux_DeviceManager.cpp
        Src/OVR_Linux_HIDDevice.cpp
        Src/OVR_Linux_HMDDevice.cpp
        Src/OVR_Linux_SensorDevice.cpp
    )
    list(APPEND HEADER_FILES
        Include/OVR/OVR_Linux_DeviceManager.h
        Include/OVR/OVR_Linux_HIDDevice.h
        Include/OVR/OVR_Linux_HMDDevice.h
    )
endif()

if(WIN32)

    list(APPEND OculusVR_EXTRA_LIBS setupapi winmm)

elseif(APPLE)

    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    list(APPEND OculusVR_EXTRA_LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY})

else()
    # Used by the SDK to read the EDID of the monitors
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/EDID
        ${CMAKE_CURRENT_BINARY_DIR}/../3rdParty/EDID
    )
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/EDID)

    find_package(X11 REQUIRED)
    find_package (Threads REQUIRED)

    # Check for XRandR (modern resolution switching and gamma control)
    if (NOT X11_Xrandr_FOUND)
        message(FATAL_ERROR "The RandR library and headers were not found")
    endif()

    # Set up library and include paths
    list(APPEND OculusVR_EXTRA_LIBS
        ${X11_X11_LIB}
        ${X11_Xrandr_LIB}
        ${CMAKE_THREAD_LIBS_INIT}
        edid udev
    )
endif()


add_library(OculusVR STATIC ${SOURCE_FILES} ${HEADER_FILES})
set(OVR_LIBRARIES ${OculusVR_EXTRA_LIBS} CACHE STRING "Dependencies of OculusVR")
target_link_libraries(OculusVR ${OculusVR_EXTRA_LIBS})

install(TARGETS OculusVR
  DESTINATION ${INSTALL_LIB_PATH}) 

install(DIRECTORY Include/
  DESTINATION ${INSTALL_HEADERS_PATH})

if(NOT WIN32)
if(NOT APPLE)
install(FILES Projects/Linux/90-oculus.rules
  DESTINATION /lib/udev/rules.d)
endif()
endif()
