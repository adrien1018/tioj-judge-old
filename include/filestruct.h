#ifndef FILESTRUCT_H_
#define FILESTRUCT_H_

/// This header contains helper functions to maintain judge file structures
/// One must set umask to 0022 to get correct permission using these functions

#include "settings.h"

#include <atomic>
#include <filesystem>

std::filesystem::path FSProblemDir(int);
std::filesystem::path FSSubmissionDir(int, int);
std::filesystem::path FSSubmissionFilePath(int, int, int);
std::filesystem::path FSProblemTarPath(int);
std::filesystem::path FSTestdataFilePath(int, int, int);
std::filesystem::path FSProblemFilePath(int, int);
std::filesystem::path FSCommonFilePath(int, int);

void CreateProblemDir(int);

void CreateSubmissionDir(int, int);

bool CheckProblemCompressed(int);

// compress a problem dir with an interrupt flag
void CompressProblemDir(int, volatile std::atomic_bool&);

void DecompressProblemDir(int);

void UpdateFileLinks(const ProblemSettings&);

void DeleteProblemFiles(int);

void DeleteSubmissionFiles(int, int);

#endif
