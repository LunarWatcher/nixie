#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

namespace nixie::Units {


enum class InputUnits {
    // Length
    INCH,
    FOOT,
    MILE,
    YARD,

    // Area
    ACRE,

    // Volume
    // Ambiguous units; 3 variants per unit (imperial, US dry, US liquid)
    PINT,
    FLUID_OUNCE,
    GALLON,
    QUART,

    // Weight
    OUNCE,
    POUND,
    STONE,
    // Ambiguous 
    TON,
};

enum class ResultType {
    // Used for liquid IM units
    IM_LIQUID,
    // Used for dry US volume units
    US_DRY,
    // Used for wet US volume units
    US_WET,

    // Has to exist purely because GOD FUCKING DAMN BULLSHIT US FLUID OUNCES having slightly different wording than the
    // other volume units
    US_LIQUID,
    US_FOOD,

    // Used when the returned unit is a US unit
    US,
    // Used when the returned unit is an imperial unit
    IM,
    // Used for the rare units where the US and imperial versions are identical
    UNIVERSAL
};

enum class MetricUnits {
    LITER,
    METER,
    KILOGRAM,
};

struct ConversionResult {
    MetricUnits unit;
    double value;
    int order;
    ResultType type;
};

inline ConversionResult operator+(const ConversionResult& a, const ConversionResult& b) {
    return {
        a.unit,
        a.value + b.value,
        a.order,
        a.type
    };
}

struct ConvertedText {
    std::vector<ConversionResult> results;

    std::string sourceValue;
    std::vector<InputUnits> sourceType;

};

struct AutoType {
    double value;
    int exponent;
    MetricUnits baseUnit;
    std::string unitDisplay;
};

/**
 * \param inputType         The imperial or US unit to convert
 * \param inputValue        The value to convert
 * \param order             Meta field indicating the order of the unit. 
 *
 *      In most cases, input order == output order. square inches returns square meters, square yard return square
 *      meters, etc.
 *
 *      There are exceptions. Notably, if the input is an area unit (like ACRE), input order == 1 while
 *      output order == 2[^1]. Though there's nothing technical preventing the use of square acres, this would be a
 *      m^4, which is almost certainly not going to come up.
 *
 *
 * ## Footnotes
 * [^1]: This is due to the output unit being m^2, while acres are just of order 2. Hectars are not supported, 
 *      because fuck hectars
 */
extern std::vector<ConversionResult> convertUnits(InputUnits inputType, double inputValue, int order = 1);
extern std::vector<ConvertedText> parseMessage(const std::string& messageContent);
extern AutoType truncateValues(double rawValue, MetricUnits baseUnit, int order);

namespace Ratios {

// Meter conversions {{{
constexpr double foot = 0.3048;
constexpr double inch = 0.0254;
constexpr double mile = 1609.344;
constexpr double yard = 0.9144;
// Note: m^2
constexpr double acre = 4046.8564224;
// }}}
// Liter conversions {{{
constexpr double im_fluid_ounce = 0.0284130625;
constexpr double us_cust_fluid_ounce = 0.0295735295625;
constexpr double us_food_fluid_ounce = 0.03;

constexpr double im_pint =  0.56826125;
constexpr double us_wet_pint = 0.473176473;
constexpr double us_dry_pint = 0.5506104713575;

constexpr double im_quart =  1.1365225;
constexpr double us_wet_quart = 0.946352946;
constexpr double us_dry_quart =  1.101220942715;

constexpr double im_gallon = 4.54609;
constexpr double us_wet_gallon = 3.785411784;
constexpr double us_dry_gallon =  4.40488377086;
// }}}
// kg conversions {{{
constexpr double weight_ounce = 0.028349523125;
constexpr double im_ton = 1016.047;
constexpr double us_ton = 907.18;

constexpr double pound = 0.45359237;
constexpr double stone =  6.35029318;
// }}}

struct TypedRatio {
    ResultType unit;
    double conversionFactor;
    MetricUnits type;
};

struct UnitDeclaration {
    std::vector<TypedRatio> conversions;
    // -1 means input
    // The order of the output argument. 
    double outputOrder = -1;
    // The order of the input argument. If > 0, overrides the order used for calculations
    double inputOrder = -1;
};

#define LMETER(type, ratio) {InputUnits::type, { {{ResultType::UNIVERSAL, ratio, MetricUnits::METER }} }}

inline std::unordered_map<InputUnits, UnitDeclaration> ratios = {
    // Length {{{
    LMETER(INCH, inch),
    LMETER(FOOT, foot),
    LMETER(MILE, mile),
    LMETER(YARD, yard),
    // }}}
    // Area {{{
    {InputUnits::ACRE, { {{ResultType::UNIVERSAL, acre, MetricUnits::METER },}, 2, 1}},
    // }}}
    // Liquid units (+ ounce) {{{
    {InputUnits::FLUID_OUNCE, { 
        {
            { ResultType::IM_LIQUID, im_fluid_ounce, MetricUnits::LITER },
            { ResultType::US_LIQUID, us_cust_fluid_ounce, MetricUnits::LITER },
            { ResultType::US_FOOD, us_food_fluid_ounce, MetricUnits::LITER }
        }
    }},
    // "oz" is ambiguous. Might be weight, might be liquid. Fuck knows which
    {InputUnits::OUNCE, { 
        {
            { ResultType::IM_LIQUID, im_fluid_ounce, MetricUnits::LITER },
            { ResultType::US_LIQUID, us_cust_fluid_ounce, MetricUnits::LITER },
            { ResultType::US_FOOD, us_food_fluid_ounce, MetricUnits::LITER },
            { ResultType::UNIVERSAL, weight_ounce, MetricUnits::KILOGRAM }
        }
    }},
    {InputUnits::PINT, {
        {
            { ResultType::IM_LIQUID, im_pint, MetricUnits::LITER },
            { ResultType::US_DRY, us_dry_pint, MetricUnits::LITER },
            { ResultType::US_WET, us_wet_pint, MetricUnits::LITER },
        }
    }},
    {InputUnits::QUART, {
        {
            { ResultType::IM_LIQUID, im_quart, MetricUnits::LITER },
            { ResultType::US_DRY, us_dry_quart, MetricUnits::LITER },
            { ResultType::US_WET, us_wet_quart, MetricUnits::LITER },
        }
    }},
    {InputUnits::GALLON, {
        {
            { ResultType::IM_LIQUID, im_gallon, MetricUnits::LITER },
            { ResultType::US_DRY, us_dry_gallon, MetricUnits::LITER },
            { ResultType::US_WET, us_wet_gallon, MetricUnits::LITER },
        }
    }},
    // }}}
    // Weight units (- ounce) {{{
    {InputUnits::POUND, {
        {
            { ResultType::UNIVERSAL, pound, MetricUnits::KILOGRAM }                        
        }
    }},
    {InputUnits::STONE, {
        {
            { ResultType::UNIVERSAL, stone, MetricUnits::KILOGRAM }                        
        }
    }},
    {InputUnits::TON, {
        {
            { ResultType::US, us_ton, MetricUnits::KILOGRAM },
            { ResultType::IM, im_ton, MetricUnits::KILOGRAM },
        }
    }},
    // }}}
};

// Stripped down set of prefixes
inline std::unordered_map<int, std::string> prefixes = {
    {-9, "n"},
    {-6, "µ"},
    {-3, "m"},
    {-2, "c"}, // Only for liquids
    {-1, "d"}, // Only for liquids
    {0, "\0"},
    {3, "k"},
    {6, "M"},
    {9, "G"},
    {12, "T"},
};

inline std::unordered_map<ResultType, std::string> typesToNames = {

    {ResultType::IM_LIQUID, "Imperial, liquid"},
    {ResultType::US_DRY, "US, dry"},
    {ResultType::US_WET, "US, fluid"},

    {ResultType::US_LIQUID, "US, fluid ounces"},
    {ResultType::US_FOOD, "US, food fluid ounces"},

    {ResultType::US, "US system"},
    {ResultType::IM, "Imperial system"},
    {ResultType::UNIVERSAL, "both systems"}
};

}

}

