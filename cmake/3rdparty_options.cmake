##
#
# The MIT License (MIT)
#
# Copyright © 2018-2019 Ruben Van Boxem
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

set(SKUI_PREBUILT_3RDPARTY_PATH CACHE STRING "NOTSET")

# zlib
if(SKUI_USE_PREBUILT_ZLIB)
  add_library(zlib INTERFACE)
  target_include_directories(zlib SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/zlib)
  target_link_libraries(zlib INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(ZLIB_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_ZLIB)
  find_package(ZLIB REQUIRED)
  add_library(zlib INTERFACE)
  target_link_libraries(zlib INTERFACE ZLIB::ZLIB)
endif()
# libexpat
if(SKUI_USE_PREBUILT_LIBEXPAT)
  add_library(expat INTERFACE)
  target_include_directories(expat SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/libexpat)
  target_link_libraries(expat INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(EXPAT_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_LIBEXPAT)
  find_package(EXPAT REQUIRED)
  add_library(expat INTERFACE)
  target_link_libraries(expat INTERFACE EXPAT::EXPAT)
endif()
# libpng
if(SKUI_USE_PREBUILT_LIBPNG)
  add_library(png INTERFACE)
  target_include_directories(png SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/libpng)
  target_link_libraries(png INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}png${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(PNG_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_LIBPNG)
  find_package(PNG REQUIRED)
  add_library(png INTERFACE)
  target_link_libraries(png INTERFACE PNG::PNG)
endif()
# giflib
if(SKUI_USE_PREBUILT_GIFLIB)
  add_library(gif INTERFACE)
  target_include_directories(gif SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/giflib)
  target_link_libraries(gif INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}gif${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(GIF_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_GIFLIB)
  find_package(GIF REQUIRED)
  add_library(gif INTERFACE)
  target_include_directories(gif SYSTEM INTERFACE ${GIF_INCLUDE_DIR})
  target_link_libraries(gif INTERFACE ${GIF_LIBRARIES})
endif()
if(SKUI_USE_PREBUILT_LIBJPEG_TURBO)
  add_library(jpeg-turbo INTERFACE)
  target_include_directories(jpeg-turbo SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/libjpeg-turbo)
  target_link_libraries(jpeg-turbo INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg-turbo${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(LibJpegTurbo_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_LIBJPEG_TURBO)
  find_package(LibJpegTurbo REQUIRED)
  add_library(jpeg-turbo INTERFACE)
  target_include_directories(jpeg-turbo SYSTEM INTERFACE ${LibJpegTurbo_INCLUDE_DIRS})
  target_link_libraries(jpeg-turbo INTERFACE ${LibJpegTurbo_LIBRARIES})
endif()
if(SKUI_USE_PREBUILT_SKIA)
  add_library(skia INTERFACE)
  target_include_directories(skia SYSTEM INTERFACE
      ${SKUI_PREBUILT_3RDPARTY_PATH}/skia)
  target_link_libraries(skia INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}skia${CMAKE_STATIC_LIBRARY_SUFFIX})
  target_link_libraries(skia INTERFACE png expat zlib gif jpeg-turbo)
  if(WIN32)
    target_link_libraries(skia INTERFACE usp10)
  elseif(APPLE)
    target_link_libraries(skia INTERFACE "-framework CoreText"
                                         "-framework CoreGraphics"
                                         "-framework Foundation")
  elseif(UNIX AND NOT MINGW)
    find_package(Freetype REQUIRED)
    target_link_libraries(skia INTERFACE ${FREETYPE_LIBRARIES} fontconfig)
  endif()
  set(SKIA_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_SKIA)
  message(WARNING "Using system Skia library currently not supported")
endif()
# Boost
if(SKUI_USE_PREBUILT_BOOST)
  add_library(boost INTERFACE)
  target_include_directories(boost SYSTEM INTERFACE ${SKUI_PREBUILT_3RDPARTY_PATH}/boost)
  set(Boost_FOUND TRUE)
elseif(SKUI_USE_SYSTEM_BOOST)
  find_package(Boost 1.66 REQUIRED)
  add_library(boost INTERFACE)
  target_include_directories(boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
endif()

# For those not requested or found as prebuilt or system libraries
add_subdirectory(3rdparty)
