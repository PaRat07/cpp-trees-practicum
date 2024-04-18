#include "../app/colors.h"


int ToIntFromHex(char hex) {
    if (hex >= 'A' && hex <= 'F') {
        return 10 + (hex - 'A');
    } else if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else {
        throw std::invalid_argument("Unexpected character " + std::string(1, hex) + " at function ToIntFromHex");
    }
}

sf::Uint32 operator""_c(const char *data, size_t size) {
    std::string s(data, size);
    s.erase(s.begin());
//    std::reverse(s.begin(), s.end());
    std::string_view sv = s;
    sf::Uint32 color = 0;
    while (!sv.empty()) {
        color <<= 4;
        color += ToIntFromHex(sv[0]);
        sv.remove_prefix(1);
    }
    color <<= 8;
    color |= 255;
    return color;
}


sf::Color primary("#D0BCFF"_c);
sf::Color surface_tint("#D0BCFF"_c);
sf::Color on_primary("#381E72"_c);
sf::Color primary_container("#4F378B"_c);
sf::Color on_primary_container("#EADDFF"_c);
sf::Color secondary("#CCC2DC"_c);
sf::Color on_secondary("#332D41"_c);
sf::Color secondary_container("#4A4458"_c);
sf::Color on_secondary_container("#E8DEF8"_c);
sf::Color tertietary("#EFB8C8"_c);
sf::Color on_tertieatary("#492532"_c);
sf::Color tartietrary_container("#633B48"_c);
sf::Color on_tertietary_container("#FFD8E4"_c);
sf::Color error("#F2B8B5"_c);
sf::Color on_error("#601410"_c);
sf::Color error_container("#8C1D18"_c);
sf::Color on_error_container("#F9DEDC"_c);
//sf::Color background("FEF7FF"_c);
//sf::Color on_background("1D1B20"_c);
sf::Color surface("#141218"_c);
sf::Color on_surface("#E6E0E9"_c);
sf::Color surface_variant("#49454F"_c);
sf::Color on_surface_variant("#CAC4D0"_c);
sf::Color outline("#938F99"_c);
sf::Color outline_variant("#49454F"_c);
//sf::Color shadow(0x000000FF);
//sf::Color scrim(0x000000FF);
sf::Color inverse_surface("#E6E0E9"_c);
sf::Color inverse_on_surface("#322F35"_c);
//sf::Color inverse_primary(0x67548EFF);
//sf::Color primary_fixed(0xEADDFFFF);
//sf::Color on_primary_fixed(0x220F46FF);
//sf::Color primary_fixed_dim(0xD2BCFDFF);
//sf::Color on_primary_fixed_variant(0x4F3D74FF);
//sf::Color secondary_fixed(0xE9DEF8FF);
//sf::Color on_secondary_fixed(0x1E182BFF);
//sf::Color secondary_fixed_dim(0xCDC2DBFF);
//sf::Color on_secondary_fixed_variant(0x4B4358FF);
//sf::Color tertietary_fixed(0xFFD9E1FF);
//sf::Color on_tertietary_fixed(0x31101BFF);
//sf::Color tertietary_fixed_dim(0xF0B7C5FF);
//sf::Color on_tertietary_fixed_variant(0x643B46FF);
//sf::Color surface_dim(0x151218FF);
//sf::Color surface_bright(0x3B383EFF);
sf::Color surface_container_lowest("#0F0D13"_c);
sf::Color surface_container_low("#1D1B20"_c);
sf::Color surface_container("#211F26"_c);
sf::Color surface_container_highest("#36343B"_c);
sf::Color surface_container_high("#2B2930"_c);
