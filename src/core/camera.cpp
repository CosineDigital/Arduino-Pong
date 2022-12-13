#include "camera.h"

Camera::Camera(const glm::vec3& position_,
               const glm::vec3& worldUp_,
               const float& yaw_,
               const float& pitch_,
               const float& lookSensitivity_) :
               mPosition(position_), mWorldUp(worldUp_), mYaw(yaw_),
               mPitch(pitch_), mLookSensitivity(lookSensitivity_)
{
    updateCameraVectors();
}

void Camera::updateCameraVectors(void) noexcept
{
    mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront.y = sin(glm::radians(mPitch));
    mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront   = glm::normalize(mFront);

    mRight   = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp      = glm::normalize(glm::cross(mRight, mFront));
}

void Camera::update(GLFWwindow* win, float ts) noexcept
{
    updatePos(win, ts);
    updateView(win, ts);
}

void Camera::updatePos(GLFWwindow* win, float ts) noexcept
{
    if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)) {
        ts *= 7.5f;
    }

#if ENABLE_Z_MOVE
    // forward
    if (glfwGetKey(win, GLFW_KEY_W)) {
        mPosition += mFront * ts;
    }
    // backward
    if (glfwGetKey(win, GLFW_KEY_S)) {
        mPosition -= mFront * ts;
    }
#endif

#if ENABLE_X_MOVE
    // right
    if (glfwGetKey(win, GLFW_KEY_RIGHT)) {
        mPosition += mRight * ts;
    }
    // left
    if (glfwGetKey(win, GLFW_KEY_LEFT)) {
        mPosition -= mRight * ts;
    }
#endif

#if ENABLE_Y_MOVE
    // up 
    if (glfwGetKey(win, GLFW_KEY_SPACE)) 
    {
        mPosition += glm::vec3(0.0f, 1.0f, 0.0f) * ts;
    }
    // down
    else if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL)) {
        mPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * ts;
    }
#endif
}

void Camera::updateView(GLFWwindow* win, float ts) noexcept
{
#if ENABLE_PITCH | ENABLE_YAW
    static double last_x = 0.0, last_y = 0.0;
    static double cur_x = 0.0, cur_y = 0.0;

    glfwGetCursorPos(win, &cur_x, &cur_y);

    double offset_x = cur_x - last_x;
    double offset_y = last_y - cur_y;

    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT))
    {
#if ENABLE_PITCH
        mPitch -= (float)offset_y * 0.1f;
#endif

#if ENABLE_YAW
        mYaw   -= (float)offset_x * 0.1f;
#endif
    }


    if (mPitch > 89.9f)
    {
        mPitch = 89.9f;
    }
    if (mPitch < -89.9f)
    {
        mPitch = -89.9f;
    }

    last_x = cur_x;
    last_y = cur_y;

    updateCameraVectors();
#endif
}

/** @Miscellaneous getters and setters */

glm::mat4 Camera::getView(void) const noexcept
{
    return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

/**
* @screen space
* @sw - the screen's width
* @sh - the screen's height
* 
* (0, sh) ------------(sw, sh)
*         |          |
*         |          |
* (0, 0)  ------------(sw, 0)
* 
*/
glm::mat4 Camera::getProjection(void) const noexcept
{
    // according to reddit: 13 x 24 blocks is reasonable
    //constexpr float w = 24.0f;
    //constexpr float h = 13.0f;
     
    constexpr float w = 2.0f;
    constexpr float h = 2.0f;
    
    return glm::ortho(-w / 2.0f + mPosition.x, // left
                       w / 2.0f + mPosition.x, // right
                      -h / 2.0f + mPosition.y, // bottom
                       h / 2.0f + mPosition.y, // top
                      -1.0f, 1.0f); // z clip plane near, far
}

glm::mat4 Camera::getInverseProjection(void) const noexcept
{
    return glm::inverse(getProjection());
}

glm::vec2 Camera::getPosition2D(void) const noexcept
{
    return { mPosition.x, mPosition.y };
}


const glm::vec3& Camera::getFrontVector(void) const noexcept
{
    return mFront;
}

const glm::vec3& Camera::getRightVector(void) const noexcept
{
    return mRight;
}