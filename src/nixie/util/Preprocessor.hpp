#pragma once

#include <string>
#include <unordered_map>
#include <utf8.h>

namespace nixie::Preprocessor {
#define DQ(chr) {chr, "\""}
#define SQ(chr) {chr, "'"}
inline std::unordered_map<utf8::utfchar32_t, std::string> utf8Conversions = {
    DQ(U'«'), DQ(U'»'), DQ(U'“'), DQ(U'”'), DQ(U'„'), DQ(U'‟'),
    SQ(U'‘'), SQ(U'’'), SQ(U'‚'), SQ(U'‛'), 
};

/**
 * Partial unicode normaliser function. This does not normalise all unicode characters; only characters relevant for
 * various functions in Nixie.
 *
 * A summarised list of some of the normalisations that take place:
 * * Smart quotes are converted to normal quotes
 *      Note that CJK brackets are not affected
 *
 */
std::string normaliseUnicode(const std::string& src);

}
