#include "MessageApps.hpp"
#include "dispatcher.h"
#include "nixie/apps/math/Units.hpp"
#include "nixie/util/Preprocessor.hpp"
#include <nixie/util/Formatters.hpp>
#include <regex>
#include <string>

namespace nixie {

void MessageApps::garbageToMetric(const dpp::message_context_menu_t& ev) {
    auto content = Preprocessor::normaliseUnicode(ev.get_message().content);

    auto converted = Units::parseMessage(content);
    if (converted.size() == 0) {
        ev.reply(dpp::message("No units found").set_flags(dpp::m_ephemeral));
        return;
    }

    std::stringstream ss;
    ss << "I found the following garbage units:\n";

    for (const auto& conv : converted) {
        ss << std::format("* {}\n", conv);
    }


    ev.reply(ss.str());
}

void MessageApps::metricToAmerican(const dpp::message_context_menu_t& ev) {
    static std::regex metricDistRegex{
        "(-?\\d+\.?\\d*) ?([cmkdM])?m\\b"
    };

    auto content = Preprocessor::normaliseUnicode(ev.get_message().content);
    
    struct Data {
        double quantity;
        std::string prefix;
    };

    std::vector<Data> data;

    for (std::sregex_iterator i = std::sregex_iterator(content.begin(), content.end(), metricDistRegex); i != std::sregex_iterator(); ++i) {
        auto match = *i;
        auto quantity = std::stod(match.str(1));
        auto prefix = match.str(2);
        auto prefixAsChar = prefix.size() == 0 ? "" : prefix;

        data.push_back({
            quantity,
            prefixAsChar
        });
    }

    if (data.size() == 0) {
        ev.reply(dpp::message("No distance units found").set_flags(dpp::m_ephemeral));
        return;
    }

    std::stringstream ss;
    ss << "I found the following metric distance units:\n";
    for (const auto& entry : data) {
        ss << "* ";
        ss << entry.quantity << entry.prefix << "m ";
        const auto recomputed = entry.quantity
            * (entry.prefix == "" ? 1.0 : std::pow(10.0, Units::Ratios::prefixToMagnitude.at(entry.prefix)));
        ss << "is approximately " << std::format("{:.3f}", recomputed) << " M16A4 assault rifles.\n";

    }

    ev.reply(ss.str());
}

}
