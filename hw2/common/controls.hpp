#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getPosition();
glm::vec3 getDirection();
float getHorizontalAngle();
float getVerticalAngle();
void setPositionAndDirection(glm::vec3 position_, float horizontalAngle_, float verticalAngle_);
#endif
