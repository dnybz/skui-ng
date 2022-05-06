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
 * Element
 * Basic GUI building block.
 */

#ifndef SKUI_GUI_ELEMENT_H
#define SKUI_GUI_ELEMENT_H

#include "gui/input.h"
#include "gui/layout/alignment.h"
#include "gui/layout/orientation.h"

#include <core/bounded_property.h>
#include <core/trackable.h>

#include <graphics/bounding_box.h>
#include <graphics/size.h>

#include <graphics/style/spacing.h>

#include <memory>
#include <optional>
#include <vector>

namespace skui::graphics
{
  class canvas;
  class drawable;
}

namespace skui::gui
{
  class element : public core::trackable
  {
  public:
    static bool show_invisible;

    ~element() override;

    virtual void draw(graphics::canvas& canvas,
                      const graphics::scalar_position& position = {0, 0}) const = 0;
    void invalidate();

    virtual graphics::scalar_size implicit_size(const graphics::canvas& canvas) const = 0;

    /// Properties
    // This element by itself
    core::bounded_property<graphics::scalar_size> size;
    graphics::style::spacing margins;
    graphics::style::spacing padding;

    // This element inside another
    alignment vertical_alignment = alignment::center;
    alignment horizontal_alignment = alignment::center;

    orientation_flags stretch = orientation::horizontal | orientation::vertical;

    /// Signals
    // Input
    input::keyboard key;
    input::pointer pointer;

  protected:
    element();

    void draw(graphics::canvas& canvas,
              const std::vector<const graphics::drawable*>& drawables,
              const std::vector<graphics::scalar_position>& positions,
              const std::optional<graphics::scalar_bounding_box>& clipping_box = {}) const;

  private:
    bool should_repaint = false;
  };

  using element_ptr = std::unique_ptr<element>;
  using element_ptrs = std::vector<element_ptr>;
}

#endif
