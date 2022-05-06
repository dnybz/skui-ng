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

#ifndef SKUI_CSS_GRAMMAR_LENGTH_H
#define SKUI_CSS_GRAMMAR_LENGTH_H

#include "css/length.h"
#include "css/grammar/numeric.h"

#include <boost/fusion/adapted/struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(skui::css::length,
                          value, unit)

namespace skui::css::grammar
{
  namespace x3 = boost::spirit::x3;

  struct unit_table : x3::symbols<css::unit>
  {
    unit_table();
  } const unit;

  struct unit_percentage_table : unit_table
  {
    unit_percentage_table();
  } const unit_percentage;

  const auto length = x3::rule<struct length, css::length>{"length"}
                   %= non_scientific_float >> unit
                    | x3::float_ >> unit;
  const auto length_percentage = x3::rule<struct length_percentage, css::length>{"length-percentage"}
                              %= non_scientific_float >> unit_percentage
                               | x3::float_ >> unit_percentage;
}

#endif
