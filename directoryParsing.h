#ifndef DIRECTORY_PARSING_H
#define DIRECTORY_PARSING_H

#include <vector>
#include <string>

  int is_regular_file(const char *);
  void read_directory(const std::string& name, std::vector<std::string>&);

#endif
