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
 * gui::element::label
 * Text label element.
 */

#ifndef SKUI_GUI_ELEMENT_LABEL_H
#define SKUI_GUI_ELEMENT_LABEL_H

#include "gui/element.h"
#include "gui/make_element.h"

#include <core/property.h>
#include <core/string.h>

#include <graphics/style/font.h>
#include <graphics/shape/text.h>
#include <graphics/shape/rectangle.h>

namespace skui::gui
{
  class label : public element
  {
  public:
    label(core::string text_);
    ~label() override;

    void draw(graphics::canvas& canvas,
              const graphics::scalar_position& position) const override;

    core::property<core::string> text;

    graphics::style::fill fill;
    graphics::style::border outline;
    graphics::style::fill background;
    graphics::style::border border;
    graphics::style::font font;

    graphics::scalar_size implicit_size(const graphics::canvas& canvas) const override;
  };

  using label_ptr = std::unique_ptr<label>;
}

#endif
