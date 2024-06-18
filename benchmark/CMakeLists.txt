
include(FetchContent)

# Declare Google Benchmark
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main  # Optionally specify a version tag or commit
)

# Configure Google Benchmark
# Set this to disable tests and other unnecessary parts of the benchmark library
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing as we don't need it.")

FetchContent_MakeAvailable(benchmark)


# Define your executable that contains the benchmarks
add_executable(run_benchmarking benchmarking.cpp)

# Link your executable against Google Benchmark
target_link_libraries(run_benchmarking benchmark::benchmark)
