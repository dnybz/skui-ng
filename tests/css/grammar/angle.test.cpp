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

#include "css/ostream.h"
#include "css/test_rule.h"

#include <css/grammar/angle.h>

namespace
{
  constexpr auto radians = ".5rad";
  constexpr auto gradians = "75grad";
  constexpr auto turns = "1.5turn";
  constexpr auto degrees = "780deg";
}

int main()
{
  using skui::test::check_rule_success;

  using skui::css::grammar::angle;
  check_rule_success(angle, radians, skui::css::angle{28.6478897f});
  check_rule_success(angle, gradians, skui::css::angle{67.5f});
  check_rule_success(angle, turns, skui::css::angle{540.f});
  check_rule_success(angle, degrees, skui::css::angle{780.f});

  return skui::test::exit_code;
}
