//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <cassert>

#include "test_macros.h"

// Test to make sure that the streams only get initialized once
// Taken from https://bugs.llvm.org/show_bug.cgi?id=43300

int main(int, char**)
{

    std::cout << "Hello!";
    std::ios_base::fmtflags stock_flags = std::cout.flags();

    std::cout << std::boolalpha << true;
    std::ios_base::fmtflags ba_flags = std::cout.flags();
    assert(stock_flags != ba_flags);

    std::ios_base::Init init_streams;
    std::ios_base::fmtflags after_init = std::cout.flags();
    assert(after_init == ba_flags);

    return 0;
}
