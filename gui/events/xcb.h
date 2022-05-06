/**
 * The MIT License (MIT)
 *
 * Copyright © 2017-2020 Ruben Van Boxem
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

/*
 * events::xcb
 * Event handling implementation for XCB.
 */

#ifndef SKUI_GUI_EVENTS_XCB_H
#define SKUI_GUI_EVENTS_XCB_H

#include "gui/events.h"

#include "gui/input/button.h"
#include <core/bitflag.h>
#include <core/utility.h>

#include <xcb/xcb.h>

namespace skui::gui::events
{
  class xcb : public base
  {
  public:
    xcb(gui::window& window);
    ~xcb() override;

    void exec() override;

  private:
    xcb_connection_t* connection;
    xcb_window_t xcb_window;

    xcb_atom_t wm_delete_window;
    core::bitflag<input::button> pressed_buttons;
  };
}

#endif
