#ifndef GROUND_IMAGE__GROUND_IMAGE_DISPLAY_HPP_
#define GROUND_IMAGE__GROUND_IMAGE_DISPLAY_HPP_

#include <QtCore>  // NOLINT

#include "ground_image/utils/static_image_texture.hpp"

#include "rviz_common/message_filter_display.hpp"
#include "rviz_common/properties/property.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_common/properties/tf_frame_property.hpp"
#include "rviz_common/render_panel.hpp"

#include "rviz_default_plugins/visibility_control.hpp"
#include "rviz_default_plugins/displays/image/ros_image_texture.hpp"
#include "rviz_rendering/render_window.hpp"
#include <OgreManualObject.h>

namespace ground_image
{
    class RVIZ_DEFAULT_PLUGINS_PUBLIC StaticGroundImage: public rviz_common::Display
{
    Q_OBJECT

    public:
    explicit StaticGroundImage(rviz_common::DisplayContext * context);

    StaticGroundImage();

    protected:

    void onInitialize() override;

    void reset() override;
    // Overrides from Display
    void update(float dt, float ros_dt) override;

    private Q_SLOTS:
    void updateImageAndDimensions();
    private:
    void setupProperties();
    void setupScreenRectangle();

    std::unique_ptr<StaticImageTexture> texture_;
    Ogre::ManualObject * screen_rect_;
    bool image_loaded_ = false;

    Ogre::SceneNode * object_node_;
    Ogre::MaterialPtr material_;

    rviz_common::properties::StringProperty * image_path_property_;
    rviz_common::properties::TfFrameProperty * frame_property_;
    rviz_common::properties::FloatProperty * width_property_;
    rviz_common::properties::FloatProperty * height_property_;
};

}  // namespace ground_image


#endif  // GROUND_IMAGE__GROUND_IMAGE_DISPLAY_HPP_