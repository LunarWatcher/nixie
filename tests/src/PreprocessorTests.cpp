#include "nixie/util/Preprocessor.hpp"
#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace nixie::Preprocessor;

TEST_CASE("Test string to u32 conversion") {
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"This is an English sentence", "This is an English sentence"},
        {"This is an English sentence with unicode characters: æøåÆØÅ«»‘’‚‛“”„‟", "This is an English sentence with unicode characters: æøåÆØÅ\"\"''''\"\"\"\""},
        {"이 문장은 영향을 받지 않아야 합니다", "이 문장은 영향을 받지 않아야 합니다"}
    };

    for (const auto& [orig, expected] : testCases) {
        INFO(orig);
        REQUIRE(normaliseUnicode(orig) == expected);
    }
}
