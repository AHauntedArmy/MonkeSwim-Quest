& $PSScriptRoot/build.ps1
if ($?) {
    adb push libs/arm64-v8a/libMonkeSwim-Quest.so /sdcard/Android/data/com.AnotherAxiom.GorillaTag/files/mods/libMonkeSwim-Quest.so
    if ($?) {
        & $PSScriptRoot/restart-game.ps1
        if ($args[0] -eq "--log") {
            & $PSScriptRoot/start-logging.ps1
        }
    }
}
