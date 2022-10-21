#include "ground_image/ground_image_display.hpp"

#include <deque>
#include <string>

#include <OgreTechnique.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>

#include "rviz_common/display_context.hpp"
#include "rviz_common/msg_conversions.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_common/properties/tf_frame_property.hpp"
#include "rviz_common/logging.hpp"
#include "rviz_common/uniform_string_stream.hpp"
#include "rviz_rendering/material_manager.hpp"


namespace ground_image
{
    StaticGroundImage::StaticGroundImage(rviz_common::DisplayContext * context)
    : StaticGroundImage()
    {
        context_ = context;
        scene_manager_ = context_->getSceneManager();
        scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
    }

    StaticGroundImage::StaticGroundImage()
    {
        texture_ = std::make_unique<StaticImageTexture>();
        setupProperties();
    }

    void StaticGroundImage::onInitialize() {
        frame_property_->setFrameManager(context_->getFrameManager());
        setupScreenRectangle();
        object_node_ = scene_node_->createChildSceneNode();
        object_node_->attachObject(screen_rect_);
        updateImageAndDimensions();
    }

    void StaticGroundImage::reset()
    {
        texture_->clear();
    }

    void StaticGroundImage::update(float dt, float ros_dt)
    {
        (void) dt;
        (void) ros_dt;

        std::string frame = frame_property_->getFrameStd();

        Ogre::Vector3 position;
        Ogre::Quaternion orientation;

        if (!context_->getFrameManager()->getTransform(frame, context_->getClock()->now(), position, orientation)) {
            RVIZ_COMMON_LOG_ERROR_STREAM("Error transforming from frame '" << frame << "' to frame '"
                                                                           << context_->getFrameManager()->getFixedFrame()
                                                                           << "'");
            setStatus(rviz_common::properties::StatusProperty::Error,
                      "Transform",
                      QString("Error transforming from frame '%1' to frame '%2'").arg(
                              QString::fromStdString(frame),
                              QString::fromStdString(context_->getFrameManager()->getFixedFrame())
                              )
                      );

            object_node_->setVisible(false);

            return;
        }

        setStatus(rviz_common::properties::StatusProperty::Ok, "Transform", "Transform OK");

        scene_node_->setPosition(position);
        scene_node_->setOrientation(orientation);

        if(image_loaded_){
            object_node_->setVisible(true);
        } else {
            object_node_->setVisible(false);
        }
    }

    void StaticGroundImage::updateImageAndDimensions()
    {
        image_loaded_ = false;
        std::string image_path = image_path_property_->getStdString();

        if (image_path.empty()) {
            RVIZ_COMMON_LOG_ERROR_STREAM("No image path specified");

            setStatus(rviz_common::properties::StatusProperty::Error,
                      "Image",
                      "No image path specified");

            object_node_->setVisible(false);

            return;
        }

        const bool loaded_correctly = texture_->loadImageFromPath(image_path);

        if (!loaded_correctly) {
            RVIZ_COMMON_LOG_ERROR_STREAM("Could not load image from path: " << image_path);

            setStatus(rviz_common::properties::StatusProperty::Error,
                      "Image",
                      QString("Could not load image from given path"));

            object_node_->setVisible(false);
            return;
        }

        setStatus(rviz_common::properties::StatusProperty::Ok, "Image", "Image OK");
        image_loaded_ = true;

        // get the width and height of the image
        float width = width_property_->getFloat();
        float height = height_property_->getFloat();

        screen_rect_->beginUpdate(0);
        screen_rect_->position(0, 0, 0);
        screen_rect_->textureCoord(0, 0);

        screen_rect_->position(width, 0, 0);
        screen_rect_->textureCoord(1, 0);

        screen_rect_->position(width, -height, 0);
        screen_rect_->textureCoord(1, 1);

        screen_rect_->position(0, -height, 0);
        screen_rect_->textureCoord(0, 1);

        screen_rect_->end();

    }

    void StaticGroundImage::setupProperties()
    {
        image_path_property_ = new rviz_common::properties::StringProperty(
            "Image Path", "",
            "Path to the image to be displayed.",
            this, SLOT(updateImageAndDimensions()));

        frame_property_ = new rviz_common::properties::TfFrameProperty(
            "Frame", rviz_common::properties::TfFrameProperty::FIXED_FRAME_STRING,
            "The frame the image should be displayed in.",
            this, nullptr, true);

        width_property_ = new rviz_common::properties::FloatProperty(
            "Width", 10.0,
            "Width of the displayed image in meters.",
            this, SLOT(updateImageAndDimensions()));

        width_property_->setMin(0.0);

        height_property_ = new rviz_common::properties::FloatProperty(
            "Height", 10.0,
            "Height of the displayed image in meters.",
            this, SLOT(updateImageAndDimensions()));

        height_property_->setMin(0.0);
    }

    void StaticGroundImage::setupScreenRectangle()
    {
        RVIZ_COMMON_LOG_INFO("setting up screen rectangle");

        static int count = 0;
        rviz_common::UniformStringStream ss;
        ss << "StaticImageDisplayObject" << count++;

        screen_rect_ = context_->getSceneManager()->createManualObject(ss.str());
        screen_rect_->estimateVertexCount(4);

        material_ = rviz_rendering::MaterialManager::createMaterialWithNoLighting(ss.str());
        material_->setSceneBlending(Ogre::SBT_REPLACE);
        material_->setDepthWriteEnabled(false);
        material_->setDepthCheckEnabled(false);

        Ogre::TextureUnitState * tu =  material_->getTechniques()[0]->getPass(0)->createTextureUnitState();
        tu->setTextureName(texture_->getName());

        tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        tu->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);

        material_->getTechniques()[0]->setLightingEnabled(true);

        screen_rect_->begin(material_, Ogre::RenderOperation::OT_TRIANGLE_FAN);

        screen_rect_->position(0, 0, 0);
        screen_rect_->textureCoord(0, 0);
        screen_rect_->normal(0, 0, 1);

        screen_rect_->position(width_property_->getFloat(), 0, 0);
        screen_rect_->textureCoord(1, 0);
        screen_rect_->normal(0, 0, 1);

        screen_rect_->position(width_property_->getFloat(), -height_property_->getFloat(),0);
        screen_rect_->textureCoord(1, 1);
        screen_rect_->normal(0, 0, 1);

        screen_rect_->position(0,  -height_property_->getFloat(),0);
        screen_rect_->textureCoord(0, 1);
        screen_rect_->normal(0, 0, 1);
        screen_rect_->end();

        material_->setCullingMode(Ogre::CULL_NONE);
    }
}  // namespace displays


#include <pluginlib/class_list_macros.hpp>  // NOLINT
PLUGINLIB_EXPORT_CLASS(ground_image::StaticGroundImage, rviz_common::Display)
