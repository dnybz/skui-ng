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
 * native_window::xcb
 * Native Window implementation for XCB.
 */

#ifndef SKUI_GUI_NATIVE_WINDOW_XCB_H
#define SKUI_GUI_NATIVE_WINDOW_XCB_H

#include "gui/native_window.h"

#include <core/utility.h>

#include <memory>
#include <unordered_map>

#include <xcb/xcb.h>

namespace skui::gui::xcb
{
  enum class atom
  {
    wm_protocols,
    wm_delete_window,
    wm_change_state,
  };
  static const std::unordered_map<atom, std::string> atom_names
  {
    {atom::wm_protocols, "WM_PROTOCOLS"},
    {atom::wm_delete_window, "WM_DELETE_WINDOW"},
    {atom::wm_change_state, "WM_CHANGE_STATE"},
  };

  class data
  {
  protected:
    data();
    virtual ~data();
    data(xcb_connection_t* connection);

    int preferred_screen_index;
    xcb_connection_t* connection;
    xcb_screen_t* preferred_screen;

    std::unordered_map<atom, xcb_atom_t> atoms;

  private:
    void initialize_atoms();
    void initialize_atom(atom atom);
  };
}

namespace skui::gui::native_window
{
  class xcb : public gui::xcb::data
            , public base
  {
  public:
    xcb();
    ~xcb() override;

    void create(const graphics::pixel_position& position,
                const graphics::pixel_size& size) override;
    void show() final;
    void hide() final;
    void close() final;

    core::string get_title() const final;
    void set_title(const core::string& title) final;
    std::pair<graphics::pixel_position, graphics::pixel_size> get_current_geometry() const override;

    xcb_connection_t* get_connection() const;
    xcb_window_t get_window() const;
    const xcb_atom_t& get_atom(gui::xcb::atom atom) const;

  protected:
    xcb(std::unique_ptr<native_visual::base>&& native_visual,
        xcb_connection_t* connection);

    xcb_window_t window;
  };
}

#endif
