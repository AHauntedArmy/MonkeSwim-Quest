#pragma once

#include "UnityEngine/Vector3.hpp"

class AverageVelocityDirection
{
    public:
        float speed;
        UnityEngine::Vector3 direction;

        //static because both instances use the same parent,
        //used to find the local position of the hand in the previous frame, was causing rubber banding without it
        //value is to be updated outside the class in a postfix of player::update()
        static UnityEngine::Vector3 lastParentPostion;
    
    private:
        
        //on pc i'm using reflection to store method and field to get the hand positions from the player class
        //on quest i'm not sure of its garbage collection safe so i'm using a flag to determine which hand instead
        bool isRightHand;
        
        int vectorAmount;
        UnityEngine::Vector3 velocityDirectionAccumulator;

        //private default constructor to force use of AverageVelocityDirection(bool) contructor
        AverageVelocityDirection(){}

    public:
        AverageVelocityDirection(bool hand);
        
        void Update();
        void Reset();
};
