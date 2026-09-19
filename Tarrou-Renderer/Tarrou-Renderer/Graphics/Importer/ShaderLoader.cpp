//
//  ShaderLoader.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include "ShaderLoader.hpp"
#include "DebugHelper.hpp"
// STL
#include <fstream>
#include <sstream>
#include <iostream>

std::string ShaderLoader::Load(const std::string& filePath, const std::string& baseDir) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        DebugHelper::PrintError( "[ShaderLoader] Failed to open: " + filePath);
        return "";
    }

    std::stringstream ss;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("#include") != std::string::npos && line.find("\"") != std::string::npos) {
            size_t start = line.find_first_of("\"") + 1;
            size_t end = line.find_last_of("\"");
            std::string includeFile = line.substr(start, end - start);
            
            // 찾아낸 파일을 재귀적으로 읽어서 현재 위치에 삽입
            ss << Load(baseDir + "/" + includeFile, baseDir) << "\n";
        } else {
            ss << line << "\n";
        }
    }
    return ss.str();
} // Load
