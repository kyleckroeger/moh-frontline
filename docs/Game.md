# Game code

Frontline's own code (EA's engine and the game) is reconstructed in `src/game/`
as `reconstructed_game` units. No public source matches it byte for byte; the
references are semantic.

## Compiler profile

Established on `bstimer.cpp`, whose six functions (2,020 bytes) and exception
tables verify:

- CodeWarrior GC/1.3 or later (1.3, 1.3.2 and 2.x produce identical code for
  this file; GC/1.2.5n, which builds the Dolphin SDK, does not). Units use
  GC/1.3.2 until a file distinguishes the release. A working profile is not
  proof of the original release.
- `-O4,p` with instruction scheduling on, `-use_lmw_stmw on`,
  `-Cpp_exceptions on`, `-inline auto` (not deferred: functions are emitted in
  source order and only earlier definitions are inlined), `-RTTI off`,
  `-str reuse`, `-enum int`, `-fp hardware`, default small-data thresholds.

### Additional evidence from `propdat.cpp` (not yet verified as a unit)

Per-function comparisons of a `propdat.cpp` candidate (38 of its 43 functions
match; the unit is not accepted) add three observations. `bstimer.cpp` compiles
identically with and without both flag changes, so its manifest is unaffected.

- **`-fp_contract on`.** Float code uses fused `fmadds`/`fnmsub` (distance
  sums, the inline `sqrtf` refinement steps). Without the flag the compiler emits
  separate multiplies and adds.
- **`-str reuse,readonly`.** The file's string literals (the MMG warnings,
  `"Unknown property type\n"`) are in `.rodata`, after a 108-byte header-defined
  `dwi::dwi_prime_list`. Without `readonly` they go to `.data`.
- **Release.** GC/1.3 and GC/2.0p1 produce different code for this file;
  GC/1.3.2, 1.3.2r, 2.0, 2.5, 2.6 and 2.7 produce identical code. This narrows the
  candidate releases but still does not identify the original one.

Other compiler behaviour that the source has to reproduce:

- Templates not declared `inline` (`offsetPtr<T>`) are instantiated at the end of
  the unit as weak functions and are never auto-inlined; `inline` templates are.
- Stack slots for inlined temporaries are grouped by inline depth (deepest
  lowest), in source order within a depth. In the endian-swap code this determines
  the field types and which small inline wrappers exist.
- A by-value parameter or a user-declared copy constructor keeps a `CVector3` in
  memory, which changes both scheduling and whether multiply-adds are fused.

## References

Rising Sun (GR8E69, [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun))
is EA's next game on the same engine. Its reconstruction is ProDG code, so its
bytes never match, but its structure and descriptive names guide Frontline's
source. About 153 KB of Frontline's unfinished functions share a name with a
Rising Sun function; Rising Sun has reconstructed about 13 KB of those. Names
and layouts it marks as descriptive stay descriptive here.

## Method

1. Read the function's instructions and relocations; recover only the accessed
   fields and strides. Partial layouts are "views": unknown bytes stay
   `unknownNN` arrays and comments say what is established.
2. Write the function, preferably in the reference's structure. Rising Sun's
   small helpers are often inlined in Frontline (`bstimer.cpp` inlines
   `BSTimerRemoveTimerEvent`, `BSGetFreeTimerEvent` and the duplicate/latter
   removal loops); keeping them as `static` helpers reproduces the code.
3. Compare per function, then tune what the compiler leaves to the source:
   - Register allocation follows declaration order. In `bstimer.cpp` the
     declaration order of locals decided every remaining difference; search
     small permutations rather than guessing.
   - Store order through one pointer follows statement order.
   - A `switch` should list only the cases the code tests.
   - An extern object's size decides small-data addressing
     (`g_pMemBlockAllocator` is 28 bytes, so it is not in `.sbss`).
4. Draft the unit with `tools/port_unit.py`. It places `extab`/`extabindex` by
   their references to kept functions and keeps only the kept functions'
   entries.

## Units

| File | Functions | Bytes | Reference | Notes |
| --- | ---: | ---: | --- | --- |
| `bstimer.cpp` | 6 | 2,020 | Rising Sun `src/script/timer_*.cpp` | `BSObject` is a view (`queueIdentity` at `+16`); `BSTimerEvent_struct.ownsEventMemory` is a byte, `DoWeOwnThisMemory` returns `bool`; `BSInitTimer` links events forward |
| `bsmachin.cpp` (fragment) | 36 | 5,192 | Rising Sun `src/script/*.cpp` | `0x80036d2c`-`0x80038174`: the behaviour-script opcode functions (`TRACE` to `BNE`), thread execution/creation/destruction and `BSEndMachine`, with all of the file's data (`g_iDefaultStackSize = 64` is in `.sdata`). `BSOpCodeFunc_NEXTSTATE` (first, 58 lines differ) and `BSInitMachine` (12) are not matched, so the property readers after `BSInitMachine` cannot form a second fragment: they share the file's pooled `.sdata2` int-to-float constant |
| `propdat.cpp` (fragment) | 14 | 5,492 | Rising Sun `src/bpd/endian.cpp` | `0x80040c6c`-`0x800421e0`: the `EndianSwap` conversions (see "Endian conversions") and the machine-gun lookups (`SearchForClosestMachineGun`, `IsMGUsed`, `MarkMGAsUsed`) with the file's first four `.sdata2` constants. `sqrtf` is the SDK `math.h` `extern inline` form (volatile result). `CVector3` behaves as 8-byte aligned. The rest of the file stays original context |
| `surfacetype.cpp` | 5 | 116 | — | Lookups into the collision database's 12-byte surface table (`CDB +0x34`); the shoot-through flag is a one-bit field |
| `MallocInit.cpp` | 1 | 144 | — | Heap set-up from the OS arena (less 24 MB above that size) |
| `real_bridge.cpp` | 3 | 196 | — | REAL runtime start-up, update and shutdown calls |
| `system.cpp` | 3 | 208 | — | `SysInit`/`SysShutdown` with the module-active flag and a 36-byte critical section |
| `gcSystem.cpp` | 5 | 232 | — | Critical sections over `MUTEX_create`/`MUTEX_destroy` (a 28-byte mutex, owner thread and count) and `SysInitDependent` (`OSInit`, `DVDInit`) |
| `dmgeom.cpp` (fragment) | 1 | 44 | — | `DMGeomSetNodeState` only; `DMGeomResetState` is drafted but not matched |
| `LinkedList.cpp` | 8 | 424 | — | Singly linked list (head, tail, count) and its element |
| `texpack.cpp` | 6 | 428 | — | Texture pack lookup (`bsearch` over 16-byte names) and offset fix-up; with its weak `offsetPtr` instantiations. Its own copy of `offsetPtr<void>` was dropped by the linker in favour of propdat's, so the source declares that specialisation instead of instantiating it |
| `anim.cpp` | 11 | 532 | — | Animation module start-up, the user-opcode callback stacks and the global `g_AnimDB` (constructed by `__sinit_anim_cpp`) |
| `orient.cpp` | 2 | 300 | — | Fixed-point (one turn = 0x1000000) shortest-way angle interpolation, `extern "C"` names; the 64-bit product shape fixes where the difference is taken |
| `memory.cpp` | 7 | 748 | `DWI_alloc`/`DWI_allocalign` and the global `operator new`/`new[]`/`delete`/`delete[]` on the REAL heap; `DWI_alloc` is inlined into the `new` forms (hence two `memset` calls), and the `throw()` deletes keep their exception-spec frames |
| `vector.cpp` | 2 | 440 | `CVector3::Constrain` (spherical interpolation toward a target; Frontline's version takes a const target and has no opposite-vector case, unlike Rising Sun's) and `CVector2::Rotate`; the scaled target is an inline `float * CVector3` temporary |
| `animwgt.cpp` | 10 | 1,232 | — | Animation weights: constant, linear, fade in-hold-out and wait-to-stop updates through `_AnimWgt_pFunctions`, with per-type parameter sizes in `_AnimWgt_TypeStructSize` (0, 8, 4, 16). `AnimWgtSet`'s body is inlined into the typed setters, so the source uses an inline helper (inferred); the fade setter needs `start = weight != -1.0f ? weight : (weight = info->weight)` |
| `anim_callback.cpp` (fragment) | 1 | 1,596 | — | `EndianSwapAnimPlugIn`: when the plug-in version shows the other byte order, swaps the version and length, then the fields of the layout chosen by the state's selection callback (random, turn, death, weapon, aim, floco with its entries; none for pathing and face). Layout views are inferred; the aim fields use the int helper directly and the others the template, which matches the target's stack grouping |
| `animchan.cpp` (fragment) | 5 | 320 | — | Channel accessors at the start of the file: speed, delay by state, state-channel lookup by index (`int` counters cast to 16 bits) and the running/active counts; the comparisons are written `state == channel->state` and the loops advance `channel++, i++`. The five functions from `AnimChanSwitchAnimByDuration` on are not reconstructed |
| `animseq.cpp` (fragment) | 5 | 484 | — | Opcode parameter reader (12-bit signed values, extended by a second word), duration scaling (16 ticks per frame at the list's frame rate) through the inlined body of `AnimSeqSetRate` (an inferred helper), and frame-count/life queries through the file-local `_AnimSeqParseToEnd` (a local external of this fragment). `AnimSeqGrow`, `AnimSeqStart` and the two parsers are not reconstructed |
| `AIObject.cpp` (fragment) | 4 | 164 | — | `CAIObject` distance helpers (squared XY, squared XYZ to another object or to the target position, XYZ) forwarded to `CAIFilterRealPosition` (position at +24 and target position at +240, inferred) |
| `AIFilter.cpp` (fragment) | 9 | 884 | — | AI filter helpers after the first function: MMG queries forwarded to `CAIObject`, the target guess position (a `CVector3` copied by value, then its coordinates; inferred inline constructor), `CAIFilterRealEulerDirection::CalculateAngleDiff` (atan2, rotate, normalise), `CAIFilterRealVector3::RotateAboutZ` (old y and x kept in locals; `math.h` included with C linkage) and the real-position distances (the SDK's inline `sqrtf`; the XY distance adds a zero z squared). `CAIFilterPlayerObject::GetLookDirection` before them is off (stack order of its copies) and the rest of the file is not reconstructed |
| `AIPlayerDoodad.cpp` (fragment) | 5 | 288 | — | `CAIPlayerDoodad`, the player counterpart of `CAITankDoodad` (same inferred non-virtual view): its 24-byte `CAIFilterPlayerObject` is created with plain `new` only when missing, and clean-up deletes it without clearing the pointer. The destructor and constructor (vtable) are not reconstructed |
| `AIPlayerObject.cpp` (fragment) | 5 | 260 | — | `CAIPlayerObject` through an inferred non-virtual view: `IsPlayer` (weak), crouching through the scene's first player, and `Init`/`Update` setting the AI health as `15 * current / maximum + 0.999` (rounded up to fifteenths) and fixed state values. The destructor and constructor (vtable) are not reconstructed |
| `AISplinePath.cpp` (fragment) | 2 | 252 | — | `CAISplinePathManager::GenerateSplinePath` (a test path and its reverse in the second half of the path array, the reverse id tagged with bit 24) and `FreeGenerateBuffers`. `AllocateGenerateBuffers` names its buffers with strings 108 bytes into the file's string pool, so it and the constructor are not part of the unit |
| `AISoldierDoodad.cpp` (fragment) | 4 | 340 | — | `CAISoldierDoodad` update (filter update, then script event 124 when one state bit is set and another clear), pre-update, clean-up and `Init` (placement-new of the 20-byte `CAIFilterSoldierObject` at soldier+17584), through the same inferred non-virtual view as the other doodads. `GetSceneNode`, the two fire functions, the destructor and the constructor are not reconstructed |
| `AISoldierObject.cpp` (fragment) | 2 | 588 | — | `CAISoldierObject` target match lists: `SetupTargetMatchList` copies the caller's 12-byte `aistatus_match` entries (`for (...; i++, match++, matches++)`), and `ResetTargetMatchList` loads a built-in list by the stored target mode (the parameter is unused; modes 2 and 3 share one list, 5 another, 4 and 6 a two-entry list, each case written out separately). The match fields and the empty `aifilter_target_mode` enum are inferred. `Init` and `Update` use `.sdata2` constants that start at a 4-mod-8 address, which the build cannot yet place on their own; they and `ChooseTarget` are not reconstructed |
| `SearchNodeList.cpp` | 6 | 788 | — | `CSearchNodeQueue`, a binary min-heap of search nodes by cost (count in the first word, 1-based entries after it; each node keeps its heap index): `Clear`, `PopFirst` and `ReplaceElement` (sift down with `while ((child = hole * 2) < m_count)` and locals in the target's declaration order), `Insert` (sift up), and the empty `CSearchNodeList` constructor and destructor over `CLinkedList`. Members and the heap view are inferred |
| `Pause.cpp` (fragment) | 2 | 228 | — | `Pause::SetState` (on a change: state 3 activates the pause screen and pauses music and ambient sound, 4 mutes and resumes them with a 25.4 fade value, 6 restores the master volume if it is below 127) and `GetObjectiveStatus`. `SetObjectiveStatus` and `AddObjective` follow and are off (the target keeps `index - 1` unfolded); the rest of the file is not reconstructed |
| `popup.cpp` (fragment) | 22 | 228 | — | `PopUpMessage` accessors from `GetString` to `Set_placement` (getters, `SetString` through `strncpy`, setters; members named after the accessors, inferred offsets). Compiled with `-inline deferred,auto` and listed in reverse. The scrolling functions before them (`Scroll` is off) and `Reset`, the destructor and the constructor after them (their small data starts at a 4-mod-8 address) are not part of the unit |
| `UserInterface.cpp` (fragment) | 1 | 280 | — | `UserInterface` destructor: outside multiplayer it deletes the HUD sprites and the bullet-count font through their virtual destructors. `CRenderBin` is an inferred view with its virtual table pointer after 28 bytes of members (declared before the virtual destructor). The constructor after it uses the file's float constants, so the static initialiser for `teamColors` (which matches) cannot join the unit |
| `ConfigGC.cpp` (fragment) | 1 | 184 | — | `CDualConfig` destructor: deletes its three held objects, each tested for null first (the test is doubled in the code), through the virtual destructor slot of the `CRenderBin` view (exact classes unknown). The static initialiser after it uses the file's float constants and a numbered static |
| `pumhandler.cpp` (fragment) | 1 | 204 | — | `PopUpMessageHandler` destructor: deletes three held objects through the virtual destructor slot of the `CRenderBin` view (exact classes unknown), clears one pointer, then its array of 20 `PopUpMessage`s is destroyed with `__destroy_arr` |
| `devicemgr.cpp` (fragment) | 7 | 388 | — | `CDeviceManager` from `GetDeviceState` to the constructor: four 2316-byte `CGCDevice`s; a device's state while its pad reads without error, readiness, pad and device updates, an empty shutdown, initialisation (port, cleared state, copy of `masterEventTable`) and the empty destructor and constructor. Device layout and pad view inferred. `UpdateActuators` and `SetActuatorState` (file constants, jump table) before them are not part of the unit |
| `player_weapon_object.cpp` (fragment) | 10 | 364 | — | `CPlayerWeaponObject` start, through an inferred non-virtual view with casts to its `CAnimObject` base: weak `Init`, casts and script accessor, visibility (the draw-enabled virtual, slot from `__vt__10ISceneNode`), the sleeve texture from the level texture pack (resource type 13), destruction through `g_AnimObjFactory`, `SetMuzzleStatusForPlayer` (sets a bit in the current weapon's flag byte through a local) and the weapon name. `CreateEjectedShell` on are not reconstructed |
| `Soldier_object.cpp` (fragment) | 7 | 216 | — | `CSoldierObject` start, through an inferred non-virtual view: the weak AI-doodad accessor and identity casts, `Destroy` (first virtual of the script object's user, as in `bsmachin.cpp`), head-transition and head-tracking setters and `FadeOut` (`a = b = c = 0.0f` stores the last first). `SetBulletEmitter` is 12 lines off (register choice in its emitter-name search) and the rest of the file is not reconstructed |
| `collisionvolume.cpp` (fragment) | 7 | 160 | — | `CCollisionVolume` start, through an inferred non-virtual view: weak collision-id and script accessors and identity casts, `Destroy` (first virtual of the script object's user) and `MarkForDestruction` (subject, then removal from `g_scene`). The bullet handler follows; the row accessors later in the file copy a 16-byte row that the view cannot reproduce without inventing a type |
| `hearingvolume.cpp` (fragment) | 3 | 168 | — | `CHearingVolume` weak collision-id accessors (inferred non-virtual view) and `PlayBulletWhizBys`, which plays the whiz-by sound at the bullet's position and heading (virtual calls in the order of `__vt__10ISceneNode`; 16-byte `CVector3` passed by value) for the scene's first player. The bullet handler follows and is not reconstructed |
| `ShellMenu.cpp` (fragment) | 6 | 48 | — | `CShellMenu` stage, mission, difficulty and controller-setting getters and the stage and mission setters (members named after the accessors, inferred offsets) |
| `thrown_obj.cpp` (fragment) | 6 | 40 | — | `CThrownObject` weak defaults (object type 4, cast, empty script translate/rotate/scale) and `SetDeleted` (a bit in the flag byte at +756, inferred bit-field view) |
| `thrownbullet.cpp` (fragment) | 6 | 48 | — | `CThrownBullet` weak bounding-volume, collision-enabled, attached-light, damage (through the weapon record) and cast accessors; the velocity getter that follows copies a 16-byte vector as doublewords |
| `projectilebullet.cpp` (fragment) | 5 | 40 | — | `CProjectileBullet` weak bounding-volume, damage, fired-by and cast accessors |
| `world_volume.cpp` (fragment) | 5 | 40 | — | Weak `CWorldVolume::TestCollision` defaults (no collision) for triangles, planes, points, world volumes and CDB objects |
| `cdbobject.cpp` (fragment) | 5 | 40 | — | The same weak `TestCollision` defaults for `CCDBObject` |
| `csg_volume.cpp` (fragment) | 2 | 296 | — | `CCSGVolume`: empty `TransformedCopy` and `GetExtents`, the union of the hierarchy object's sub-volume extents starting from `FLT_MAX`/`-FLT_MAX` (sub-volume `GetExtents` virtual in the order of `__vt__7IVolume`). `Create` (which stores the vtables) and the collision tests are not reconstructed |
| `cdbgeom.cpp` (fragment) | 5 | 156 | — | Weak `CCDBVolBox` collision tests: none against world volumes, otherwise the `CVolBox` base tests (point by value, CDB object, box, generic volume) |
| `animated_volume.cpp` (fragment) | 8 | 444 | — | `CAnimatedVolume` collision tests: the `CVolSphere` base tests for triangles, planes, points, capsules, spheres and boxes; against CDB objects and generic volumes it swaps the collision order and calls the other volume's test (virtual order of `__vt__7IVolume`) |
| `sphere.cpp` (fragment) | 3 | 240 | — | `CVolSphere` collision tests against boxes and generic volumes (swap the collision order and let the other volume test, virtual order of `__vt__7IVolume`) and `GetRadius`. The centre accessors that follow copy 16-byte vectors as doublewords |
| `Screen.cpp` (fragment) | 4 | 32 | — | `CScreen` depth and frame formats (by address) and width and height (inferred offsets) |
| `staticmesh.cpp` (fragment) | 3 | 36 | — | Weak `CStaticMesh::IsFoggingEnabled` (a `bool` bit-field) and `CStaticAnimMesh` ticks per frame and frame count from its animation header |
| `attachobject.cpp` (fragment) | 4 | 28 | — | `CStaticObject` weak casts, draw-enabled flag (`bool` bit-field at +480) and attached light |
| `animated.cpp` (fragment) | 4 | 28 | — | `CAnimated` weak defaults: no rotation or translation to extract, empty rotation setter, null morph cast |
| `skybox.cpp` (fragment) | 3 | 20 | — | `CSkyBox` weak visibility (always) and draw-enabled defaults and the empty `SVertex` constructor |
| `compartment.cpp` (fragment) | 3 | 32 | — | `CCompartment` visibility and draw-enabled queries and the static shadow-texture setter on `g_CptShadowBin` |
| `Tank_object.cpp` (fragment) | 3 | 20 | — | `CTankObject` weak AI-doodad accessors (doodad at +1856) and cast |
| `player.cpp` (fragment) | 3 | 16 | — | `CPlayerObject` weak AI-doodad accessor (+36) and identity casts |
| `UIStudio.c` (fragment) | 3 | 28 | — | Registration of the client transform, resource and message callbacks in the studio record (inferred slots; deferred inlining, reverse order) |
| `matrix.cpp` (fragment) | 4 | 112 | — | `CMatrix` row setters (position, up, front, right) copying a by-value `CVector3` coordinate by coordinate (rows of 16 bytes, inferred) |
| `quaternion.cpp` (fragment) | 1 | 204 | — | `CQuaternion::EndianSwap`: the vector part, then the scalar (stored first), through the inlined float `EndianSwap`/`ChangeEndian` helpers of `propdat.cpp` |
| `rcmp_main.cpp` (fragment) | 2 | 84 | — | `RCMP_Initialize` (framework `Init`, then the RCMP system REAL defaults) and `RCMP_Shutdown` (framework `Restore`) on `g_fw` |
| `IStudio.cpp` (fragment) | 2 | 80 | — | `IStudio` screen activation and loading forwarded to the UI studio (`UISSetScreenActive`, `UISLoadScreen` with zero extra parameters) |
| `IStudioFuncs.cpp` (fragment) | 2 | 116 | — | Unloading and loading the shell big file (`g_pLevelBigFile`, loaded through `TLT_LoadFileNormal` with alignment 256) |
| `bulletfactory.cpp` (fragment) | 3 | 160 | — | `CBulletFactory`: whether a thrown bullet type can be cooked, a bullet type's sprite from the projectile (64-byte) or thrown (76-byte) property tables, and destruction through the projectile or thrown `CBulletFactoryHelper` |
| `weaponfactory.cpp` (fragment) | 2 | 88 | — | `CWeaponFactory` destructor (nothing to release) and constructor (cleared fields) |
| `weapon.cpp` (fragment) | 3 | 96 | — | `CWeapon` single-round reload from the reserve (up to the clip size), starting a reload (state 1 and the reloading bit) and emptying the clip and reserve |
| `bsbifunc.cpp` (fragment) | 3 | 120 | — | Built-ins that only drop their arguments (stop, continue and start orienting towards a cover point): the script stack pointer moves back by the current built-in's argument count |
| `publisher.cpp` (fragment) | 2 | 88 | — | `ISubject::MarkForDestruction` (destructible base, then notify event 2) and the weak `NotifyObservers` through `CPublisher::PublishEvent` |
| `hierobject.cpp` (fragment) | 3 | 120 | — | Weak `CMatrix::PostMultiply` and `CStaticObject::IsFollowingPath` (path state other than 0 or 1, through a result local) and `CHierObject::BeginUpdate` (skipped while paused) |
| `hwvideodisplaylist.cpp` (fragment) | 2 | 108 | — | `GCHW_VD` scale and position setters recomputing the drawn extent (size × scale + position) |
| `world_object.cpp` (fragment) | 9 | 364 | — | `CWorldObject` start, through an inferred non-virtual view: weak identity casts, collision id, draw-enabled and visibility queries, the bounding volume at +272, and `DrawShadows`/`Draw` over the compartment list (nodes of two links followed by a `CCompartment`; the node pointer is declared before the end sentinel). `CCompartment` is declared with the `ISceneNode` virtual functions in the order of `__vt__10ISceneNode`. `BeginUpdate` on are not reconstructed |
| `motionblur.cpp` (fragment) | 4 | 84 | — | `CBlurBin::IsUsed` (blur amount non-zero) and `Link` (empty), `CMotionBlur::SetAmount` and `Init` (registers the blur bin with the render list); the file statics are local externals. The bin's destructor (whose inline `CRenderBin` destructor would be emitted as a weak duplicate) and the static initialiser are not part of the unit |
| `font.cpp` (fragment) | 2 | 152 | — | Static `CFontDriver` hooks: `DestroyFont` (empty) and `CreateFont`, which builds a zeroed 72-byte `CTexture` from the font's shape for `g_pCurrentFont` (a local external) and links font, texture and `CFont`. `EndDraw` is off (its colour copy goes through a word temporary on the stack); `StartDraw` and `Draw` match in a draft but follow it, so they are not part of the unit |
| `particlesystem.cpp` (fragment) | 7 | 116 | — | `CPropertyParticleSystem` accessors: the local-to-world matrix and the particle lifetime, system lifetime, emission delay and rate (`short`), render type and seed read through the particle-property record (inferred view). The vector getters before them copy 16 bytes as doublewords and are not part of the unit |
| `AITankDoodad.cpp` (fragment) | 5 | 296 | — | `CAITankDoodad` through an inferred non-virtual view: scene node (the tank), update and pre-update forwarded to its `CAIFilterTankObject`, clean-up (virtual delete) and `Init` (placement-new of the 20-byte filter at tank+2096, link, one-time vars, the tank AI object's `Init`). The destructor and constructor store `__vt__13CAITankDoodad`, whose inline `CAIDoodad` fire methods would be emitted as weak duplicates; they are not reconstructed |
| `animdb.cpp` (fragment) | 8 | 1,264 | — | `CAnimDatabase`: two object-type groups of up to 20 animation files (an inlined group search, inferred), state lookups forwarded to the files (`GetStateInfo`/`GetStateCallbackID` take the state first, then the object type), `AddFile` (copy, upload frames to ARAM, add to the type's group) and `Init` (ARAM pool manager and the 8 MB animation pool, with `g_ARAM_*`). `GetFileFromFileNum` comes first and is 18 lines off (register swap; draft in `scratch/lib/animdb_wip.cpp`) |
| `animst.cpp` (fragment) | 6 | 808 | — | State-slot accessors at the start of the file: four 104-byte slots per object (views inferred from offsets) with delay, flag, user-memory and weight lookups by state (the condition reads through a slot pointer, the result is indexed `owner->slots[i]`), the current state (lowest slot in status 3, scanned from the last slot) and the callback-index search in `_AnimSt_CallbackTable` (a local external). `AnimStProcess` on are not reconstructed |
| `skeleton.cpp` (fragment) | 1 | 140 | — | `CharSkelGetJointByName` (`strcmp` over 20-byte joint records). `CharRelocateSkeleton` is drafted with the propdat `ChangeEndian` helpers and is 29 lines off (register numbering and swap stack slots; draft in `scratch/lib/skeleton_wip.cpp`) |
| `animintf.cpp` (fragment) | 2 | 104 | — | `AnimSystem_Shutdown`/`AnimSystem_Init` (registers the user-opcode process and skip handlers). The skip handler's switch is decoded (draft in `scratch/lib/animintf_wip.cpp`) but keeps the opcode in another register (34 lines off) |
| `destructor_queue.cpp` (fragment) | 4 | 384 | — | `CDestructorQueue`: a singleton array of (object, delay) entries; `Execute` counts each delay down and calls the `Destroy` virtual (slot from `__vt__13IDestructible`) when it expires, compacting the rest in place; `Shutdown`, `Reset` and `Init` with the inline constructor and destructor (inferred). `IDestructible::MarkForDestruction` is the vtable's key function, so defining it would emit the vtable with weak copies of the inline destructor and `Destroy`, which the build tooling cannot place yet; it is not reconstructed |
| `bsevents.cpp` (fragment) | 5 | 372 | — | Behaviour-script events, start of the file: the process-state flag (`g_eProcessState`, a local external), event memory blocks from `g_pMemBlockAllocator` with a reference count at +32 (freed and marked `0xFFFF0000` once it drops to zero; block view inferred), and the declared-state search that walks a thread's state and its parents through the sorted event-handler entries. `BSEventProcessEvents` on are not reconstructed |
| `bsfile.cpp` (fragment) | 4 | 280 | — | Script class lookup: `BSFileGetClass` searches `g_pcClassList` by name (an inlined search, written as an inferred helper), rereads an unloaded class or reads and links a new one; `BSFileDeleteClass` closes its two files; empty init and memory hooks. `BSFileReadClass` is not reconstructed |
| `bsobject.cpp` | 6 | 984 | — | Behaviour-script objects: event triggering across an object's machine threads (immediate events queued and processed at once; event 44 before initialisation is only accepted immediately and marks the object initialised when unhandled), destruction, creation from a script class (one thread per code block, context words after the threads, unique IDs from `g_uniqueIDCount`), and the 24-byte `g_pBSObjectAllocator` (880 objects, registered for destruction by `__sinit_bsobject_cpp`). Members inferred from offsets |
| `objcreate.cpp` (fragment) | 13 | 188 | — | `CDumpObjectContainer`'s destructor (array delete of its objects) and the weak scene-node, script-data, proximity-data and hit-reaction accessors of `BSGO_Dummy`, `BSGO_Destructable`, `BSGO_Tank` and `BSGO_Soldier` (members inferred; result types left incomplete). The object creators before them and the static initialiser after them (whose array destructor is a compiler-numbered `$860` local) are not part of the unit |
| `Moh2.cpp` (fragment) | 36 | 268 | — | Weak inline bodies of the scene-node interfaces emitted in this file, from `ISceneNode::IsVisible` to `AsMovingNode`: false/zero/-1 queries, empty event handlers, the null identity casts (result types inferred as pointers to the named classes) and `IDestructible::Destroy`. The neighbouring bodies that load `.sdata2` constants (default vectors) and the rest of the file are not part of the unit |
| `volume_query_test.cpp` (fragment) | 16 | 120 | — | Weak inline defaults of `IVolume` at the end of the file: `Create`, `TransformedCopy`, every `TestCollision` overload (no collision), `TestVisibility` and `GetExtents` (result types inferred). The two destructors and the query test before them are not part of the unit |
| `bsschedule.cpp` (fragment) | 11 | 784 | — | Behaviour-script schedules: circular lists of 32-byte registration records from a 128-record pool (`g_AllRecords`, `g_FreeRecordList`), register/unregister, init/reset/shutdown. Compiled with `-inline deferred,auto`: `Unregister(BSObject*)` inlines `Unregister(record)`, which follows it in the image, so the source lists the functions in reverse image order. `BSSchedule::Update` (first in the image) is not reconstructed |
| `input.cpp` (fragment) | 9 | 808 | — | `CInputManager` start: clearing the four 189-entry key tables (inside 2316-byte device records of the `CDeviceManager` base; layout and base relationship inferred), readiness, rumble feedback gated by `g_bInMultiplayerMode` and the shell's per-player setting, actuator reset with `PADControlMotor`, key values and keymaps. Compiled with `-inline deferred,auto`: `GetValueAnyPlayer` inlines `GetValue`, which follows it, so the source lists the functions in reverse. `Update`, `Init` and the static instance are not reconstructed |
| `MathFun.cpp` (fragment) | 8 | 656 | — | Percent test (64-bit compare returned as a byte), random percent/real/signed-real/64-bit range values from the library's 31-bit `random()` (zero-extended into 64-bit products, scaled by 2^-31), seeding and the close-to-zero test. The angle functions after them are not reconstructed |
| `bsutil.cpp` (fragment) | 2 | 100 | — | `BSUtilObjectInstanceMemoryAllocator::GetFreeElement`/`FreeElement` (free-list link stored after each element). `Init` and `DoWeOwnThisMemory` differ only in register numbering (draft in `scratch/lib/bsutil_wip.cpp`); the static initialisation of `g_pMemAllocators` matches but cannot be verified because its compiler-numbered array destructor (`__arraydtor$623`) gets another number when compiled alone |
| `rcmpbase.cpp` (fragment) | 3 | 292 | — | `RCMP::DECODER::FreeChosenCodec`, `ChooseCodec` and the destructor (class `operator delete` through `rcmp_sys`'s free hook), with `DECODER`'s vtable and RTTI. The decoder keeps its stream callbacks in an embedded `CODEC_IDATA` (default constructor: value 2, the rest 0). The constructors and chunk accessors before them match except three frame accessors (6 lines, branch layout) and the `DECODER` constructor (4 lines); drafts in `scratch/lib/rcmpbase_wip.cpp` |
| `audioplayer.cpp` | 5 | 652 | — | `RCMP::AUDIO_PLAYER`, the movie player's audio: a tap-mode stream (30 ms overhead, buffer `AV::audiobuff` from the RCMP system allocation hook, freed with `MEM_free`), started by releasing the request hold, sped up through the stream pitch multiplier, finished when the request status reports nothing queued (`bool` result); deleting goes through the system's free hook. Members and the hook parameters are inferred |
| `avplayer.cpp` (fragment) | 2 | 96 | — | Weak class deletes of `RCMP::AV_MS_TIMER` and `RCMP::DECODER` through the RCMP system free hook |
| `rcmp_mpc_codec.cpp` (fragment) | 3 | 68 | — | Weak `MPC_FRAME` class delete, `MPC_CODEC_INTERNAL::GetFrameRate` (the decoder double `MPDframe_rate` as a float) and the current frame number |
| `swvideodisplaylist.cpp` (fragment) | 3 | 96 | — | `GCSW_VD` (software video display): `Draw` stores the frame and calls the `ReDraw` virtual (order of `__vt__7GCSW_VD`), empty `SetScale`, `SetPosition` stores integer x and y |
| `animemot.cpp` (fragment) | 2 | 88 | — | ARAM transfer callbacks: record the completion tick in `end` and clear `g_bDMAToARAM` or `g_bDMAToMRAM` |
| `sound.cpp` (fragment) | 10 | 396 | — | `CSoundSysLock` (enters the sound critical section) and the `CSoundStream` wrappers over the EA stream API: pause and unpause through the pitch multiplier (0 and 4096), fades, volume, request and stream status, purge and file queueing (stream handle at the start of the object, inferred) |
| `matstack.cpp` (fragment) | 14 | 1,020 | — | `CMatrixStack`: push/pop/load/store/identity/row scaling on a stack of matrices allocated with `DWI_allocalign`, the current-stack pointer and the global `g_matStack` created by `InitClass` (which inlines the constructor, `Allocate(8)` and `SetCurrent`). Only `__sinit_matstack_cpp` is left out: it constructs an unidentified global `CMatrix` |
| `anim_obj.cpp` (fragment) | 22 | 344 | — | The first `CAnimObject` functions: the empty overrides (weak in the image: inline in the class and emitted for its vtable, so `__declspec(weak)` here), `MarkForDestruction` (subject base, then removal from `g_scene`), attachment flags, `SetTMLocalToWorld` and the light-volume manager calls. The class is an inferred non-virtual view (members at their offsets in a ~9.4 KB object). The row accessors after them copy 16-byte vectors with `lfd`/`stfd` into the result, which no honest `CVector3` view reproduced (draft in `scratch/lib/anim_obj_wip.cpp`) |
| `AnimObjFactory.cpp` (fragment) | 3 | 1,016 | — | Weak endian conversions of the object collision data (count and a second word, then each 128-byte box), each box (its `CVolBox`, then the word at +112) and `CVolBox` (width, depth, height, then three basis rows and the centre through an always-inlined `CVector3` swap, inferred) |
| `fader.cpp` (fragment) | 2 | 120 | — | `CFaderControl::IsFading` and the constructor (its `CColor` members copied byte by byte, so `CColor` has a member-wise copy constructor; the vtable is external because `Update`, the key function, is outside the unit). The `CFader` functions after them are drafted with deferred inlining; `SetControl` is 30 lines off (it keeps two colour temporaries; draft in `scratch/lib/fader_wip.cpp`) |
| `dmmorph.cpp` (fragment) | 5 | 400 | — | Morph blend buffers: `CMemBuffer` (frees its memory when owned) and `CBlendBufferCache` (array delete of its entries; each `BufferCacheData` holds two memory buffers, members initialised before the array, implicit weak destructor). The blending functions and the rest of the file are not reconstructed |
| `dmrender.cpp` (fragment) | 14 | 548 | — | `DMRender*`: binding a mesh (marks the geometry state dirty and sets flag bits 0x411), cluster/morph objects, matrices, scale and flags on a 608-byte `DMRenderObject_T`, the part list rebuilt when the geometry state is dirty, and the cluster module start-up/shutdown. Record members are inferred from offsets. `DMLightingScaleAlpha` and `DMLightingUpdate` come first and are not reconstructed |
| `UISEvent.c` | 5 | 840 | — | UI rate functions: a table of 52-byte entries (by function id and control) that move a control value toward a target, with load, advanced load (rate from the current action value over a number of frames), unload and removal passes. Compiled as C++ with deferred inlining: the search in `UISFindRateFnc`, last in the image, is inlined into the others, so the source lists functions in reverse image order |
| `UISActionProcess.c` (fragment) | 1 | 204 | — | `UISAddThreadAction`: pushes an action (id, 16-byte group record, parameter count, parameters in reverse) on the downward-growing UI action stack. `_UISDoThreadAction` (10-case dispatch that re-queues failed unloads through the inlined add) and `UISProcessThreadAction` are drafted with every call and field placed but differ in register assignment (`scratch/lib/UISActionProcess_wip.c`) |
| `UISUtils.c` (fragment) | 3 | 260 | — | The lookups at the end of the file: sub-control event and event code addresses in a control's 8-byte entries (kind in the top flag bits) and a screen index by its two ids. The file-local `_UISFindHintPC` before them matches too but is only kept by its caller, the hint parser, which is not reconstructed |
| `soundtable.cpp` (fragment) | 5 | 216 | — | `CSoundTable`'s destructor (closes the loaded file), the three weak `offsetPtr` instantiations (explicitly instantiated here from a `__declspec(weak)` template, since their user `Init` is outside the unit) and the static initialisation of `g_soundMapSingleton`. The event lookups, `Init` and `EndianSwap` before them are not reconstructed |
| `configfile.cpp` (fragment) | 4 | 640 | — | `CConfigFile`: section selection (`stricmp` over 64 `CConfigSection` name/start/end records), the destructor, and the constructor that splits the loaded file at `[`/`]` (the section end is `p = sections[n].end = strchr(...)`, in that order). `GetString` and `GetBool` come first and share an inlined value lookup; they are 3 and 5 lines off (draft in `scratch/lib/configfile_wip.cpp`) |
| `draw_context.cpp` | 1 | 260 | — | `CDrawContext`'s constructor: three `CMatrix` members (whose inline constructor calls `CMatrix::InitClass` once), references to the four source matrices, cleared state and three 1.0 scales; it multiplies the matrices and loads the projection and view into GX. Both class layouts are inferred views |
| `viewport.cpp` | 3 | 660 | — | `CViewport`: normalised rectangle to GX viewport (jittered when the screen's render mode says so) and scissor, and the cached clip-to-view matrix. `CVector3` is passed by value with `lfd`/`stfd` copies, so this file's view of it is 8-byte aligned (x, y, z padded to 16 bytes); `CScreen`'s two virtual size queries (vtable `+0x24`/`+0x28`) and its mode pointer (`+0xB0`) are inferred |
| `framework.cpp` | 2 | 8 | Empty `FRAMEWORK::Init`/`Restore` |
| `isexportdefs.cpp` | 1 | 16 | `NullifyScreenAndLibrary` |
| `isShellGroup.cpp`, `ispausegroup.cpp`, `isShellLibrary.cpp`, `ispauselibrary.cpp` (fragments) | 4 | 56 | One-line selectors of the shell/pause screen and library tables; the files' string data is not reconstructed |
| `trig.cpp` (fragment) | 4 | 272 | `MathArcTan2`, `MathSinCos`, `MathCosf`, `MathSinf`. `MathLLAngleInit` (Taylor tables) is left out: 4 instructions differ (element 0 of `_Math_TaylorConst` is addressed through a copied base register), and a 12-byte object the linker stripped sits between the tables |

## Endian conversions

`propdat.cpp` starts with `EndianSwap` for each property record. As in Rising
Sun, each converted field gets one `ChangeEndian` call. Frontline inlines every
conversion, so the overloads are recovered from the code they leave:

- `ChangeEndian(short&)`, `ChangeEndian(int&)` and `ChangeEndian(unsigned int&)`
  copy the value to a local byte array and swap it. The 32-bit form is written
  as two explicit pair swaps; a loop reproduces the bytes of most functions but
  moves the scheduler's split points in others.
- A `ChangeEndian` template converts other 32-bit types (pointers, `unsigned
  long`) through `int`. Floats go through `ChangeEndian(float&)`, which calls
  `EndianSwap(float&, bool)` (a weak function in this file); its `bool` is
  unused and its meaning is unknown.
- Each inline level puts its temporaries in a separate group of stack slots
  (deeper levels lower, source order within a level). The slot order of each
  target function therefore shows which fields are direct `int`s, which go
  through the template and which are floats, and that is how the views' field
  types were chosen. Where only one group appears, the types are not
  established.

The fragment covers only these functions. The remaining propdat functions (BSP
patch-up, trigger setup, player starts and `PatchUpAllPropertyData`) are drafted
in scratch and are not accepted.

## EA library units (`src/ea/`)

EA's shared libraries (the "REAL" runtime, sound and resource code linked into
Frontline) are reconstructed in `src/ea/` as `reconstructed_game` units, from
the disassembly alone (no reference source). They use the game profile with
two differences, both shown by the verified units:

- `-Cpp_exceptions off`: functions that call out (`CPU_detect`, `MEM_initadr`)
  have no `extab`/`extabindex` entries.
- `-str reuse` without `readonly`: string literals are in `.data`/`.sdata`
  (`"Gekko PowerPC"`, `"RAM"`).

The sound library (`SNDI_*`, `SFILTER_*`) never fuses multiply-adds, so its
units are built without `-fp_contract on`, and with `-use_lmw_stmw off`
(`SFILTER_splitter` saves five registers through `_savegpr_27`). A sound filter starts with its
process function, a restore function (`+0x4`, where present) and an optional
input filter (`+0x8`) that is run first; the filter files share that layout.

Some files are named `.c` but have C++-mangled names (`sfir8.c`); they are built
with `-lang c++`. Struct and parameter types are views; where the code does not
establish a type, the source comments say so.

| File | Functions | Bytes | Notes |
| --- | ---: | ---: | --- |
| `fontnull.cpp` | 1 | 4 | Static `NULL_draw` referenced by the global `FONTnulldriver` table (20 bytes; only the draw entry is established) |
| `rcmp_mpc_codec_chunk_types.cpp` | 1 | 28 | Loop over a one-entry anonymous-namespace `ChunkTypes` table (`'MPCh'`) |
| `sfir8.c` | 2 | 244 | Eight-tap symmetric FIR (eight history samples, five coefficients) and its reset |
| `cpudetect.cpp` | 1 | 80 | Fills the static `cpuinfo` from the bus clock word at `0x800000FC` |
| `meminitadr.cpp` | 1 | 80 | One `MEMCLASS_create` call (`"RAM"`, 32-byte alignment) |
| `sfamplf.c` | 3 | 492 | Amplifier filter: process (gain loop, unrolled by the compiler), create, modify (`parameter / 256`) |
| `sflpffir8.c` | 3 | 356 | Low-pass FIR; its process filters the requested count after the input filter succeeds (the other two use the input's returned count). Modify: `2 * (p0 >> 8) / (p1 >> 8)` |
| `sfhpffir8.c` | 3 | 348 | High-pass FIR. Modify: `(p0 >> 7) / (p1 >> 8)` |
| `sfbpffir8.c` | 3 | 400 | Band-pass FIR; modify reads its parameters into locals before either store |
| `sfsrc.c` | 3 | 152 | Source filter: copies from a caller buffer and advances it |
| `sfft24.c` | 2 | 112 | Float to integer conversion clamped to +/-32767 |
| `sfsplit.c` | 3 | 384 | Splitter: alternately pulls from its input (keeping a copy) and replays the copy |
| `sfmixer.c` | 3 | 632 | Mixer: adds a second input's output into the first's |
| `sflpf.c` | 3 | 836 | One-pole low-pass: `y = y * feedback + gain * in` |
| `SNDI_sin.c` | 1 | 124 | Taylor series to x^13 after reducing below 2*pi |
| `SNDI_cos.c` | 1 | 124 | Taylor series to x^12 after reducing below 2*pi |
| `SNDI_root1x.c` | 1 | 116 | Binomial series for sqrt(1 + x); terms are separate variables summed in one expression |
| `spantoaz.c` | 1 | 20 | Pan-to-azimuth table lookup (the 128-entry table is this file's data) |
| `ssine.c` | 1 | 124 | Integer sine from a 257-entry quarter-wave table |
| `SNDI_findprime.c` | 1 | 152 | First prime at or above `a * b / 1000`; integer root by stepping `i * (i - 1)` |
| `SNDI_mult16.c` | 1 | 44 | `SNDI_findprime(a, b / 16) * 16` |
| `sexithndl.c` | 1 | 32 | Calls `SNDSYS_restore` |
| `slinkmix.c` | 1 | 88 | Installs the seven main-CPU mixer entry points |
| `idct.cpp` | 1 | 232 | Clip-table set-up of the MPEG-2 reference decoder's fast IDCT (`iclp[i]` clamped to -256..255) |
| `inittmr.cpp` | 5 | 524 | REAL timer set-up: a periodic OS alarm at `bus clock / 4 / hz` posts to the timer thread, which runs eight handler slots; the tick counters are volatile |
| `timerthread.cpp` | 4 | 352 | The timer thread's message loop and its queue, stack and thread objects |
| `exit.cpp` | 4 | 488 | REAL exit handlers: a 64-entry table run in reverse on restore; `REAL_exit` inlines `REAL_restore` |
| `ssysreal.c` | 3 | 172 | Hooks the sound system into REAL (system task, abort hook, exit handler); its local static gets a run-time guard (`init$`) |
| `initosalloc.cpp` | 4 | 380 | OS heap set-up with the Dolphin arena-rounding idiom, and MSL's `__sys_alloc`/`__sys_free` on the REAL allocator |
| `abortmsg.cpp` | 2 | 400 | `REAL_abortmessage` / `SYSTEM_abortmessage`: format into a 512-byte buffer with CodeWarrior's `__builtin_va_info`, then the abort hook or print plus `REAL_exit` |
| `memclass.cpp` | 2 | 592 | `MEMCLASS_create` lays out the LOW/free/HIGH blocks of a REAL memory class, plus `MEMCLASS_remove`; built like the sound library (`-use_lmw_stmw off`, `_savegpr_21`). `MEMBLOCK`/`MEMCLASS` members are inferred from offsets |
| `memunused.cpp` | 1 | 64 | `MEM_totalunused` (its own file record starts at 0x8014f4b8, between `memclass.cpp` and `memalloc.cpp`) |
| `meminit.cpp` | 1 | 136 | `MEM_init`: the largest OS-heap block becomes the default class; `MEM_restore` is registered as an exit handler |
| `memalloc.cpp` (fragment) | 3 | 132 | `MEM_allocalign`, `MEM_alloc` and `MEM_allocz`: forwarders to `MEM_allocaligna` (no alignment for the latter two; the final flag is false only for `MEM_allocz`) |
| `memrestore.cpp` | 1 | 372 | `MEM_restore`: clears duplicate `memclass` entries, then removes the classes and frees their OS-heap blocks |
| `debugger.cpp`, `fontdriver.cpp`, `timer.cpp` | 4 | 28 | `DEBUG_break` (empty), `FONT_installdriver`, `TIMER_gettick`/`TIMER_getfrequency` |
| `coda.cpp` | 2 | 16 | `SND::CODASetNew`/`CODASetDelete` allocation hooks |
| `memclear.cpp`, `rcmp2real.cpp` | 2 | 76 | `MEM_clear`; `RCMP::RCMP_SYSTEM::SetREALDefaults` routes RCMP allocation to `MEM_allocalign`/`MEM_free` |
| Sound one-function files: `sbhdrsze.c`, `smemhigh.cpp`, `sover.c`, `sstqmem.c`, `sstqreqi.c`, `sinitut.c`, `spoutlat.c`, `smixtmul.c`, `sstcrtap.c` | 9 | 428 | Small API wrappers; fields of the `sndgs`/`sndmix` globals are read through inferred offsets because their layouts are unknown |
| `fontchar.cpp`, `pad.cpp` | 4 | 156 | `FONT_bsearch` (character table binary search); `PAD_init`/`PAD_getdataptr`/`PAD_update` over the platform pad layer |
| `ipad.cpp` (fragment) | 1 | 236 | `iPAD_update`: copies the sampled `vgPadStatus` to `gPadStatus` with interrupts disabled, probes each port (standard controller `0x09000000` or WaveBird `0x8B100000`), marks newly connected controllers not ready and resets them, and marks other ports as having no controller; compiled with `-use_lmw_stmw off` (`_savegpr_22`). The sampling callback and `iPAD_init` (two lines off) come first and are not part of the unit |
| Sound stream and control files: `sinit16.c`, `sinitxa.c`, `sgetpvol.c`, `sstsetgl.c`, `smixfram.c`, `sstgetpv.c`, `ssthighp.c`, `sstlowp.c`, `sstpmult.c`, `sstrmdry.cpp`, `ssttmul.c`, `sstvol.c`, `sstfxlev.c`, `splysdef.c`, `srrange.c`, `sstop.c` | 16 | 1,308 | Stream setters store the value in the stream record and forward it to the voice; records and globals are read through inferred offsets |
| `fontcreate.cpp`, `fontinit.cpp` | 4 | 252 | Font creation through the current driver's hooks; the standard font is created once and destroyed at exit |
| `fontdraw.cpp` (fragment) | 2 | 68 | `FONT_drawtexta`/`FONT_getrecta`: the 8-bit text entry points forwarding to the `unsigned char` instances of the templated drawers |
| `printstr.cpp` (fragment) | 5 | 688 | REAL print channels: the static `PRINT_vstring` formats into an 8 KB buffer and passes it to each enabled device, the C and C++ variadic `PRINT_string` entry points (the C++ one on channel 2; `va_list` is MSL's `std::__tag_va_List`), and the device and channel enable switches (initialising first). `PRINT_movechannel` and `PRINT_init` are not reconstructed |
| `signals.cpp`, `mutex2.cpp`, `memmove.cpp` | 8 | 480 | REAL signals (one-slot OS message queue) and mutexes (`MUTEX_destroy` poisons the record with 0xdeadbeef); `MEM_move` copies backwards on overlap |
| `memfill.cpp` | 1 | 492 | `MEM_fill`: align with byte/halfword/word stores, `while (size >= 32)` block stores (the compiler turns it into a counted, unrolled loop), then the tail |
| `memcopy.cpp` | 1 | 1,332 | `MEM_copy`: byte, halfword or word copy by the relative alignment of source and destination (byte temporaries are `int`) |
| `initvblt.cpp` | 1 | 104 | `ttDoVTimerMsg`: vertical-blank tick and its eight subscribers |
| Sound voice and bank files: `spatkey.c`, `spktctoh.c`, `sstovrhd.c`, `smasterv.c`, `spitch.c`, `sfxlevel.c`, `sbplay.c`, `sautovol.c` | 9 | 1,300 | `iSNDpatchkey` (the per-patch voice iterator), master volume, pitch/effect/fade controls, bank playback |
| `s3dlow.c`, `slib.c`, `smixptch.c` | 3 | 604 | `SND3dpos`; `iSNDcalcvol` (four volumes out of 127, divided by 127³, then optional key-scale and curve tables); `MIX_setpitch` creates the channel's resampler on first use |
| `smixhip.c`, `sbpatinf.c`, `susercb.c`, `sclnt100.c`, `sgetdata.c`, `sststat.c` | 9 | 1,410 | High-pass stage, patch info, the user-data and 100 Hz client lists (`sndgs` viewed through an inferred struct), big-endian sample reads, stream status |
| `smixlowp.c` | 1 | 284 | `MIX_setlowpass`: RC low-pass stage created on first use (cutoff below 1.0, a fraction of the output rate, scaled `<< 7`), removed otherwise; the rate is shifted as `(unsigned short)` so the compiler masks it |
| `sresopat.c` | 1 | 152 | `SNDBANKI_asyncresolvepatch`: parse each timbre of a patch and, when its flags match the mask, resolve it asynchronously and run the user-data callback (header layout inferred) |
| `sbasync.c` (fragment) | 2 | 496 | Asynchronous bank loading, start of the file: completing (`FILESYS_opstatus`/`FILESYS_completeop`) and issuing (`FILESYS_read`, chunk-sized, clipped to the remaining size when streamed) a ring of four reads tracked by issued/completed counters in the load record behind `sndbas` (views inferred; locals in the target's declaration order). The header transfer, downloads, server and public load functions are not reconstructed |
| `saems.c` (fragment) | 18 | 1968 | AEMS module updaters from the multiplexer to the event sender (`AEMSI_updatemux` through `SNDAEMSI_updatesend`): integer arithmetic over a component's inputs with clamping, a 64-bit scaled product and a zero-divisor guard, short/word/byte table lookups, a delay trigger and a sample delay line timed by `sndaems`, maximum, minimum and edge-triggered state values, instance destruction, and an event (definition id plus inputs) sent to `SNDAEMS_beginevent` and each registered handler when the inputs change. The component, its per-type static definitions, `sndaems` and the event buffer size are inferred. The ramp updater after them (64-bit fixed point) is not part of the unit |
| `smixer.c` (fragment) | 2 | 64 | `SNDI_New`/`SNDI_Delete`: mixer allocation through cleared sound memory (`SNDMEMI_allocz`) and `SNDMEMI_free` |
| `snddrv.c` (fragment) | 4 | 148 | Sound-driver mutex wrappers at the end of the file: `SNDI_mutexalloc`/`lock`/`unlock` call `OSInitMutex`/`OSLockMutex`/`OSUnlockMutex` on the mutex inside `snddrv` (opaque view, only the mutex offset known); `SNDI_mutexfree` is empty |
| `pathxSND.c` (fragment) | 2 | 240 | The last two functions: `PATHX_isynctask` runs the installed `pathService` callback (its parameters are not established), and `PATHX_ifade` stores a track's volume and fades it as a sound (`SNDautovol`, time in tenths) or a stream (`SNDSTRM_autovol`) inside a critical section (track view inferred). The PATHX API from `PATHX_stop` to `PATHX_milliseconds` is not reconstructed |
| `slinklst.c`, `ssysserv.c`, `sgettag.c`, `sstrstat.c` | 10 | 1,316 | Doubly linked lists, sound server clients and `SNDSYS_service`, the header tag reader, request status (64-bit helpers return `SINT64` in r3/r4) |
| `sfilter.c`, `supf.c` | 6 | 720 | Mixer filter chains (insert by priority, remove, connect ports) and the 16-bit PCM unpacker |
| `sbremove.c` | 1 | 360 | `SNDbankremove`: stop the bank's voices, run user-data callbacks per timbre, free sample memory (recursive for -1) |
| `seffect.c` | 3 | 568 | Effect bus lookup (`switch` on the bus flags), bus set-up and master send levels |
| `sbadd.c` | 2 | 576 | `SNDbankadd` (download samples in 4 KB pieces, resolve patches) and the per-timbre user-data callback |
| `smixc.c`, `sfxrev.c` | 2 | 636 | `mixc` (scaled add into a mix buffer) and `MIXI_reverbblock` (comb filter with a one-pole low-pass in the feedback, 1e-30 anti-denormal offset) |
| `sdfx.c` | 3 | 732 | Platform effects: reverb select/restore (inlined into `SNDPLATFORM_fxinit`) and per-voice send levels as DSP aux-bus levels or software wet gain (scales 1/127 and 1/(127·32767)) |
| `sx87d16.c` | 1 | 392 | `decode16x87`: 16-bit PCM to float |
| `supmutf.c`, `supmutpf.c`, `suppf.c` | 7 | 1,424 | MicroTalk unpackers (in memory and through the packet player, 432-sample blocks) and the packet-player 16-bit PCM unpacker |
| `spktplay.c` (fragment) | 1 | 268 | `SNDPKTPLAY_create` (slot search with an early exit, player record in caller memory) |
| `salloc.c` (fragment) | 1 | 96 | `SNDVOICEI_get`: handle to voice index with ownership check |
| `aramalloc.cpp` (fragment) | 1 | 116 | `ARAM_NEW_poolmanager`; `ARAM_NEW_pool` differs only in register numbering and the allocator itself is not reconstructed |
| `suplf.c` | 3 | 276 | Looping 16-bit PCM unpacker (the converter's base is taken into a local before the loop) |
| `saramman.c` (fragment) | 3 | 256 | ARAM manager set-up, pool bounds in 32-byte units (the end is masked with `~4` exactly as compiled) and teardown; `SNDARAM_alloc`/`SNDARAM_free` are not reconstructed |
| More sound files: `sbvalid.c`, `sst3dpos.c`, `ssthold.c`, `sctlfilt.c`, `shipass.c`, `slowpass.c`, `sstgetrp.c`, `sstopall.c`, `sstautov.c`, `sattrdef.c`, `scalcfx.c`, `sballoc.c`, `sbhdrcpy.c`, `sctrldry.cpp`, `stimemul.c` | 17 | 1,768 | Bank slots, voice controls applied to each platform voice (`iSNDpatchkey` loop), stream setters and defaults; record members are inferred views |
| `threads.cpp` (fragment) | 2 | 76 | `THREAD_init` (records the main thread) and the alarm handler that sets the signal stored after the alarm. `THREAD_yield` is 10 lines off (the 64-bit millisecond-to-tick product is computed in scratch registers and then moved) and `THREAD_iscurrent` follows it, so both are left out (draft in `scratch/lib/threads_full_wip.cpp`) |
| `srandom.c` (fragment) | 1 | 92 | `SNDI_randomseed` with the generator's state (`SNDIrandseedorig` in `.data`, `SNDIrandseed` in `.bss`). `iSNDrandom` (the add-with-carry step) is not matched: the target recomputes the array base per stage and branches on each carry, as if through an inline helper (draft in `scratch/lib/srandom_wip.c`) |
| `supxaf.cpp` (fragment) | 3 | 212 | EA-XA unpacker set-up through `SND::CEAXABLKDecf` (a 168-byte decoder with class `new`/`delete`, `Feed`), the frame getter and the restore hook. The decode step `SFILTER_unpackxaf` comes first and is 16 lines off (register allocation; draft in `scratch/lib/supxaf_wip.cpp`) |
| `supxalf.cpp` (fragment) | 3 | 216 | Looping EA-XA unpacker set-up (loop bounds from the init parameters, no initial `Feed`), frame getter and restore; the looping decode step before them is not reconstructed |
| `supxapf.cpp` (fragment) | 2 | 200 | Packet-player EA-XA unpacker set-up (packet handle and sample channel of the voice's master voice) and restore; the decode step before them is not reconstructed |
| `eaxadecf.cpp` (fragment) | 4 | 168 | `SND::CEAXABLKDecf` allocation through the CODA hooks, its constructor and `Feed` (refused while a block is pending); `decodexac`, `Decode`, `GetState` and `SetState` are not reconstructed |
| `ssysinit.c` (fragment) | 2 | 380 | `SNDSYS_restore` (module shutdown hooks in `sndgs`, stop all, platform restore, free the voice and bank tables, memory restore, clear the initialised flag) and `SNDSYS_inited` (that flag read as a signed byte). `SNDSYS_getopts`, `SNDSYS_setopts` and `SNDSYSI_init` come first and are not reconstructed |
| `sst.c` (fragment) | 3 | 204 | `SNDSTRMI_destroyall`, `SNDSTRMI_getstreamptr` (range check against the stream count in `sndgs`) and `SNDSTRMI_releasecallback`. `SNDSTRMI_calcdatarate` comes first and differs only in register numbering (draft in `scratch/lib/sst_wip.c`); the stream service, parsing and API functions after them are not reconstructed |
| `filedev.cpp` (fragment) | 5 | 368 | `readfile` (fills the read request and wakes the read thread), `writefile`/`getfilesize` through the MSL FILE functions for host files (handle type 3) and the cached size for disc files (type 2), and the `stopreadfile`/`deletefile` stubs. The request record `gCurRead` is file-local and a local external of this fragment; the device set-up, open/close and DVD read path before them are not reconstructed |
| `sserver.c` (fragment) | 2 | 104 | `SNDSYS_entercritical`/`SNDSYS_leavecritical`: the sound mutex with a nesting count byte in `sndgs`. The 100 Hz server before them and `SNDSYS_add100hzclient` after them are not reconstructed |
| `sdspmix.c` (fragment) | 2 | 196 | `SNDDRV_DSPMixerSetPan`/`SNDDRV_DSPMixerSetAuxBus` on the 64 mixer channels (`__MIXChannel`, 80 bytes each, with `numoutputchannels`). Compiled with GC/1.3: 1.3.2 builds the `0x40000000` mode flag with `oris` instead of `lis`/`or`. `SNDDRV_DSPMixerInit` is drafted (its channel loop is unrolled twice instead of four times; `scratch/lib/sdspmix_wip.c`); the rest is not reconstructed |
| `memblock.cpp` | 1 | 44 | `MEM_initblock` writes a block header ('BM', flags, size, neighbour links); the name and tail-size arguments are not stored |
| `memlist.cpp` | 5 | 464 | Address-ordered circular free list (`FREE_find`, `FREE_findlargest`, `FREE_gettotalfree`, `FREE_add`, `FREE_remove`) with the class's 'BS' sentinel; `FREE_findlargest` needs the operand order of `0 > size - 1 ? 0 : size - 1` for its branchless max |
| `memstd.cpp` | 1 | 8 | `MEM_size` |
| `systask.cpp` | 4 | 772 | `SYNCTASK_*`: a 16-entry table of tick-scheduled callbacks with guarded local statics; entry layout inferred |
| `bmem.cpp` | 4 | 680 | Block pools (`BPoolMan`): pool chain and circular free list; the static `AddNewPool` is defined last so it is not inlined. Members inferred |
| `syncfile.cpp` | 9 | 1,100 | Synchronous FILESYS open/read/close/size/addbig/delbig/exists: chunked (0x8000) block IO driven by a completion callback; the context's polled fields are volatile, and `#pragma dont_inline` keeps `syncblockio` out of `FILESYS_readsync` as in the original |
| `hlsfile.cpp` | 10 | 1,384 | High-level file services (`FILE_exists`, `FILE_size(z)`, `FILE_loadz`, `FILE_loadsize`, `FILE_loadbigheader`) run as atomic FILESYS operations; buffers come from `gFileSysOpts` |
| `locatbig.cpp` (fragment) | 3 | 504 | BIG archive header type (0xC0FB, "BIGF", "BIG\0"), size and the (static) debug-tag reader; the entry lookup that follows (`BIG_locateentryz`, ...) is not reconstructed |

