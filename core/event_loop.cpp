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

#include "core/event_loop.h"

#include <memory>

namespace skui::core
{
  event_loop::event_loop(event_loop::filter_type filter, command_queue::commands_type commands)
    : filter{std::move(filter)}
    , queue{std::move(commands)}
  {}

  event_loop::event_loop(command_queue::commands_type commands)
    : event_loop{{}, std::move(commands)}
  {}

  int event_loop::execute()
  {
    while(!exit)
    {
      // wait on the queue to receive work
      queue.wait();

      auto commands = queue.take_commands();

      if(filter)
        filter(commands);

      while(!commands.empty() && !exit)
      {
        commands.front()->execute();
        commands.pop_front();
      }
    }
    return exit_code;
  }

  void event_loop::push(command_queue::command_ptr&& command)
  {
    queue.push(std::move(command));
  }

  void event_loop::stop(int return_code)
  {
    exit_code = return_code;
    queue.push(std::make_unique<command>([this] { exit = true; }));
  }

  void event_loop::interrupt(int return_code)
  {
    exit_code = return_code;
    queue.push_front(std::make_unique<command>([this] { exit = true; }));
  }
}
