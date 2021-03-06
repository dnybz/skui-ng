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
 * Bounding box.
 * Position and width/height(/depth) meant to be used as a bounding box.
 */

#ifndef SKUI_GUI_BOUNDING_BOX_H
#define SKUI_GUI_BOUNDING_BOX_H

#include "graphics/pixel.h"
#include "graphics/position.h"
#include "graphics/scalar.h"
#include "graphics/size.h"

namespace skui
{
  namespace graphics
  {
    template<typename T>
    struct bounding_box_2d
    {
      position2D<T> top_left;
      graphics::size2D<T> size;
    };

    using pixel_bounding_box = bounding_box_2d<pixel>;
    using scalar_bounding_box = bounding_box_2d<scalar>;
  }
}

#endif
