#pragma once

#include <string>
#include <fstream>
#include <sstream>

std::string load_file_as_string(const char* path) {
  std::ifstream stream(path, std::ios::in);

  if(!stream.is_open()) {
    printf("ERROR");
    return "ERROR";
  }

  std::stringstream sstream;
  sstream << stream.rdbuf();
  stream.close();

  return sstream.str();
}
