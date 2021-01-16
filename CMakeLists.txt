cmake_minimum_required(VERSION 2.8.12)

project(abuse C CXX ASM)
set(abuse_VERSION 0.9)
set(abuse_VERSION_MAJOR 0)
set(abuse_VERSION_MINOR 9)
set(abuse_VERSION_PATCH 0)
set(abuse_VERSION_TWEAK 0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin) # static libs are archive
# Detect the platform, as there's quite a bit of platform variation

if (WIN32)
    # TODO: Think about Win64
    enable_language(RC)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


if(TARGET_PS4)
	message("PS4SDK DIR: ${PS4SDK}")

	# i'm not going to fight stupid broken fucking bs
	set(SDL2DIR "${PS4SDK}/portlibs/SDL")
	set(SDL2_LIB_DIR "${SDL2DIR}/lib")
	set(SDL2_INCLUDE_DIR "${SDL2DIR}/include")
	
	set(SDL2_LIBRARIES "${SDL2_LIB_DIR}/libSDL2.a")	#libSDL2.a libSDL2main.a ?
	set(SDL2_MIXER_LIBRARIES "${SDL2_LIB_DIR}/libSDL2_mixer.a") #libSDL2_mixer.a
	
#target_link_libraries(abuse ${SDL2_LIBRARY})
#target_link_libraries(abuse ${SDL2_MIXER_LIBRARIES})

	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-all -Wno-error -Wno-absolute-value")# -Uprintf -Dprintf=klog")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-all -Wno-error -Wno-absolute-value")# -Uprintf -Dprintf=klog")

else()

# still fucking broke imagine that
#	find_package(SDL2 2.0.0 REQUIRED)
#	find_package(SDL2_mixer 2.0.0 REQUIRED)
	# i'm not going to fight stupid broken fucking bs
	set(SDL2DIR "C:/dev/libs/SDL")
	set(SDL2_LIB_DIR "${SDL2DIR}/lib/x64")
	set(SDL2_INCLUDE_DIR "${SDL2DIR}/include" "${SDL2DIR}/include/SDL2")

	set(SDL2_LIBRARIES "${SDL2_LIB_DIR}/SDL2.lib")	#libSDL2.a libSDL2main.a ?
	set(SDL2_MIXER_LIBRARIES "${SDL2_LIB_DIR}/SDL2_mixer.lib") #libSDL2_mixer.a
	
	
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-all -Wno-error -Wno-pragma-pack -Wno-absolute-value -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-all -Wno-error -Wno-pragma-pack -Wno-absolute-value -Wno-deprecated-declarations")

endif()



include(CheckIncludeFiles)

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files("sys/time.h" HAVE_SYS_TIME_H)
check_include_files("sys/ioctl.h" HAVE_SYS_IOCTL_H)
check_include_files("netinet/in.h" HAVE_NETINET_IN_H)
check_include_files(bstring.h HAVE_BSTRING_H)

if (NOT PS4)
	set(HAVE_NETWORK TRUE CACHE BOOL "Enable networking support")
endif(NOT PS4)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if(PS4)
	set(ASSETDIR "/app0/")
    add_definitions(-DASSETDIR="${ASSETDIR}")
elseif(WIN32)
    # For Windows, just use whatever CMake wants
    set(ASSETDIR ".")
elseif(APPLE)
    # Under Mac OS X, I actually want to redirect this to an app bundle,
    # which makes things weird.
    set(ASSETDIR "abuse.app/Contents/Resources/data")
else()
    # Under Linux, match the original autotools prefix
    set(ASSETDIR "share/games/abuse")
    # Only define this for ports that actually use it
    add_definitions(-DASSETDIR="${CMAKE_INSTALL_PREFIX}/${ASSETDIR}")
endif()

add_definitions(-DPACKAGE_NAME="abuse")
add_definitions(-DPACKAGE_VERSION="${abuse_VERSION}")
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Packaging

if (NOT PS4)
set(CPACK_PACKAGE_NAME "Abuse")
set(CPACK_PACKAGE_VENDOR "Crack-Dot-Com")
set(CPACK_PACKAGE_VERSION_MAJOR ${abuse_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${abuse_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${abuse_VERSION_PATCH})
#set(CPACK_PACKAGE_DESCRIPTION_FILE "README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Abuse game")
#set(CPACK_PACKAGE_ICON "doc/abuse.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_EXECUTABLES "abuse;Abuse")
if (WIN32)
    set(CPACK_GENERATOR "WIX;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "TGZ")
endif()
# WIX-specific
set(CPACK_WIX_UPGRADE_GUID "3A89DEB1-B520-4DF5-B44D-96A0EBB27DC0")
set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/doc/icon.ico")
set(CPACK_WIX_PROGRAM_MENU_FOLDER "Abuse")

include(CPack)

#else()
#add_definitions(-Uprintf -Dprintf=klog)

endif(NOT PS4)

add_subdirectory(src)
add_subdirectory(data)
