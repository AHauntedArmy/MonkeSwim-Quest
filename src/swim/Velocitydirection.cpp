#include "swim/Velocitydirection.hpp"
#include "GorillaLocomotion/Player.hpp"
#include "UnityEngine/Transform.hpp"

using namespace UnityEngine;

extern Logger& getLogger();
#define INFO(value...) getLogger().info(value)

//initializing static object
Vector3 AverageVelocityDirection::lastParentPostion;

AverageVelocityDirection::AverageVelocityDirection(bool hand)
{
    isRightHand = hand;

    speed = 0.0f;
    vectorAmount = 0;
    
    Vector3 vectorZero(0.0f, 0.0f, 0.0f);

    direction = vectorZero;
    velocityDirectionAccumulator = vectorZero;
}

void AverageVelocityDirection::Update()
{
    //get pointer to the player instance, if player doens't exist or the function fails, skip this function
    GorillaLocomotion::Player* playerInstance = GorillaLocomotion::Player::get_Instance();
    if(playerInstance == nullptr) return;

    Vector3 lastPosition;
    Vector3 currentPosition;

    //if this instance is for the right hand, use right hand positions
    //otherwise use left, pc version uses reflection to store a reference to these instead.
    if(isRightHand){
        INFO("this is the right hand");
        lastPosition = playerInstance->lastRightHandPosition;
        currentPosition = playerInstance->CurrentRightHandPosition();

    } else {
        INFO("this is the left hand");
        lastPosition = playerInstance->lastLeftHandPosition;
        currentPosition = playerInstance->CurrentLeftHandPosition();
    }

    //localise the hand position to the players position so player drift doesn't add velocity
    Vector3 lastLocalised = lastPosition - lastParentPostion;
    Vector3 currentLocalised = currentPosition - playerInstance->get_transform()->get_position();

    //calculate the direction of the players arm swing
    Vector3 newDirection = lastLocalised - currentLocalised;
    
    ++vectorAmount;

    //average out the direction of the arm swing so tracking inaccuracies and awkward swings doesn't create jerky movement
    velocityDirectionAccumulator = velocityDirectionAccumulator + (vectorAmount == 1 ? newDirection : newDirection / 2.0f);
    direction = (velocityDirectionAccumulator / (float)vectorAmount).get_normalized();

    speed = newDirection.get_magnitude();

    INFO("hand speed = %f", speed);
}

void AverageVelocityDirection::Reset()
{
    speed = 0.0f;
    vectorAmount = 0;
    velocityDirectionAccumulator = Vector3::get_zero();
}