
#ifndef CODESIZE_HPP
#define CODESIZE_HPP

#include <string>
#include <vector>

struct FEntry
{
  std::string name;
  int size;
};

extern std::vector<FEntry> functions;

#endif
