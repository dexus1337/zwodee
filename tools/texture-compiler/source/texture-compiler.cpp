#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "graphics/dds-format.hpp"
#include "stb_image.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::string change_extension(const std::string& path, const std::string& new_ext)
{
    size_t last_dot = path.find_last_of(".");
    size_t last_slash = path.find_last_of("\\/");
    if (last_dot != std::string::npos && (last_slash == std::string::npos || last_dot > last_slash))
    {
        return path.substr(0, last_dot) + new_ext;
    }
    return path + new_ext;
}

bool compile_texture(const std::string& input_path, const std::string& output_path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    // Load PNG with 4 channels (RGBA)
    unsigned char* pixels = stbi_load(input_path.c_str(), &width, &height, &channels, 4);
    if (!pixels)
    {
        std::cerr << "Failed to load input texture: " << input_path << "\n";
        return false;
    }

    // Build DDS header for uncompressed RGBA8 (to be loaded directly by SDL3)
    zwodee::dds_header header{};
    header.magic = 0x20534444; // "DDS "
    header.size = 124;
    header.flags = 0x1 | 0x2 | 0x4 | 0x1000; // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
    header.height = static_cast<uint32_t>(height);
    header.width = static_cast<uint32_t>(width);
    header.pitch_or_linear_size = static_cast<uint32_t>(width * 4);
    header.mip_map_count = 1;
    
    header.ddspf.size = 32;
    header.ddspf.flags = 0x40 | 0x1; // DDPF_RGB | DDPF_ALPHAPIXELS
    header.ddspf.rgb_bit_count = 32;
    header.ddspf.r_bit_mask = 0x000000FF;
    header.ddspf.g_bit_mask = 0x0000FF00;
    header.ddspf.b_bit_mask = 0x00FF0000;
    header.ddspf.a_bit_mask = 0xFF000000;
    header.caps = 0x1000; // DDSCAPS_TEXTURE

    std::ofstream out(output_path, std::ios::binary);
    if (!out)
    {
        std::cerr << "Failed to open output file: " << output_path << "\n";
        stbi_image_free(pixels);
        return false;
    }

    // Write DDS magic and header
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Write raw RGBA pixels
    out.write(reinterpret_cast<const char*>(pixels), width * height * 4);

    out.close();
    stbi_image_free(pixels);

    std::cout << "Successfully compiled: " << input_path << " -> " << output_path << " (" << width << "x" << height << " DDS)\n";
    return true;
}

void pause_if_double_clicked()
{
#ifdef _WIN32
    DWORD process_list[2];
    DWORD count = GetConsoleProcessList(process_list, 2);
    if (count <= 1)
    {
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
    }
#endif
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage:\n";
        std::cout << "  Drag and drop one or more images onto this executable\n";
        std::cout << "  Or run: zwodee-texture-compiler <input.png> [output.dds]\n\n";
        pause_if_double_clicked();
        return 0;
    }

    bool is_explicit_output = false;
    if (argc == 3)
    {
        std::string out_arg = argv[2];
        if (out_arg.size() >= 4 && (out_arg.compare(out_arg.size() - 4, 4, ".dds") == 0 || out_arg.compare(out_arg.size() - 4, 4, ".DDS") == 0))
        {
            is_explicit_output = true;
        }
    }

    if (is_explicit_output)
    {
        bool success = compile_texture(argv[1], argv[2]);
        return success ? 0 : -1;
    }
    else
    {
        int failed_count = 0;
        for (int i = 1; i < argc; ++i)
        {
            std::string input_path = argv[i];
            std::string output_path = change_extension(input_path, ".dds");
            if (!compile_texture(input_path, output_path))
            {
                failed_count++;
            }
        }
        pause_if_double_clicked();
        return failed_count == 0 ? 0 : -1;
    }
}
