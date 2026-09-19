//
//  DebugHelper.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#include <iostream>
#include <string>

namespace DebugHelper {

    inline void PrintError(const std::string& msg) {
        std::cerr << msg << '\n';
    } // PrintError
}
