#include "nixie/apps/math/Units.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace nixie::Units;
#define Approx Catch::Matchers::WithinRel

TEST_CASE("Verify order = 1 conversions", "[UnitConversion]") {
    SECTION("Length") {
        auto singleInch = convertUnits(InputUnits::INCH, 1);
        REQUIRE(singleInch.size() == 1);
        REQUIRE(singleInch.at(0).order == 1);
        REQUIRE(singleInch.at(0).unit == MetricUnits::METER);
        REQUIRE(singleInch.at(0).type == ResultType::UNIVERSAL);
        REQUIRE_THAT(singleInch.at(0).value, Catch::Matchers::WithinRel(0.0254));
    }
    SECTION("Volume") {
        auto gallons = convertUnits(InputUnits::GALLON, 69);
        REQUIRE(gallons.size() == 3);
        for (auto& gallon : gallons) {
            REQUIRE(gallon.order == 1);
            REQUIRE(gallon.unit == MetricUnits::LITER);
        }

        REQUIRE(gallons.at(0).type == ResultType::IM_LIQUID);
        REQUIRE_THAT(gallons.at(0).value, Approx(313.7, 0.1));
        REQUIRE(gallons.at(1).type == ResultType::US_DRY);
        REQUIRE_THAT(gallons.at(1).value, Approx(303.93, 0.1));
        REQUIRE(gallons.at(2).type == ResultType::US_WET);
        REQUIRE_THAT(gallons.at(2).value, Approx(261.2, 0.1));
    }
    SECTION("Ambiguous fucking garbage") {
        auto ounces = convertUnits(InputUnits::FLUID_OUNCE, 69);
        REQUIRE(ounces.size() == 3);
        for (auto& ounces : ounces) {
            REQUIRE(ounces.order == 1);
            REQUIRE(ounces.unit == MetricUnits::LITER);
        }

        REQUIRE(ounces.at(0).type == ResultType::IM_LIQUID);
        REQUIRE_THAT(ounces.at(0).value, Approx(1.961, 0.1));
        REQUIRE(ounces.at(1).type == ResultType::US_LIQUID);
        REQUIRE_THAT(ounces.at(1).value, Approx(2.04, 0.1));
        REQUIRE(ounces.at(2).type == ResultType::US_FOOD);
        REQUIRE_THAT(ounces.at(2).value, Approx(2.07, 0.1));

        auto pseudoSolidOunces = convertUnits(InputUnits::OUNCE, 69);
        REQUIRE(pseudoSolidOunces.size() == 4);
        REQUIRE(pseudoSolidOunces.at(0).type == ResultType::IM_LIQUID);
        REQUIRE_THAT(pseudoSolidOunces.at(0).value, Approx(1.961, 0.1));
        REQUIRE(pseudoSolidOunces.at(1).type == ResultType::US_LIQUID);
        REQUIRE_THAT(pseudoSolidOunces.at(1).value, Approx(2.04, 0.1));
        REQUIRE(pseudoSolidOunces.at(2).type == ResultType::US_FOOD);
        REQUIRE_THAT(pseudoSolidOunces.at(2).value, Approx(2.07, 0.1));
        REQUIRE(pseudoSolidOunces.at(3).type == ResultType::UNIVERSAL);
        REQUIRE(pseudoSolidOunces.at(3).unit == MetricUnits::KILOGRAM);
        REQUIRE_THAT(pseudoSolidOunces.at(3).value, Approx(1.956, 0.1));
    }
} 

TEST_CASE("Verify order = 1 conversions of area units", "[UnitConversion]") {
    auto twoAcres = convertUnits(InputUnits::ACRE, 2);
    REQUIRE(twoAcres.size() == 1);
    REQUIRE(twoAcres.at(0).order == 2);
    REQUIRE(twoAcres.at(0).unit == MetricUnits::METER);
    REQUIRE(twoAcres.at(0).type == ResultType::UNIVERSAL);
    REQUIRE_THAT(twoAcres.at(0).value, Approx(8093.7, 0.1));
} 

TEST_CASE("Verify order = 2 conversions", "[UnitConversion]") {
    auto singleInch = convertUnits(InputUnits::INCH, 1, 2);
    REQUIRE(singleInch.size() == 1);
    REQUIRE(singleInch.at(0).order == 2);
    REQUIRE(singleInch.at(0).unit == MetricUnits::METER);
    REQUIRE(singleInch.at(0).type == ResultType::UNIVERSAL);
    REQUIRE_THAT(singleInch.at(0).value, Approx(6.4516e-4));
}

TEST_CASE("Verify message conversion", "[UnitConversion][MessageApps]") {
    SECTION("Compound") {
        const std::string sampleMessage = R"('6' is just a string. 5"11 is one garbage unit, and so is 6"9' and 6' and 6". 69 miles over 420 square inches explains why 692,243.53 gallons is a dumb value)";

        auto converted = parseMessage(sampleMessage);
        for (auto& entry : converted) {
            INFO(entry.sourceValue);
        }
        REQUIRE(converted.size() == 7);
        REQUIRE(converted.at(0).sourceValue == "5\"11");
        REQUIRE(converted.at(1).sourceValue == "6\"9'");
        REQUIRE(converted.at(2).sourceValue == "6'");
        REQUIRE(converted.at(3).sourceValue == "6\"");
        REQUIRE(converted.at(4).sourceValue == "69 miles");
        REQUIRE(converted.at(5).sourceValue == "420 square inches");
        REQUIRE(converted.at(6).sourceValue == "692,243.53 gallons");

        REQUIRE(converted.at(0).results.size() == 1);
        REQUIRE(converted.at(1).results.size() == 1);
        REQUIRE(converted.at(2).results.size() == 1);
        REQUIRE(converted.at(3).results.size() == 1);
        REQUIRE(converted.at(4).results.size() == 1);
        REQUIRE(converted.at(5).results.size() == 1);
        REQUIRE(converted.at(6).results.size() == 3);

        REQUIRE_THAT(converted.at(0).results.at(0).value, Approx(1.803, 0.001));
        REQUIRE_THAT(converted.at(1).results.at(0).value, Approx(2.057, 0.001));
        REQUIRE_THAT(converted.at(2).results.at(0).value, Approx(1.8288, 0.0001));
        REQUIRE_THAT(converted.at(3).results.at(0).value, Approx(0.1524, 0.0001));
        REQUIRE_THAT(converted.at(4).results.at(0).value, Approx(111044.736, 0.1));
        REQUIRE_THAT(converted.at(5).results.at(0).value, Approx(0.271, 0.001));

        REQUIRE(converted.at(6).results.at(0).type == ResultType::IM_LIQUID);
        REQUIRE_THAT(converted.at(6).results.at(0).value, Approx(3147001.6, 0.9));
        REQUIRE(converted.at(6).results.at(1).type == ResultType::US_DRY);
        REQUIRE_THAT(converted.at(6).results.at(1).value, Approx(3049252.29, 0.1));
        REQUIRE(converted.at(6).results.at(2).type == ResultType::US_WET);
        REQUIRE_THAT(converted.at(6).results.at(2).value, Approx(2620426.81, 0.1));
    }

}

TEST_CASE("Truncation", "[UnitConversion][MessageApps]") {
    SECTION("Weight") {
        auto grams = truncateValues(0.009, MetricUnits::KILOGRAM, 1);
        REQUIRE(grams.unitDisplay == "g");
        REQUIRE(grams.value == 9.0);
        auto tonnes = truncateValues(9001, MetricUnits::KILOGRAM, 1);
        REQUIRE(tonnes.unitDisplay == " tonnes");
        REQUIRE_THAT(tonnes.value, Approx(9.001));

        auto massive = truncateValues(120000000, MetricUnits::KILOGRAM, 1);
        REQUIRE(massive.unitDisplay == " tonnes");
        REQUIRE_THAT(massive.value, Approx(120000.0));
    }

    SECTION("Volume") {
        auto liters = truncateValues(120.0, MetricUnits::LITER, 1);
        auto deciliters = truncateValues(0.65, MetricUnits::LITER, 1);
        auto centiliters = truncateValues(0.033, MetricUnits::LITER, 1);

        REQUIRE(liters.unitDisplay == "L");
        REQUIRE(deciliters.unitDisplay == "dL");
        REQUIRE(centiliters.unitDisplay == "cL");

        REQUIRE(liters.value == 120);
        REQUIRE_THAT(deciliters.value, Approx(6.5));
        REQUIRE_THAT(centiliters.value, Approx(3.3));
    }

    SECTION("Length") {
        auto inch = truncateValues(0.0254, MetricUnits::METER, 1);
        REQUIRE(inch.unitDisplay == "cm");
        REQUIRE_THAT(inch.value, Approx(2.54));

        auto m = truncateValues(500, MetricUnits::METER, 1);
        REQUIRE(m.unitDisplay == "m");
        REQUIRE_THAT(m.value, Approx(500.0));

        auto km = truncateValues(1000, MetricUnits::METER, 1);
        REQUIRE(km.unitDisplay == "km");
        REQUIRE_THAT(km.value, Approx(1.0));

        auto Mm = truncateValues(1000'000, MetricUnits::METER, 1);
        REQUIRE(Mm.unitDisplay == "Mm");
        REQUIRE_THAT(Mm.value, Approx(1.0));

        auto mm = truncateValues(0.001, MetricUnits::METER, 1);
        REQUIRE(mm.unitDisplay == "mm");
        REQUIRE_THAT(mm.value, Approx(1.0));

    }

    SECTION("Area") {
        auto sqkm = truncateValues(1000000, MetricUnits::METER, 2);
        REQUIRE(sqkm.unitDisplay == "km^2");
        REQUIRE_THAT(sqkm.value, Approx(1.0));

        auto sqm = truncateValues(1000, MetricUnits::METER, 2);
        REQUIRE(sqm.unitDisplay == "m^2");
        REQUIRE_THAT(sqm.value, Approx(1000.0));
    }

}
