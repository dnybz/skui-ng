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
 * Utility
 * Various helper constructs
 */

#ifndef SKUI_CORE_UTILITY_H
#define SKUI_CORE_UTILITY_H

#include "core/string.h"

#include <memory>

#include <cstdlib>
#include <ostream>

namespace skui::core
{
  template<typename Freeable>
  struct free_deleter { void operator()(Freeable* resource) { std::free(const_cast<void*>(reinterpret_cast<const void*>(resource))); } };
  template<typename T>
  using unique_free_ptr = std::unique_ptr<T, free_deleter<T>>;

  class ostream_format_keeper
  {
  public:
    ostream_format_keeper(std::ostream& stream)
      : stream{stream}
      , flags{stream.flags()}
    {}

    ~ostream_format_keeper()
    {
      stream.flags(flags);
    }

  private:
    std::ostream& stream;
    std::ios_base::fmtflags flags;
  };

#ifdef _WIN32
  string convert_to_utf8(const std::wstring& utf16_string);
  std::wstring convert_to_utf16(const string& utf8_string);
  std::string get_last_error_string();
#endif
}

#endif
