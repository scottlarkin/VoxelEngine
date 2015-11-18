

#include "FileHelper.hpp"
#include <fstream>
#include <sstream>
#include "glew.h"


std::string stringFromFile(std::string filepath)
{
    std::ifstream fp;
    fp.open(filepath, std::ifstream::in);
    if (fp.is_open() == false) {
        return "";
    }
    std::stringstream ss;
    ss << fp.rdbuf();
    return ss.str();
}



TextureProperties::TextureProperties(int magFilter, int minFilter, int wrap, int anisotropy)
: magFilter_(magFilter), minFilter_(minFilter), wrap_(wrap), anisotropy_(anisotropy)
{
}


