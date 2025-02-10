#pragma once

#include "nixie/apps/AppConfig.hpp"
#include <dpp/dpp.h>

namespace nixie::SlashApps {

extern void getTimestamp(const dpp::slashcommand_t&);

inline EventMap<dpp::slashcommand_t> apps {
    {"timestamp", {
        "Converts timestamps to copypastable special timestamps",
        AppContext::ctxm_chat_input,
        SlashApps::getTimestamp
    }},
};

}
