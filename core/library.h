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
 * Library
 * A dynamically loadable library.
 */

#ifndef SKUI_CORE_LIBRARY_H
#define SKUI_CORE_LIBRARY_H

#include "core/path.h"
#include "core/string.h"

#include <functional>
#include <vector>

namespace skui::core
{
  class library
  {
  public:
    library(path filename);
    ~library();

    bool load(path filename = {});
    bool unload();

    template<typename FunctionType>
    auto resolve(const string& symbol_name)
    {
      static_assert(std::is_function_v<FunctionType>);
      using function_type = std::add_pointer_t<FunctionType>;

      return function_type(resolve_symbol(symbol_name));
    }

  private:
    using function_ptr = void(*)();

    function_ptr resolve_symbol(const string& symbol_name);

    path filename;
    void* native_handle;
  };
}

#endif
