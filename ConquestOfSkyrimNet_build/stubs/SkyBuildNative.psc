Scriptname SkyBuildNative Hidden

; Register a clean, nonrespawning Container as a display rack with model-authored slots.
; The loaded rack model supplies the available display pivots.
Bool Function ConfigureWeaponRack(ObjectReference akRack) Global Native

; Remove runtime visual geometry only. Inventory is never removed here.
Function ForgetWeaponRack(ObjectReference akRack) Global Native

; ---- runtime navmesh (for other mods) ----
; Rebuild the navmesh around the player, as the tool wheel's Navmesh sector does.
Bool Function RequestNavmesh() Global Native
; 0 awaiting engine, 1 idle, 2 capturing, 3 building, 4 generated, 5 failed
Int Function NavmeshStage() Global Native
Bool Function NavmeshBusy() Global Native
Float Function NavmeshProgress() Global Native
String Function NavmeshMessage() Global Native
; 1 when these functions exist, 2 with TrackObject (0 from an older SkyBuild)
Int Function NavmeshApiVersion() Global Native

; Another mod's building joins SkyBuild's placed objects so the navmesh cuts around it (version 2).
Bool Function TrackObject(ObjectReference akRef) Global Native
Function UntrackObject(ObjectReference akRef) Global Native
Bool Function IsTracked(ObjectReference akRef) Global Native
