#include <nixie/apps/math/Units.hpp>

#include <sstream>

template<>
struct std::formatter<nixie::Units::ConvertedText, char> {

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<class FmtContext>
    typename FmtContext::iterator format(const nixie::Units::ConvertedText& in, FmtContext& ctx) const
    {
        std::stringstream out;
        out << in.sourceValue << " is ";
        if (in.results.size() != 1) {
            out << "ambiguous: ";
        }
        for (auto& conversion : in.results) {
            auto formatted = nixie::Units::truncateValues(conversion.value, conversion.unit, conversion.order);
            auto value = formatted.value;
            auto unit = formatted.unitDisplay;
            auto designation = nixie::Units::Ratios::typesToNames.at(conversion.type);
            out << std::format(std::locale("en_GB.UTF-8"), "{:.3f}", value) << unit;
            if (in.results.size() > 1) {
                out << " if " << designation;
            }
            out << ". ";
        }
 
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
