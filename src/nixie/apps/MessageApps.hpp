#pragma once

#include "dispatcher.h"
#include "nixie/apps/AppConfig.hpp"
#include <dpp/dpp.h>

namespace nixie::MessageApps {

extern void garbageToMetric(const dpp::message_context_menu_t& ev);
extern void metricToAmerican(const dpp::message_context_menu_t& ev);

inline EventMap<dpp::message_context_menu_t> apps {
    {"Convert to metric", {
        "Attempts to convert garbage units to metric",
        AppContext::ctxm_message,
        MessageApps::garbageToMetric,
    }},
    {"Convert distance to american", {
        "Attempts to convert metric units to american",
        AppContext::ctxm_message,
        MessageApps::metricToAmerican,
    }},
};

}
