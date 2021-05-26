#pragma once

#include "Velocitydirection.hpp"
#include "UnityEngine/Vector3.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

class MonkeSwim
{
    public:
        //flag to know if the mod is allowed
        static bool canFly;
    
    private:
        //flags for using the same settings everywhere
        static bool useDefault;
        static bool useGlobal;

        //modifiers for the settings, use SetStats(UnityEngine::Vector3(x, y, z)) to adjust the settings.
        static float dragValue;         //vector3.z
        static float swimMultiplier;    //vector3.y
        static float maxSwimSpeed;      //vector3.x
       
        static UnityEngine::Vector3 velocity;
        
        static AverageVelocityDirection rightHand;
        static AverageVelocityDirection leftHand;

    public:
        static void StartMod();
        static void EnableMod(bool toEnable);
        static void SetStats(UnityEngine::Vector3 stats);

        //prefix patch on pc
        static void CalculateVelocity();
        //postfix patch on pc
        static void UpdateVelocity();
};