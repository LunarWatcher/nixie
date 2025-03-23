#pragma once

#include <string>
namespace nixie::Preprocess {

/**
 * Partial unicode normaliser function. This does not normalise all unicode characters; only characters relevant for
 * various functions in Nixie.
 *
 * A summarised list of some of the normalisations that take place:
 * * Smart quotes are converted to normal quotes
 *
 */
std::string normaliseUnicode(const std::string& src);

}
