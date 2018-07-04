#ifndef UTILS_H_
#define UTILS_H_

// This header defines helper functions for simplicity

#include <iostream>
#include <string>

#ifdef DEBUG
#define _DEBUG1(x) std::cerr << x << std::endl;
#define _DEBUG2(x, y) std::cerr << x << ": " << y << std::endl;
#else
#define _DEBUG1(x)
#define _DEBUG2(x, y)
#endif

// Basic rule for MySQL database, table and column name
bool IsValidName(const std::string&);

#endif
