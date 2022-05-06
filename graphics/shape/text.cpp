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

#include "graphics/shape/text.h"

#include "graphics/canvas.h"

#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>

namespace skui::graphics
{
  text::text(core::string characters,
             style::font font)
    : characters{std::move(characters)}
    , font{std::move(font)}
  {}

  text::~text() = default;

  void text::draw(canvas& canvas,
                  const scalar_position& position,
                  const std::optional<scalar_bounding_box>& clipping_box) const
  {
    canvas.draw(*this, position, clipping_box);
  }

  scalar_size text::implicit_size(const canvas& canvas) const
  {
    return canvas.measure_text(*this);
  }
}
