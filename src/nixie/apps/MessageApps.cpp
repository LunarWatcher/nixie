#include "MessageApps.hpp"
#include "dispatcher.h"
#include "nixie/apps/math/Units.hpp"
#include <nixie/util/Formatters.hpp>

namespace nixie {

void MessageApps::garbageToMetric(const dpp::message_context_menu_t& ev) {
    auto content = ev.get_message().content;

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

}
