#ifndef FILESTRUCT_H_
#define FILESTRUCT_H_

/// This header contains helper functions to maintain judge file structures

#include "settings.h"

#include <atomic>

std::string FSProblemDir(int);
std::string FSProblemTarPath(int);
std::string FSTestdataFilePath(int, int, int);
std::string FSProblemFilePath(int, int);
std::string FSCommonFilePath(int, int);

void CreateProblemDir(int);

bool CheckProblemCompressed(int);

// compress a problem dir with an interrupt flag
void CompressProblemDir(int, volatile std::atomic_bool&);

void DecompressProblemDir(int);

void UpdateFileLinks(const ProblemSettings&);

#endif
