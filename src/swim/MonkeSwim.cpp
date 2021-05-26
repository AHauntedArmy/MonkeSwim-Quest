#include "swim/MonkeSwim.hpp"
#include "trigger/SwimTrigger.hpp"

#include "UnityEngine/Rigidbody.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"

using namespace UnityEngine;
using namespace UnityEngine::XR;
using namespace GorillaLocomotion;

extern Logger& getLogger();
#define INFO(value...) getLogger().info(value)
#define ERROR(value...) getLogger().error(value)

//intializing static objects
bool MonkeSwim::canFly = false;
bool MonkeSwim::useDefault = false;
bool MonkeSwim::useGlobal = false;

float MonkeSwim::dragValue = 0.0f;
float MonkeSwim::swimMultiplier = 0.0f;
float MonkeSwim::maxSwimSpeed = 0.0f;

Vector3 MonkeSwim::velocity;

AverageVelocityDirection MonkeSwim::rightHand(true);
AverageVelocityDirection MonkeSwim::leftHand(false);
//end of static objects

void MonkeSwim::StartMod()
{
    GameObject* monkeSwimConfig = GameObject::Find(il2cpp_utils::newcsstr("AirSwimConfig"));
    if(monkeSwimConfig == nullptr) {
        INFO("monkeswimconfig is nullptr");
        return;
    }

    //intializing codegen objects
    velocity = Vector3::get_zero();

    Transform* waterSwim = monkeSwimConfig->get_transform()->Find(il2cpp_utils::newcsstr("WaterSwimTriggers"));
    Transform* useDefaults = monkeSwimConfig->get_transform()->Find(il2cpp_utils::newcsstr("AirSwimConfigDefault"));
    Transform* globalSettings = monkeSwimConfig->get_transform()->Find(il2cpp_utils::newcsstr("AirSwimConfigGlobal"));
    
    useDefault = false;
    useGlobal  = false;

    if(globalSettings != nullptr){ 
        INFO("global settings object found, using custom global settings");
        SetStats(globalSettings->get_localPosition());
        useGlobal = true;   
       
    } else if(useDefaults != nullptr){
        INFO("default object found, setting global default settings");

        Vector3 newSettings;

        newSettings.x = 6.5f;
        newSettings.y = 1.1f;
        newSettings.z = 0.0f;
        
        SetStats(newSettings);
        useDefault = true;

    } else {
        INFO("no settings flags found, using settings in airswimconfig");
        SetStats(monkeSwimConfig->get_transform()->get_localPosition());
    }

    if(waterSwim != nullptr){
        INFO("unique zones found");
        GameObject* waterObject = waterSwim->get_gameObject();

        if(waterObject != nullptr){
            INFO("getting list of every child object with a collider");
            Array<Collider*>* triggers = waterObject->GetComponentsInChildren<Collider*>(true);

            for(int i = 0; i < triggers->Length(); i++){
                if(!triggers->get(i)->get_isTrigger()) continue;

                INFO("found a child with a trigger collider, attching SwimTrigger");
                triggers->get(i)->get_gameObject()->AddComponent<Swim::SwimTrigger*>();
            }
        }

        EnableMod(false);

    }   else EnableMod(true);

    //set the last parent position to this current position
    Player* playerInstance = Player::get_Instance();
    if (playerInstance == nullptr) return;

    Transform* playerTransform = playerInstance->get_transform();
    if(playerTransform == nullptr) return;

    AverageVelocityDirection::lastParentPosition = playerTransform->get_position();
}

void MonkeSwim::EnableMod(bool toEnable)
{
    //get a local pointer to the players rigidbody
    Player* playerInstance = Player::get_Instance();
    if(playerInstance == nullptr) return;

    Rigidbody* playerPhysics = playerInstance->playerRigidBody;
    if(playerPhysics == nullptr) return;

    if(toEnable){
        INFO("enable mod = true");
        playerPhysics->set_drag(dragValue);
        playerPhysics->set_useGravity(false);

         AverageVelocityDirection::lastParentPosition = playerInstance->get_transform()->get_position();

    } else {
        INFO("enable mod = false");
        playerPhysics->set_drag(0.0f);
        playerPhysics->set_useGravity(true);

        velocity = Vector3::get_zero();
    }

    canFly = toEnable;
}

void MonkeSwim::SetStats(UnityEngine::Vector3 stats)
{
    if(useDefault || useGlobal) return;

    maxSwimSpeed = stats.x;
    swimMultiplier = stats.y;
    dragValue = stats.z;

    INFO("maxswimspeed = %d \nswimmultiplier = %d \ndragvalue = %d", maxSwimSpeed, swimMultiplier, dragValue);
}

void MonkeSwim::CalculateVelocity()
{
    //plonking this here just so i don't have to do globalnamespace::ovrinput every time
    using namespace GlobalNamespace;

    if(!canFly) return; 

    bool rightInput = false;
    bool leftInput = false;

    //unity's XR system is buggy, use oculus integration OVR stuff instead
    rightInput = OVRInput::Get(OVRInput::Button::PrimaryHandTrigger, OVRInput::Controller::RTouch);
    leftInput = OVRInput::Get(OVRInput::Button::PrimaryHandTrigger, OVRInput::Controller::LTouch);

    Vector3 rightvelocity = Vector3::get_zero();
    Vector3 leftvelocity = Vector3::get_zero();

    if( rightInput || leftInput){
        
        INFO("input detected, calculating vectors");

        Vector3 cameraOffset(0.0f, Camera::get_main()->get_transform()->get_forward().y, 0.0f);

        float precision = 0.001f;

        float rightSpeed = 0.0f;
        float leftSpeed = 0.0f;

        if(rightInput){
            INFO("calculating right controller vectors");
            rightHand.Update();
            rightSpeed = rightHand.speed;

            if(rightSpeed >= precision) 
                rightvelocity = (rightHand.direction + cameraOffset).get_normalized() * (swimMultiplier * rightSpeed); 

                INFO("right vector \n X: %f \nY: %f \nZ: %f", rightvelocity.x, rightvelocity.y, rightvelocity.z);
        }

        if(leftInput){
            INFO("calculating left controller vectors");
            leftHand.Update();
            leftSpeed = leftHand.speed;

            if(leftSpeed >= precision) 
                leftvelocity = (leftHand.direction + cameraOffset).get_normalized() * (swimMultiplier * leftSpeed);

        }

        if(rightSpeed == 0.0f) rightHand.Reset();
        if(leftSpeed == 0.0f) leftHand.Reset();

        velocity = leftvelocity + rightvelocity;

    } else {
        rightHand.Reset();
        leftHand.Reset();
    }

    if(velocity.get_magnitude() > maxSwimSpeed)
        velocity = velocity.get_normalized() * maxSwimSpeed;

}

void MonkeSwim::UpdateVelocity()
{
    if(!canFly) return;

    //local pointer to the player, if this doesn't exist we can't do anything so just exit the function
    Player* playerInstance = Player::get_Instance();
    if(playerInstance == nullptr) return;

    Rigidbody* playerPhysics = Player::get_Instance()->playerRigidBody;
    if(playerPhysics != nullptr){

        INFO("updating velocity");

        Vector3 playerVelocity = playerPhysics->get_velocity();
        playerVelocity = playerVelocity + velocity;

        if(playerVelocity.get_magnitude() > maxSwimSpeed)
            playerVelocity = playerVelocity.get_normalized() * maxSwimSpeed;
        
        INFO("player velocity \nX: %d \nY: %d \nZ: %d", playerVelocity.x, playerVelocity.y, playerVelocity.z);
        INFO("maxswimspeed = %d \nswimmultiplier = %d \ndragvalue = %d", maxSwimSpeed, swimMultiplier, dragValue);
        playerPhysics->set_velocity(playerVelocity);
    }
    
    //store our current player position to be used for localising hand position in the next frame
    AverageVelocityDirection::lastParentPosition = playerInstance->get_transform()->get_position();

    //reset velocity back to zero
    velocity = Vector3::get_zero();
    
}