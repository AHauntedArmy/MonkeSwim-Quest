#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Collider.hpp"

DECLARE_CLASS_CODEGEN(Swim, SwimTrigger, UnityEngine::MonoBehaviour, 
    DECLARE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* enter);
    DECLARE_METHOD(void, OnTriggerExit, UnityEngine::Collider* leave);

    REGISTER_FUNCTION(
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_METHOD(OnTriggerExit);
    )
)