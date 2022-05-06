##
#
# The MIT License (MIT)
#
# Copyright © 2017-2020 Ruben Van Boxem
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

if(UNIX AND NOT APPLE)
  find_package(XCB COMPONENTS XCB AUX SHM IMAGE)
  find_package(X11_XCB)
  find_package(X11)
  find_package(GLX)
  find_package(EGL)
endif()

# Common source files
set(gui_src
    icon.h
    input.h
    layout.h layout.cpp
    main_windows.cpp
    make_element.h
    window.h window.cpp
               window_linux.cpp
               window_macosx.cpp
               window_windows.cpp
    window_flags.h
    window_state.h
   )
set(element_src
    element.h element.cpp
    element/graphics_view.h element/graphics_view.cpp
    element/label.h element/label.cpp
    element/spacer.h element/spacer.cpp
   )
set(events_src
    events.h events.cpp events_linux.cpp
                          events_macosx.cpp
                          events_windows.cpp
    events/appkit.h events/appkit.cpp
    events/uikit.h events/uikit.cpp
    events/win32.h events/win32.cpp
    events/xcb.h events/xcb.cpp
   )
set(input_src
    input.h
    input/key.h
    input/button.h input/button.cpp
    input/scroll.h input/scroll.cpp
    )
set(layout_src
    layout/alignment.h
    layout/column.h layout/column.cpp
    layout/orientation.h
    layout/row.h layout/row.cpp
   )
set(native_window_src
    native_window.h native_window.cpp
                      native_window_linux.cpp
                      native_window_macosx.cpp
                      native_window_windows.cpp
    native_window/appkit.h native_window/appkit.cpp
    native_window/uikit.h native_window/uikit.cpp
    native_window/win32.h native_window/win32.cpp
    native_window/xcb.h native_window/xcb.cpp
    native_window/xlib.h native_window/xlib.cpp
   )
set(native_visual_src
    native_visual.h native_visual.cpp
    native_visual/cgl.h native_visual/cgl.cpp
    native_visual/coregraphics.h native_visual/coregraphics.cpp
    native_visual/egl.h native_visual/egl.cpp
    native_visual/glx.h native_visual/glx.cpp
    native_visual/raster.h native_visual/raster.cpp
    native_visual/wgl.h native_visual/wgl.cpp
    native_visual/win32.h native_visual/win32.cpp
    native_visual/xcb.h native_visual/xcb.cpp
   )
source_group("" FILES ${gui_src})
source_group(element FILES ${element_src})
source_group(events FILES ${events_src})
source_group(input FILES ${input_src})
source_group(layout FILES ${layout_src})
source_group(native_visual FILES ${native_visual_src})
source_group(native_window FILES ${native_window_src})

# The library itself
add_library(gui
            ${gui_src}
            ${element_src}
            ${events_src}
            ${input_src}
            ${layout_src}
            ${native_window_src}
            ${native_visual_src}
           )
target_link_libraries(gui PUBLIC core graphics)
target_compile_definitions(gui PRIVATE $<$<PLATFORM_ID:Windows>:NOMINMAX>)

# Platform specific files and libraries
if(NOT WIN32)
  exclude_from_build(events/win32.h events/win32.cpp
                     events_windows.cpp
                     main_windows.cpp
                     native_window/win32.h native_window/win32.cpp
                     native_visual/wgl.h native_visual/wgl.cpp
                     native_visual/win32.h native_visual/win32.cpp
                     native_window_windows.cpp
                     window_windows.cpp)
endif()
if(NOT APPLE)
  exclude_from_build(events/appkit.h events/appkit.cpp
                     events/uikit.h events/uikit.cpp
                     events_macosx.cpp
                     native_window/appkit.h native_window/appkit.cpp
                     native_window/uikit.h native_window/uikit.cpp
                     native_visual/cgl.h native_visual/cgl.cpp
                     native_visual/coregraphics.h native_visual/coregraphics.cpp
                     native_window_macosx.cpp
                     window_macosx.cpp)
endif()
if(NOT UNIX OR APPLE)
  exclude_from_build(events/xcb.h events/xcb.cpp
                     events_linux.cpp
                     native_visual/glx.h native_visual/glx.cpp
                     native_visual/xcb.h native_visual/xcb.cpp
                     native_visual_linux.cpp
                     native_window/xcb.h native_window/xcb.cpp
                     native_window/xlib.h native_window/xlib.cpp
                     native_window_linux.cpp
                     window_linux.cpp)
endif()

if(XCB_FOUND)
  target_link_libraries(gui PUBLIC XCB::XCB XCB::AUX XCB::IMAGE)
else()
  exclude_from_build(events/xcb.h events/xcb.cpp
                     native_window/xcb.h native_window/xcb.cpp
                     native_visual/xcb.h native_visual/xcb.cpp)
endif()

if(GLX_FOUND)
  target_link_libraries(gui PUBLIC GLX)
else()
  exclude_from_build(native_visual/glx.h native_visual/glx.cpp)
endif()

if(EGL_FOUND)
  target_link_libraries(gui PUBLIC EGL::EGL)
else()
  exclude_from_build(native_visual/egl.h native_visual/egl.cpp)
endif()

if(X11_FOUND AND X11_XCB_FOUND)
  target_link_libraries(gui PUBLIC ${X11_LIBRARIES} X11::XCB)
else()
  exclude_from_build(native_window/xlib.h native_window/xlib.cpp)
endif()
