/**
 * The MIT License (MIT)
 *
 * Copyright © 2019-2020 Ruben Van Boxem
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

#ifndef SKUI_CSS_PROPERTY_LINEAR_GRADIENT_H
#define SKUI_CSS_PROPERTY_LINEAR_GRADIENT_H

#include "css/angle.h"
#include "css/position.h"

#include "css/property/background_gradient.h"

#include <variant>

#include <cinttypes>

namespace skui::css
{
  struct linear_gradient : background_gradient<length>
  {
    std::variant<position, angle> direction; // direction or explicit angle [0,360] degrees
  };

  constexpr bool operator==(const linear_gradient& lhs, const linear_gradient& rhs)
  {
    return static_cast<const background_gradient<length>&>(lhs) == static_cast<const background_gradient<length>&>(rhs)
        && lhs.direction == rhs.direction;
  }
}

#endif
