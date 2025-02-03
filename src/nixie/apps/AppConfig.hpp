#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <unordered_map>

namespace nixie {
// Simple alias to shorten slashcommand_contextmenu_type to something a bit more typeable
using AppContext = dpp::slashcommand_contextmenu_type;

template <typename Event>
struct AppConfig {
    std::string description;
    AppContext type;
    std::function<void(const Event& ev)> callback;

    std::optional<std::vector<dpp::command_option>> opts = std::nullopt;

    std::vector<dpp::interaction_context_type> interactionContexts = {
        dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel
    };

    dpp::slashcommand attach(dpp::cluster& bot, const std::string& name) {

        auto command = dpp::slashcommand()
            .set_application_id(bot.me.id)
            .set_type(type)
            .set_name(name)
            .set_description(description)
            .set_interaction_contexts(interactionContexts);

        if (opts) {
            for (const auto& opt : opts.value()) {
                command.add_option(opt);
            }
        }
        return command;
    }

};
template <typename Event>
using EventMap = std::unordered_map<std::string, AppConfig<Event>>;

namespace Apps {

template <typename Event>
void attachAll(EventMap<Event>& map, std::vector<dpp::slashcommand>& out, dpp::cluster& bot) {
    for (auto& [name, conf] : map) {
        out.push_back(conf.attach(bot, name));
    }
}

}

}
