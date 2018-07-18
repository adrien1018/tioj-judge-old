#include <unistd.h>
#include <sys/stat.h>

#include <chrono>
#include <random>
#include <thread>
#include <fstream>

#include <gtest/gtest.h>
#include "communicator_main.h"

namespace {

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
