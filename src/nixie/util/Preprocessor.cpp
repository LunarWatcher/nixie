#include "Preprocessor.hpp"
#include <string>
#include <utf8.h>


namespace nixie {

std::string Preprocessor::normaliseUnicode(const std::string &src) {
    std::string out;
    auto inserter = std::back_inserter(out);
    for (auto it = src.cbegin(); it != src.cend();) {
        try {
            auto cp = utf8::next(it, src.cend());
            if (utf8Conversions.contains(cp)) {
                out += utf8Conversions.at(cp);
            } else {
                utf8::append(cp, inserter);
            }
        } catch (utf8::not_enough_room& e) {
            break;
        }
    }

    return out;
}

}
