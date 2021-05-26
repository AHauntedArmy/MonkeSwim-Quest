#include "main.hpp"
#include "swim/MonkeSwim.hpp"
#include "trigger/SwimTrigger.hpp"
#include "MonkeMapLoader/include/Behaviours/MapLoader.hpp"
#include "MonkeMapLoader/include/Behaviours/Teleporter.hpp"
#include "modloader/shared/modloader.hpp"
#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "UnityEngine/Collider.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

//stolen from spacemonke, seemed pretty hand for logging information
#define INFO(value...) getLogger().info(value)
#define ERROR(value...) getLogger().error(value)

// Loads the config from disk using our modInfo, then returns it for use
Configuration &getConfig()
{
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger &getLogger()
{
    static Logger *logger = new Logger(modInfo);
    return *logger;
}

MAKE_HOOK_OFFSETLESS(Player_Update, void, Il2CppObject* self)
{
    INFO("player update was called");
    MonkeSwim::CalculateVelocity();
    Player_Update(self);
    MonkeSwim::UpdateVelocity();
}

//MapLoader::Loader::JoinGame()
MAKE_HOOK_OFFSETLESS(MapJoined, void)
{
    INFO("MapJoined was called");
    MapJoined();   
    
    if(MapLoader::Loader::mapInstance != nullptr){
        INFO("map exists, starting mod");
        MonkeSwim::StartMod();
    }
}


//MapLoader::Teleporter::Trigger(UnityEngine::Collider*)
MAKE_HOOK_OFFSETLESS(MapLeft, void, MapLoader::Teleporter* self,  UnityEngine::Collider* collider)
{
    MapLeft(self, collider);

    INFO("we have entered a teleporter");

    /**
    //MapLoader::TeleporterType teleport = CRASH_UNLESS(il2cpp_utils::GetFieldValue<MapLoader::TeleporterType>(self, "teleporterType"));

    //if we teleport back to the tree house, turn the mod off
    //if(teleport == MapLoader::TeleporterType::Treehouse)
    **/

    if(self->teleporterType == MapLoader::TeleporterType::Treehouse){
        INFO("we have returned to the treehouse, turning off mod");
        MonkeSwim::EnableMod(false);
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load()
{
    il2cpp_functions::Init();

    Logger& logger = getLogger();

    logger.info("making sure MonkeMapLoader exists");
    
    Modloader::requireMod("MonkeMapLoader", "1.0.2");

    logger.info("Installing hooks...");

    // Install our hooks
    INSTALL_HOOK_OFFSETLESS(logger, Player_Update, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, MapJoined, il2cpp_utils::FindMethodUnsafe("MapLoader", "Loader", "JoinGame", 0));
    INSTALL_HOOK_OFFSETLESS(logger, MapLeft, il2cpp_utils::FindMethodUnsafe("MapLoader", "Teleporter", "Trigger", 1));

    logger.info("Installed all hooks!");

    INFO("registering swimtrigger");
    custom_types::Register::RegisterType<Swim::SwimTrigger>();
}