#include "ground_image/utils/static_image_texture.hpp"

#include "rviz_default_plugins/displays/image/ros_image_texture.hpp"
#include <OgreDataStream.h>


namespace ground_image{
StaticImageTexture::StaticImageTexture()
: rviz_default_plugins::displays::ROSImageTexture()
{
}

StaticImageTexture::~StaticImageTexture() = default;

bool StaticImageTexture::loadImageFromPath(std::string & image_path) {
    image_path_ = image_path;

    bool image_loaded = false;
    std::ifstream ifs(image_path_.c_str(), std::ios::binary|std::ios::in);
    if (ifs.is_open())
    {
        Ogre::String tex_ext;
        Ogre::String::size_type index_of_extension = image_path_.find_last_of('.');
        if (index_of_extension != Ogre::String::npos)
        {
            tex_ext = image_path_.substr(index_of_extension+1);
            Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(image_path_, &ifs, false));
            Ogre::Image img;
            img.load(data_stream, tex_ext);

            getTexture()->unload();
            getTexture()->loadImage(img);
            image_loaded = true;
        }
        ifs.close();
    }
    return image_loaded;
}

bool StaticImageTexture::update() {
    return loadImageFromPath(image_path_);
}
} // namespace ground_image