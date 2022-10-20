#ifndef GROUND_IMAGE_STATIC_IMAGE_TEXTURE_HPP
#define GROUND_IMAGE_STATIC_IMAGE_TEXTURE_HPP
#include "rviz_default_plugins/displays/image/ros_image_texture.hpp"


namespace ground_image
{
    class StaticImageTexture : public rviz_default_plugins::displays::ROSImageTexture
    {
    public:
        StaticImageTexture();
        ~StaticImageTexture() override;

        bool loadImageFromPath(std::string & image_path);
        bool update() override;
    private:
        std::string image_path_;
    };
}  // namespace ground_image

#endif //GROUND_IMAGE_STATIC_IMAGE_TEXTURE_HPP
