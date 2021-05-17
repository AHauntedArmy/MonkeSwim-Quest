#include "trigger/SwimTrigger.hpp"
#include "swim/MonkeSwim.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/GameObject.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"

DEFINE_TYPE(Swim::SwimTrigger);

namespace Swim{
    void SwimTrigger::OnTriggerEnter(UnityEngine::Collider* enter)
    {
        if(to_utf8(csstrtostr(enter->get_gameObject()->get_name())) != "Body Collider") return;

        MonkeSwim::SetStats(get_transform()->GetParent()->get_localPosition());
        MonkeSwim::EnableMod(true);
    }

    void SwimTrigger::OnTriggerExit(UnityEngine::Collider* leave)
    {
        if(to_utf8(csstrtostr(leave->get_gameObject()->get_name())) != "Body Collider") return;
        MonkeSwim::EnableMod(false);
    }
}