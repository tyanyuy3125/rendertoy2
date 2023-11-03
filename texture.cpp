#include "texture.h"
#include "importer.h"

rendertoy::ImageTexture::ImageTexture(const std::string &path)
: _image(ImportImageFromFile(path))
{
}