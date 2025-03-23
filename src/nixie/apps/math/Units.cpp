#include "Units.hpp"
#include "spdlog/spdlog.h"
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <regex>

namespace nixie {

std::vector<Units::ConversionResult> Units::convertUnits(InputUnits inputType, double inputValue, int order) {
    if (order < 1) {
        throw std::runtime_error("Misconfigured order");
    }

    std::vector<ConversionResult> results;

    // TODO: look more closely into the possibility of using mp-units. The main limitation right now seems to be the
    // lack of runtime computation due to the type model, while the system here (as set up) can theoretically convert
    // between any arbitrary order without needing to add more code
    const auto& mappings = Ratios::ratios.at(inputType);
    for (const auto& conversion : mappings.conversions) {
        int calcOrder = mappings.inputOrder > 0 ? mappings.inputOrder : order;
        int outputOrder = mappings.outputOrder > 0 ? mappings.outputOrder : calcOrder;

        double metricValue;
        if (std::holds_alternative<double>(conversion.conversionFactor)) {
            metricValue = inputValue * std::pow(std::get<double>(conversion.conversionFactor), calcOrder);
        } else if (std::holds_alternative<Ratios::NonLinConv>(conversion.conversionFactor)) {
            metricValue = std::get<Ratios::NonLinConv>(conversion.conversionFactor)(inputValue, calcOrder);
        }

        results.push_back({
            .unit = conversion.type,
            .value = metricValue,
            .order = outputOrder,
            .type = conversion.unit
        });
    }

    return results;
}

std::vector<Units::ConvertedText> Units::parseMessage(const std::string& messageContent) {
    static std::regex literals(
        // Scenario two: clusterfuck foot + inch notation that's either x'y" or x"y' (where x is always feet and y
        // is always inches). People are inconsistent, so both need to be accounted for
        // However, due to conflicts with main scenario entries, these have to be done first
        R"_(((\d+(?:\.\d+)?)"(?:(\d+(?:\.\d+)?)'?)))_" // Groups 1-3
        R"_(|((\d+(?:\.\d+)?)'(?:(\d+(?:\.\d+)?)"?)))_" // Groups 4-6
        // Main scenario: number + unit
        // " and ' are caught as fallbacks
        // Groups 7-11
        R"_(|(((?:\d+[, ]?)+(?:\.\d+)?) ?(square)? ?("|'|(?:mi\b|pt\b|qt|gal\b|yd|ac|st|ft|lbs?)\.?|in(?:ch(?:es)?|\.)?\b|(?:fl.?)?oz\.?|foot|feet|yards?|acres?|pints?|quarts|stone|gallons?|ounc[es]*|miles?|tons?|pounds?|f(?:ahrenheit)?\b)(\^?2)?))_"
        ,
        std::regex_constants::optimize | std::regex_constants::icase
    );

    std::vector<Units::ConvertedText> out;

    for (std::sregex_iterator i = std::sregex_iterator(messageContent.begin(), messageContent.end(), literals); i != std::sregex_iterator(); ++i) {
        auto match = *i;
        auto quantity = match.str(8);
        auto square = match.str(9);
        auto unit = match.str(10);
        auto altSquare = match.str(11);

        auto feet = match.str(2);
        if (feet == "") { feet = match.str(5); }
        auto inches = match.str(3);
        if (inches == "") { inches = match.str(6); }


        int order = 1;
        if (square == "square" || altSquare != "") {
            order = 2;
        }

        if (quantity == "" && feet != "") {
            // Must be a feet"inches[']/feet'inches["] notation
            double nFeet = std::stod(feet);
            double nInches = inches == "" ? 0 : std::stod(inches); 

            auto result = convertUnits(InputUnits::FOOT, nFeet).at(0) + convertUnits(InputUnits::INCH, nInches).at(0);
            out.push_back(ConvertedText {
                { result },
                match.str(1) == "" ? match.str(4) : match.str(1),
                {InputUnits::FOOT, InputUnits::INCH}
            });

        } else if (quantity != ""){
            quantity.erase(std::remove(quantity.begin(), quantity.end(), ','), quantity.end());
            quantity.erase(std::remove(quantity.begin(), quantity.end(), ' '), quantity.end());
            unit.erase(std::remove(unit.begin(), unit.end(), '.'), unit.end());
            std::transform(unit.cbegin(), unit.cend(), unit.begin(), [](const auto& chr) {
                return std::tolower(chr);
            });

            double numericValue = std::stod(quantity);
            InputUnits iu;
            // Inches without feet
            if (unit == "\"" || unit.starts_with("in") ) {
                auto start = match.position();
                if (unit == "\"" && start != 0 && messageContent.at(start - 1) == '"') {
                    continue;
                }
                // Filter out '1234'
                iu = InputUnits::INCH;
            } else if (unit == "foot" || unit == "feet" || unit == "ft" || unit == "'") {
                auto start = match.position();
                if (unit == "'" && start != 0 && messageContent.at(start - 1) == '\'') {
                    continue;
                }
                iu = InputUnits::FOOT;
            } else if (unit.starts_with("mi")) {
                iu = InputUnits::MILE;
            } else if (unit.starts_with("yard") || unit.starts_with("yd")) {
                iu = InputUnits::YARD;
            } else if (unit.starts_with("acre") || unit.starts_with("ac")) {
                iu = InputUnits::ACRE;
            } else if (unit.starts_with("pint") || unit.starts_with("pt")) {
                iu = InputUnits::PINT;
            } else if (unit.starts_with("fl")) {
                iu = InputUnits::FLUID_OUNCE;
            } else if (unit.starts_with("ounce") || unit.starts_with("oz")) {
                iu = InputUnits::OUNCE;
            } else if (unit.starts_with("gal")) {
                iu = InputUnits::GALLON;
            } else if (unit.starts_with("quart") || unit.starts_with("qt")) {
                iu = InputUnits::QUART;
            } else if (unit.starts_with("lb") || unit.starts_with("pound")) {
                iu = InputUnits::POUND;
            } else if (unit.starts_with("st")) {
                iu = InputUnits::STONE;
            } else if (unit.starts_with("ton")) {
                iu = InputUnits::TON;
            } else if (unit.starts_with("f")) { // NOTE: Must be after all other F units, because this is
                                                   // functionally a catchall
                iu = InputUnits::FAHRENHEIT;
            } else {
                spdlog::error("Unknown unit: {}", unit);
                throw std::runtime_error("A regex pattern is unhandled");
            }

            auto res = convertUnits(iu, numericValue, order);
            out.push_back({
                res,
                match.str(7),
                { iu }
            });
        } else {
            spdlog::error("Unknown configuration in string: {}", messageContent);
            throw std::runtime_error("Bad parsing");
        }
    }

    return out;
}

Units::AutoType Units::truncateValues(double rawValue, Units::MetricUnits baseUnit, int order) {
    std::string unitDisplay;
    int exponent = 0;
    auto sign = std::signbit(rawValue);
    rawValue = std::abs(rawValue);
    switch (baseUnit) {
    case MetricUnits::METER: {
        auto conversionOperator = std::pow(1000.0, (double) order);
        if (rawValue < 0.01 && order == 1) {
            while(rawValue < 1) {
                if (Ratios::prefixes.contains(exponent - 3)) {
                    exponent -= 3;
                    rawValue *= conversionOperator;
                } else {
                    break;
                }
            }
        } else if (rawValue < 1 && order == 1) {
            exponent -= 2;
            rawValue *= 100.0;
        } else {
            while (rawValue >= conversionOperator) {
                if (Ratios::prefixes.contains(exponent - 3)) {
                    exponent += 3;
                    rawValue /= conversionOperator;
                } else {
                    break;
                }
            }
        }
        unitDisplay += Ratios::prefixes[exponent];
        unitDisplay += "m";
    } break;
    case nixie::Units::MetricUnits::KILOGRAM:
        if (rawValue < 1) {
            while(rawValue < 1) {
                if (Ratios::prefixes.contains(exponent)) { // NOTE: exponent + 3 - 3 cancels out to exponent
                    exponent -= 3;
                    rawValue *= 1000.0;
                } else {
                    break;
                }
            }

            // Since the base unit of gram infuriatingly is the kilogram, +3 to the exponent
            if (exponent + 3 != 0) {
                unitDisplay += Ratios::prefixes[exponent + 3];
            }
            unitDisplay += "g";
        } else if (rawValue >= 1000) {
            exponent += 3;
            rawValue /= 1000.0;
            unitDisplay = std::string(" tonne") + (rawValue != 1.0 ? "s" : "");
        } else {
            unitDisplay = "kg";
        }

        break;
    case nixie::Units::MetricUnits::LITER:
        if (rawValue < 1) {
            rawValue *= 10.0;
            exponent = -1;

            if (rawValue < 0.1) {
                rawValue *= 100.0;
                exponent -= 2;
            }
            while (rawValue <= 0.001 && Ratios::prefixes.contains(exponent - 3)) {
                rawValue *= 1000;
                exponent += 3;
            }
            unitDisplay += Ratios::prefixes[exponent];
        }
        unitDisplay += "L";
        break;
    case nixie::Units::MetricUnits::CELSIUS:
        unitDisplay += " °C";
        break;
    }

    if (order > 1) {
        unitDisplay += "^" + std::to_string(order);
    }

    return {
        .value = rawValue * (sign ? -1 : 1),
        .exponent = exponent,
        .baseUnit = baseUnit,
        .unitDisplay = unitDisplay,
    };
}

double Units::Ratios::fToC(double f, int) {
    return 5.0 / 9.0 * (f - 32.0);
}

}
