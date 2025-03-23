add_executable(tests
    src/UnitConversionTests.cpp
    src/PreprocessorTests.cpp
)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.0
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests 
    Catch2::Catch2WithMain
    nixiesrc
)

# vim:ft=cmake
