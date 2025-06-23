#include "appcommand.h"
#include "config/Config.hpp"
#include "dispatcher.h"
#include "nixie/apps/MessageApps.hpp"
#include "spdlog/spdlog.h"

#include <dpp/dpp.h>

#include "apps/AppConfig.hpp"

int main() {
    nixie::Config cfg;

    dpp::cluster bot(cfg.token);

    bot.on_log([](const dpp::log_t& message) {
        switch (message.severity) {
        case dpp::ll_trace:
            spdlog::trace(message.message);
            break;
        case dpp::ll_debug:
            spdlog::debug(message.message);
            break;
        case dpp::ll_info:
            spdlog::info(message.message);
            break;
        case dpp::ll_warning:
            spdlog::warn(message.message);
            break;
        case dpp::ll_error:
            spdlog::error(message.message);

            // Meant to handle an infuriating post-startup bug where DNS resolution fails. This puts dpp in an
            // unrecoverable state, where it won't fail, but it won't recover either. It's stuck until it's manually
            // restarted.
            // This special abort forces dpp to restart cycle. Pretty sure this is the only error that's critical, but
            // an error in diguise
            if (message.message.find("discord.com:443: getaddrinfo error") != std::string::npos) {
                spdlog::critical("DNS resolution failed. DPP will not recover from this. Aborting");
                abort();
            }
            break;
        case dpp::ll_critical:
            spdlog::critical(message.message);
            abort();
            break;
        default:
            spdlog::error(message.message);
        }
    });

    bot.on_ready([&bot](const auto& event) {
        if (dpp::run_once<struct boot_t>()) {
            std::vector<dpp::slashcommand> commands;


            nixie::Apps::attachAll(nixie::MessageApps::apps, commands, bot);


            spdlog::info("Creating {} apps...", commands.size());
            bot.global_bulk_command_create(commands);
        }
    });

    bot.on_message_context_menu([](const dpp::message_context_menu_t& event) {
        nixie::MessageApps::apps.at(event.command.get_command_name()).callback(event);
    });

    bot.start(dpp::st_wait);
}
