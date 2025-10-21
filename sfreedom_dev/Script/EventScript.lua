------------------------------------------------------------------------------------------------
--	Common Functions
------------------------------------------------------------------------------------------------

function EventScript__Common__GetActor(kCmdObj)

	-- "ActorGUID" -> "NameTTID" -> "ClassNo" -> "PartyIndex" -> "SpeicalActor"

	--	"ACTOR_GUID" 가 있다면 GUID로 찾는다.
	local	kGUIDString = kCmdObj:GetAttr("ActorGUID");
	if kGUIDString ~= "" then
		
		local	kGUID = GUID(kGUIDString);
		
		local	kPilot = g_pilotMan:FindPilot(kGUID);
		
		if kPilot:IsNil() then
			return	nil;
		end
		
		return	kPilot:GetActor();
		
	end
	
	-- "NameTTID" 이 있다면
	local	kNameTTID = kCmdObj:GetAttr("NameTTID");
	if kNameTTID ~= "" then
	
		local	kName = GetTT(kNameTTID);
	
		local	kPilot = g_pilotMan:FindPilotByName(kName,true);
		
		if kPilot:IsNil() then
			return	nil;
		end
		
		return	kPilot:GetActor();	
	
	end
	
	
	-- "ClassNo" 가 있다면
	local	kClassNo = kCmdObj:GetAttrInt("ClassNo");
	if kClassNo ~= nil and kClassNo ~= 0 then
		return g_pilotMan:FindActorByClassNo(kClassNo);
	end
	
	-- "PARTY_INDEX" 가 있다면 파티인덱스로 찾는다.
	local	kPartyIndex = kCmdObj:GetAttrInt("PartyIndex");
	if kPartyIndex ~= "" then
	
	end
	
	-- "SPECIAL_ACTOR" 이 있다면
	local	kSpecialActor = kCmdObj:GetAttr("SpecialActor");
	if kSpecialActor ~= "" then
	
		if kSpecialActor == "ME" then	--	나 = "ME"
			return	GetMyActor()
		elseif kSpecialActor == "PARTY_MASTER" then	--	파티장 = "PARTY_MASTER"
		
		end
	
	end

	-- "Function" 이 있다면
	local kFunActor = kCmdObj:GetAttr("Func")
	if kFunActor ~= "" then
		local kGUID = nil
		local func = loadstring("return " ..kFunActor)
		if nil~= func then
			kGUID = func()
		end

		if nil~=kGUID and false==kGUID:IsNil() then
			local kPilot = g_pilotMan:FindPilot(kGUID)
			if false==kPilot:IsNil() then
				return	kPilot:GetActor()
			end
		end
	end

	return	nil;	--	액터를 찾을 수 없다;	
end

function EventScript__Common__WaitAction(kCmdObj,kActionName)
	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil()  then
		return	false
	end
	
	local	kAction = kActor:GetAction()
	if kAction:IsNil() then
		return	false
	end
	
	if kAction:GetID() == kActionName then
		return	false
	end
	
	kAction = kActor:GetReservedTransitAction()
	if kAction:GetID() == kActionName then
		return	false
	end	
	
	return	true;
end

function	EventScript__Common__MoveEventCamToTarget(kCmdObj)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEventCam = g_world:GetCameraModeEvent()
	if kEventCam:IsNil() then
		return;
	end

	local	fMoveTime = kCmdObj:GetAttrFloat("MoveTime");
	local	fCamDistance = kCmdObj:GetAttrFloat("CamActorDistance");
	local	fCamMoveUp = kCmdObj:GetAttrFloat("CamMoveUpDistance");
	local	fHorizRotAngle = kCmdObj:GetAttrFloat("HorizRotAngle");
	local	fVertRotAngle = kCmdObj:GetAttrFloat("VertRotAngle");
	
	if kCmdObj:GetAttr("CamActorDistance") == "" then
		fCamDistance = math.sqrt(GetCameraTrn():GetY()*GetCameraTrn():GetY()+GetCameraTrn():GetZ()*GetCameraTrn():GetZ());
	end
	
	if kCmdObj:GetAttr("CamMoveUpDistance") == "" then
		fCamMoveUp = GetCameraRot():GetZ();
	end
	
	if kCmdObj:GetAttr("HorizRotAngle") == "" then
		fHorizRotAngle = 0;
	end
	
	if kCmdObj:GetAttr("VertRotAngle") == "" then
		fVertRotAngle = math.atan(GetCameraTrn():GetZ()/GetCameraTrn():GetY())*180/3.141592;
	end
	
	kEventCam:StartMoveCamera(fMoveTime,fCamDistance,fCamMoveUp,fHorizRotAngle,fVertRotAngle);
	
end

------------------------------------------------------------------------------------------------
--	EventScript Functions
------------------------------------------------------------------------------------------------

function EventScript_BALLOONDIALOGUE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kText = kCmdObj:GetAttr("Text");
	if kText ~= "" then
	
		kActor:ShowChatBalloon(kText);
		return	true;
	
	end
	
	local	iTTWID = kCmdObj:GetAttrInt("TextTableID");
	kActor:Talk(iTTWID);
	
	return	true;

end

function EventScript_WAITTIME_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	fStartTime = kCmdObj:GetParamFloat("START_TIME")
	if fStartTime == 0 then
		fStartTime = fAccumTime;
		kCmdObj:SetParamFloat("START_TIME",fStartTime);
	end
	
	local	fElapsedTime = fAccumTime - fStartTime;
	local	fTotalTime = kCmdObj:GetAttrFloat("Time");
	
	return	fElapsedTime>=fTotalTime;

end

function EventScript_LOCKPLAYERINPUT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	bLock = kCmdObj:GetAttrBool("Lock");
	local iLockType = kCmdObj:GetAttrInt("LockType");
	if nil == iLockType or 0 == iLockType then
		iLockType = 1 -- for eventscript lock
	end

	BlockGlobalHotKey(bLock)
	if bLock then
		--	Lock input
		LockPlayerInput(iLockType)
	else
		--	Unlock input
		UnLockPlayerInput(iLockType)
	end

	return	true;

end

function EventScript_LOCKPLAYERINPUTMOVE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	bLock = kCmdObj:GetAttrBool("Lock");
	local iLockType = kCmdObj:GetAttrInt("LockType");
	if nil == iLockType or 0 == iLockType then
		iLockType = 1 -- for eventscript lock
	end

	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		if bLock then
			-- Lock action input
			LockPlayerInputMove(iLockType)
		else
			-- Unlock action input
			UnLockPlayerInputMove(iLockType)
		end
	end

	return	true;
end

function EventScript_BALLOON_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	bShow = kCmdObj:GetAttrBool("Show");
	g_pilotMan:SetHideBalloon(false==bShow);

	return	true;
end

function EventScript_SHOWACTOR_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	local	bShow = kCmdObj:GetAttrBool("Show");
	local	bSave = kCmdObj:GetAttr("Save");
	local	eType = kCmdObj:GetAttr("UnitType")

	if true==bShow or (nil == bSave or "" == bSave or "true" == bSave) then
		bSave = true
	else
		bSave = false
	end
	
	if nil ~= eType then
		if eType == "UT_PLAYER" then
			eType = UT_PLAYER
		elseif eType == "UT_SUMMONED" then
			eType = UT_SUMMONED
		elseif eType == "UT_MONSTER" then
			eType = UT_MONSTER
		elseif eType == "UT_BOSSMONSTER" then
			eType = UT_BOSSMONSTER
		elseif eType == "UT_GROUNDBOX" then
			eType = UT_GROUNDBOX
		elseif eType == "UT_NPC" then
			eType = UT_NPC
		elseif eType == "UT_OBJECT" then
			eType = UT_OBJECT
		else
			eType = 0
		end
	else
		eType = 0
	end
	
	local	kClassNo = kCmdObj:GetAttrInt("ClassNo");
	if bSave then
		if nil ~= kClassNo and 0 ~= kClassNo then
			g_world:SaveHideActorClassNo(kClassNo, bShow==false)
		end
		if 0 ~= eType then
			g_world:SaveHideActorType(eType, bShow==false)
		end
	end

	if kActor == nil or kActor:IsNil() then
		if 0 ~= eType then
			ShowActorUnitType(eType, bShow, true)
		end
		return	true
	end
	
	kActor:SetHide(bShow==false);
	kActor:SetHideShadow(bShow==false);
	kActor:SetAbil(AT_EVENTSCRIPT_HIDE, false==bShow);

	return	true;

end

function EventScript_SETACTORPOSITION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kTargetPos = kCmdObj:GetAttrPoint3("TargetPos");
	
	kActor:SetTranslate(kTargetPos);
		
	return	true;

end
function EventScript_ATTACHEFFECT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kEffectID = kCmdObj:GetAttr("EffectID");
	local	kDummy = kCmdObj:GetAttr("DummyName");
	local	iSlotNum = kCmdObj:GetAttrInt("SlotNumber");
	
	kActor:AttachParticle(iSlotNum,kDummy,kEffectID);
		
	return	true;
end
function EventScript_DETACHEFFECT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	iSlotNum = kCmdObj:GetAttrInt("SlotNumber");
	kActor:DetachFrom(iSlotNum);
		
	return	true;
end
function EventScript_PLAYSOUND_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end

	local	kSoundID = kCmdObj:GetAttr("SoundID");
	local	iSlotNum = kCmdObj:GetAttrInt("SlotNumber");
	
	kActor:AttachSound(iSlotNum,kSoundID);
		
	return	true;
end
function EventScript_STOPSOUND_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end

	local	iSlotNum = kCmdObj:GetAttrInt("SlotNumber");
	kActor:DetachFrom(iSlotNum);
		
	return	true;
end
function EventScript_PLAYBGM_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iSoundNo = kCmdObj:GetAttr("SoundNo");
	PlayBgSound(iSoundNo );
		
	return	true;
end
function EventScript_STOPBGM_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iSoundNo = kCmdObj:GetAttr("SoundNo");		
	StopBgSound(iSoundNo);
	
	return	true;
end
function EventScript_PLAYACTION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kActionName = kCmdObj:GetAttr("ActionName");
	
	local	kAction = kActor:ReserveTransitActionIgnoreCase(kActionName);
	kAction:SetDoNotBroadCast(true);

	for i = 1, 10 do
		local param = kCmdObj:GetAttr("IPARAM_"..i)
		if nil == param or "" == param then
			break
		end
		kAction:SetParamInt(param, kCmdObj:GetAttr("IVALUE_"..i))
	end
		
	return	true;
end

function EventScript_PLAYANIMATION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kSlotName = kCmdObj:GetAttr("SlotName");
	local	bKeepAnimation = kCmdObj:GetAttrBool("KeepAnimation");
	
	local	kAction = kActor:ReserveTransitAction("a_play_animation");
	kAction:SetParam(1,kSlotName);
	
	if bKeepAnimation then
		kAction:SetParamInt(2,1)
	end
		
	return	true;
end

function EventScript_SETACTORDIRECTION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kDirection = kCmdObj:GetAttrInt("Direction");
	
	kActor:SetLookingDirection(kDirection,true);
		
	return	true;
end

function EventScript_WALK_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kTargetPos = kCmdObj:GetAttrPoint3("TargetPos");
	local	iSpeed = kCmdObj:GetAttrInt("Speed");
	local	bBackMove = kCmdObj:GetAttrBool("BackMove");
	
	local	kAction = kActor:ReserveTransitAction("a_run");
	kAction:SetParamAsPoint(0,kTargetPos);
	kAction:SetDoNotBroadCast(true);
	kAction:SetParamInt(4,iSpeed);
	
	if bBackMove then
		kAction:SetParamInt(5,1)
	end
		
	return	true;
	
end

function EventScript_WALKWAYPOINT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
		
	return	true;
	
end

function EventScript_WAITWALK_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	return	EventScript__Common__WaitAction(kCmdObj,"a_run");

end
function EventScript_ACTIVATEEVENT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iEventID = kCmdObj:GetAttrInt("EventID");
	
	GetEventScriptSystem():ActivateEvent(iEventID);
		
	return	true;
	
end

function EventScript_DEACTIVATEEVENT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iEventID = kCmdObj:GetAttrInt("EventID");
	
	GetEventScriptSystem():DeactivateEvent(iEventID);
		
	return	true;
	
end

function EventScript_FADE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	fFadeTime = kCmdObj:GetAttrFloat("FadeTime");
	local	bIsFadeIn = kCmdObj:GetAttrBool("FadeIn");
	local	kColor = kCmdObj:GetAttrColor("FadeColor");
	
	if bIsFadeIn then
		g_world:SetShowWorldFocusFilterColorAlpha(kColor,1,0,fFadeTime,true,false);
	else
		g_world:SetShowWorldFocusFilterColorAlpha(kColor,0,1,fFadeTime,true,false);
	end
		
	return	true;
	
end

function EventScript_WAITFADE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end 
	
	return	g_world:IsEndShowWorldFocusFilter();
	
end

function EventScript_WAITANIMATION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	local	kActionName = "a_play_animation";
	
	local	kAction = kActor:GetAction()
	if kAction:IsNil() or kActor:IsNil() then
		return	true
	end
	
	if kAction:GetID() == kActionName then
		return	kAction:GetParam(3) == "end";
	end
	
	kAction = kActor:GetReservedTransitAction()
	if kAction:GetID() == kActionName then
		return	false
	end	
	
	return	true;
	
end

function EventScript_SETMOVIESCENE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	bEnable = kCmdObj:GetAttrBool("Enable");
	local	fTime = kCmdObj:GetAttrFloat("Time")
	
	UI_CallMovieScene(bEnable,fTime);
	
	return	true;

end

function EventScript_CLOSEMOVIESCENE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local	fTime = kCmdObj:GetAttrFloat("Time")	

	UI_CloseMovieScene(fTime)

	return	true
end

function EventScript_ADDMONSTER_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iClassNo = kCmdObj:GetAttrInt("ClassNo")
	local	kSpawnPos = kCmdObj:GetAttrPoint3("SpawnPos");
	local	kNameTTID = kCmdObj:GetAttr("NameTTID");
	local kActionName = kCmdObj:GetAttr("Action")
	
	local	kName = GetTT(kNameTTID);
	
	--	같은 이름의 액터가 이미 있는지 체크하자.
	if g_pilotMan:FindPilotByName(kName,true):IsNil() == false then
		MessageBox("An actor having same name["..kNameTTID.."] already exist.","EventScriptCmmand AddMonster Failed");
		return true
	end
	
	local guid = GUID("123")
	guid:Generate();

	local pilot = g_pilotMan:NewPilot(guid, iClassNo, 0, "MONSTER")
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	kActor:SetUseSmoothShow(false)	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 2)
	
	pilot:SetName(kName);
	if nil == kActionName or "" == kActionName then
		pilot:GetActor():ReserveTransitAction("a_opening")
	else
		pilot:GetActor():ReserveTransitActionIgnoreCase(kActionName)
	end
	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	
	return	true;

end

function EventScript_ADDNPC_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	if kCmdObj:GetParamInt("__skip") > 0 then
		kCmdObj:SetParamInt("__skip", 0)
		return true
	end
	
	local	iClassNo = kCmdObj:GetAttrInt("ClassNo")
	local	kSpawnPos = kCmdObj:GetAttrPoint3("SpawnPos");
	local	kNameTTID = kCmdObj:GetAttr("NameTTID");
	local kActionName = kCmdObj:GetAttr("Action")
	
	local	kName = GetTT(kNameTTID);
	
	--	같은 이름의 액터가 이미 있는지 체크하자.
	if g_pilotMan:FindPilotByName(kName,true):IsNil() == false then
		MessageBox("An actor having same name["..kNameTTID.."] already exist.","EventScriptCmmand AddNPC Failed");
		return true
	end
		
	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, iClassNo, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	pilot:SetName(kName);
	if nil == kActionName or "" == kActionName then
		pilot:GetActor():ReserveTransitAction("a_idle")
	else
		pilot:GetActor():ReserveTransitActionIgnoreCase(kActionName)
	end
	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	return	true;

end

function EventScript_ADDPLAYER_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iClassNo = kCmdObj:GetAttrInt("ClassNo")
	local	iSex = kCmdObj:GetAttrInt("Sex")
	local	kSpawnPos = kCmdObj:GetAttrPoint3("SpawnPos");
	local	kNameTTID = kCmdObj:GetAttr("NameTTID");
	local kActionName = kCmdObj:GetAttr("Action")
	
	local	kName = GetTT(kNameTTID);
	
	--	같은 이름의 액터가 이미 있는지 체크하자.
	if g_pilotMan:FindPilotByName(kName,true):IsNil() == false then
		MessageBox("An actor having same name["..kNameTTID.."] already exist.","EventScriptCmmand AddPlayer Failed");
		return true
	end
		
	local guid = GUID("123")
	guid:Generate();

	local pilot = g_pilotMan:NewPilot(guid, iClassNo, iSex)
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end
	
	kActor:SetUseSmoothShow(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 1)
	
	pilot:SetName(kName);
	if nil == kActionName or "" == kActionName then
		pilot:GetActor():ReserveTransitAction("a_idle")
	else
		pilot:GetActor():ReserveTransitActionIgnoreCase(kActionName)
	end
	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	return	true;

end

function EventScript_REMOVEACTOR_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	
	return	true;

end

function EventScript_SLOWMOTION_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local Type = kCmdObj:GetAttrInt("Type");
	local From = kCmdObj:GetAttrFloat("From");
	local To = kCmdObj:GetAttrFloat("To");
	local Time = kCmdObj:GetAttrFloat("Time");
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	g_world:SetSlowMotion(Type, From, To, Time)

	return true;
end

function EventScript_DIALOGUE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	iNameTTID = kCmdObj:GetAttrInt("NameTTID");
	local	iTitleTTID = kCmdObj:GetAttrInt("TitleTTID");
	local	iTextTTID = kCmdObj:GetAttrInt("TextTTID");
	local	kFaceID = kCmdObj:GetAttr("FaceID");
	local kStitchImageID = kCmdObj:GetAttr("StitchImageID");
	
	if kFaceID == "" then
		kFaceID = "Empty"
	end
	
	local	kName = GetTT(iNameTTID);
	local	kTitle = GetTT(iTitleTTID);
	local	kText = GetTT(iTextTTID);
	
	g_kCanCancelQuestDialog = false;
	GetQuestMan():CallEventFullScreenTalk(kName,kTitle,kText,WideString(kFaceID),kStitchImageID);
	
	return	true;

end

function EventScript_WAITDIALOGUE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local bEventScriptTalk = GetQuestMan():IsEventScriptDialog() == false 
	local bQuestTalk = GetQuestMan():IsFullQuestDialog() == false
	return bEventScriptTalk or bQuestTalk
end

function EventScript_CLOSE_DAILOGUE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	CloseUI("FRM_QUEST_FULLSCREEN")
	return true;
end

function EventScript_SETCAMERAMODEFOLLOW_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	g_world:SetCameraMode(1,kActor);
	return	true;	

end

function EventScript_SETCAMERAMODEEVENT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(9,GetMyActor());
	return	true;	

end


function EventScript_MOVECAMTOACTOR_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEventCam = g_world:GetCameraModeEvent()
	if kEventCam:IsNil() then
		return	true
	end
	
	kEventCam:SetTargetActor(kActor:GetPilotGuid());
	
	EventScript__Common__MoveEventCamToTarget(kCmdObj);
	
	return	true;	

end
function EventScript_MOVECAMTOPOS_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEventCam = g_world:GetCameraModeEvent()
	if kEventCam:IsNil() then
		return	true
	end

	local	kTargetPos = kCmdObj:GetAttrPoint3("TargetPos")
	kEventCam:SetTargetPos(kTargetPos);
	
	EventScript__Common__MoveEventCamToTarget(kCmdObj);
	
	return	true;	

end
function EventScript_MOVECAMTOTRIGGER_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEventCam = g_world:GetCameraModeEvent()
	if kEventCam:IsNil() then
		return	true
	end

	local	kTarget = kCmdObj:GetAttr("Target")
	local 	kTargetPos = g_world:FindTriggerLoc(kTarget)
	kEventCam:SetTargetPos(kTargetPos);

	EventScript__Common__MoveEventCamToTarget(kCmdObj);

	return	true;

end
function EventScript_WAITCAMERA_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	kEventCam = g_world:GetCameraModeEvent()
	if kEventCam:IsNil() then
		return	true
	end

	return	kEventCam:GetState() == 2;

end

function EventScript_ENABLETRIGGER_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local	bEnable = kCmdObj:GetAttrBool("enable");
	local	kTriggerID = kCmdObj:GetAttr("triggerid");
	
	local	kTrigger = g_world:GetTriggerByID(kTriggerID)

	if kTrigger:IsNil() == false then
		kTrigger:SetEnable(bEnable);
	end

	return	true;
end

function EventScript_ADDHELPER_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	CreateHelper()
	return true;
end

function EventScript_DELHELPER_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	RemoveHelper()
	return true;
end

function EventScript_SETFOLLOWCAMERA_OnUpdate(kCmdObj, fAccumTime,fFrameTime)

	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		SetBreak();
		return	true
	end
	
	local kCamaeraID = kCmdObj:GetAttr("CameraID");
	local bUseActorPositionAsSoundListener = kCmdObj:GetAttrBool("UseActorPositionAsSoundListener");
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	g_world:SetCameraModeByActorInObjectName(kActor,kCamaeraID, true, fAccumTime, bUseActorPositionAsSoundListener);

	return true;
end

function EventScript_RESETFOLLOWCAMERA_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(1, kActor);

	return true;
end

function EventScript_CAMERAADJUST_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local width = kCmdObj:GetAttrInt("WIDTH");
	local zoom = kCmdObj:GetAttrInt("ZOOM");
	local up = kCmdObj:GetAttrInt("UP");
	local target_x = kCmdObj:GetAttrInt("TARGET_X");
	local target_y = kCmdObj:GetAttrInt("TARGET_Y");
	local target_z = kCmdObj:GetAttrInt("TARGET_Z");
	local group = kCmdObj:GetAttrInt("GROUP");
				
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraAdjustCameraInfo(Point3(width,zoom,up), Point3(target_x,target_y,target_z));
	return true;
end

function EventScript_SAVECAMERAPOS_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SaveCameraPos()
	return true;
end

function EventScript_RESTORECAMERAPOS_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RestoreCameraPos()
	return true;
end

function EventScript_MAPMOVE_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local iMapID = kCmdObj:GetAttrInt("MapID");
	local iSpawnIndex = kCmdObj:GetAttrInt("SpawnIndex");

	if iSpawnIndex == nil then
		iSpawnIndex = 1;
	end

	if iMapID ~= nil  then
--		Net_ReqMapMove(iMapID, iSpawnIndex);
		Net_RecentMapMove()
	end

	return true;
end

function EventScript_MAPMOVEONLYINSTANCE_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if nil==g_world then
		return true
	end
	
	if true == g_world:IsHaveWorldAttr(GATTR_INSTANCE) then
		return EventScript_MAPMOVE_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	end
	return true
end

function EventScript_CALLUI_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local kUIWndName = kCmdObj:GetAttr("UIWndName"); 
	local iIsModal = kCmdObj:GetAttrInt("IsModal"); 

	if iIsModal == nil or iIsModal == 0 then
		iIsModal = false;
	else
		iIsModal = true;
	end

	if kUIWndName ~= nil then
		CallUI(kUIWndName, iIsModal);
	end

	return true;
end

function EventScript_WAITSHOWMAPTHUMBNAIL_OnUpdate()
	return GetUIWnd("FRM_CONSTELLATION_MINIMAP_THUMBNAIL"):IsNil();
end

g_iEscEventScriptID = 0
function OnEventScriptEscScript()
	ClearESCScript()
	ODS("OnEventScriptEscScript\n")
	if false==GetEventScriptSystem():IsNowActivate() then
		return
	end
	
	CloseUI("FRM_QUEST_FULLSCREEN")
	GetEventScriptSystem():DeactivateAll()
	if 0 ~= g_iEscEventScriptID then
		GetEventScriptSystem():ActivateEvent(g_iEscEventScriptID)
		ClearEventScriptEscScript()
	end
end

function ClearEventScriptEscScript()
	if 0~=g_iEscEventScriptID then
		g_iEscEventScriptID = 0
		ClearESCScript()
	end
end

function ActivateEventScript(iEventID)
end

function DeActivateEventScript(iEventID)
	ClearEventScriptEscScript()
end

function EventScript_ESCSCRIPT_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local iEvnetID = kCmdObj:GetAttrInt("EventID"); 
	
	if 0==iEvnetID then
		ClearESCScript()
		return true
	end

	g_iEscEventScriptID = iEvnetID
	RegistESCScript("OnEventScriptEscScript")
	return true
end

function EventScript_CLEARESCSCRIPT_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	ClearEventScriptEscScript()
	return true
end

g_EventScriptTextDefault = {}
g_EventScriptTextDefault["W"] = 500
g_EventScriptTextDefault["SPACE"] = 15 --여유공간
function EventScriptText(kUIWndName, iTextID, fAliveTime, iAlignX, iAlignY, iWidth, iSpace)
	if kUIWndName == nil then
		return true
	end
	
	if fAliveTime == nil then
		fAliveTime = 0
	end

	local kWnd = CallUI(kUIWndName);
	if kWnd:IsNil() then
		return true
	end
	
	local kText = kWnd:GetControl("FRM_TEXT")
	if kText:IsNil() then
		return true
	end
	
	if iWidth == nil or iWidth<1 then
		iWidth = g_EventScriptTextDefault["W"]
	end
	
	local kSize = kText:GetSize()
	kSize:SetX(iWidth)
	kText:SetSize(kSize)
	kText:SetStaticTextW(GetTT(iTextID))
	iWidth = kText:GetTextSize():GetX() + g_EventScriptTextDefault["SPACE"]

	if iSpace == nil or iSpace<1 then
		iSpace = g_EventScriptTextDefault["SPACE"]
	end
	
	--
	local kTextSize = kText:GetSize()
	kTextSize:SetX(iWidth)
	kText:SetSize(kTextSize)

	local kTextPos = kText:GetTextPos()
	kTextPos:SetX(iWidth/2)
	kText:SetTextPos(kTextPos)
	SetUISizeForTextY(kText)
	
	local kWndSize = kText:GetSize()
	kWndSize:SetY(kWndSize:GetY()+kTextPos:GetY()+iSpace)
	kWnd:SetSize(kWndSize)	
	
	kWnd:SetAliveTime(fAliveTime*1000)
	if iAlignX then kWnd:SetAlignX(iAlignX) end
	if iAlignY then kWnd:SetAlignY(iAlignY) end
	kWnd:VAlign()
	kWnd:SetInvalidate(true)
	return true
end

function EventScript_TEXT_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local kUIWndName = kCmdObj:GetAttr("UIWndName"); 
	local iTextID = kCmdObj:GetAttrInt("TextTTID"); 
	local fAliveTime = kCmdObj:GetAttrFloat("WaitTime"); 
	local iAlignX = kCmdObj:GetAttrInt("AlignX"); 
	local iAlignY = kCmdObj:GetAttrInt("AlignY"); 
	local iWidth = kCmdObj:GetAttrInt("Width");
	local iSpace = kCmdObj:GetAttrInt("Space");
	
	return EventScriptText(kUIWndName, iTextID, fAliveTime, iAlignX, iAlignY, iWidth, iSpace)
end

function EventScript_CLOSEUI_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local kUIWndName = kCmdObj:GetAttr("UIWndName");

	if kUIWndName ~= nil then
		CloseUI(kUIWndName);
	end

	return true;
end

function EventScript_ADDACTORFILTER_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	ODS("-------------------------------------------------------------------------\n")
	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	g_world:AddDrawActorFilter(kActor:GetPilotGuid());

	return true;
end

function EventScript_CLEARACTORFILTER_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ClearDrawActorFilter()
	return true;
end

function EventScript_REGISTHELPER_OnUpdate(kCmdObj, fAccumTime,fFrameTime)
	local	kActor = EventScript__Common__GetActor(kCmdObj)
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	RegistHelperGuid( kActor:GetPilotGuid() )
	
	return true
end

function EventScript_SYSTEMMESSAGE_OnUpdate(kCmdObj, fAccumTime, fFrameTime)
	local kText = kCmdObj:GetAttr("TTID");

	if kText ~= nil then
		AddWarnDataStr(GetTextW(kText), 0)
	end

	return true;	
end

function EventScript_DOSTRING_OnUpdate(kCmdObj, fAccumTime, fFrameTime)
	local kText = kCmdObj:GetAttr("COMMAND");

	if kText ~= nil then
		DoString( kText )
	end

	return true;
end

function EventScript_CALLMINIMAP_OnUpdate(kCmdObj,fAccumTime,fFrameTime)

	local bClose = kCmdObj:GetAttrBool("CLOSE");
	if bClose then
		if IsUIWnd("SFRM_BIG_MAP") then
			CloseUI("SFRM_BIG_MAP")
		end
	else
		if not IsUIWnd("SFRM_BIG_MAP") then
			CallWorldMap(true)
		end
	end
	return	true;
end

function EventScript_CALLDESCUI_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local	iTTWID = kCmdObj:GetAttrInt("TextTableID");
	local	iAliveTime = kCmdObj:GetAttrInt("ALIVE_TIME")
	CALL_SUPER_GROUND_MAP_DESC(iTTWID, iAliveTime)
	return true;
end
function EventScript_CALLSUPERGROUNDTOOLTIP_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local bClose = kCmdObj:GetAttrBool("CLOSE");
	local iAliveTime = kCmdObj:GetAttrInt("ALIVE_TIME")
	if bClose then
		CloseAllClickHelpUI()
	else
		CallClickHelpUICustom(GetUIWnd("SFRM_BIG_MAP"), 799218, 1, 5,3,9,10,-0.93,-3.0,iAliveTime)
		CallClickHelpUICustom(GetUIWnd("FRM_SUPER_GND_TITLE"):GetControl("BTN_LEAVE"), 799219, 2, 5,3,9,15,-0.97,-1.8,iAliveTime)
		CallClickHelpUICustom(GetUIWnd("FRM_SUPER_CUR_FLOOR_SHORT"), 799220, 3, 5,3,9,20,-1.00,-1.7,iAliveTime)
		CallClickHelpUICustom(GetUIWnd("FRM_SUPER_CUR_FLOOR_SHORT"):GetControl("FRM_REMAIN_MONSTER"), 799221, 4, 6,3,11,15,-0.975,-1.3,iAliveTime)
		CallClickHelpUICustom(GetUIWnd("FRM_IN_SUPER_GROUND_ITEM"), 799222, 5, 6,3,11,20,-0.97,-1.04,iAliveTime)
	end
	return true;
end

function EventScript_DOSCRIPT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local kScript = kCmdObj:GetAttr("OBJECTID")

	if nil ~= kScript then
		GetEventTimer():Add("EVENTSCRIPT_DOSCRIPT", 0.001, kScript, 1, true)
	end

	return true
end

function EventScript_MAPOBEJCT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local	bEnable = kCmdObj:GetAttrBool("enable");
	local	kID = kCmdObj:GetAttr("id");
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if g_world:IsNil() == false then
		g_world:HideNode(kID, not bEnable)
	end
	return true
end

function EventScript_PLAYMOVIE_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local kMovieName = kCmdObj:GetAttr("MOVIENAME")
	if nil~=kMovieName then
		PlayMovie(kMovieName)
	end
	return true
end

function EventScript_HIDEUI_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local	bHide = kCmdObj:GetAttrBool("Enable");
	if false==bHide then
		UIOn()
	else
		UIOff(true)
	end
	return true
end

function EventScript_CHECKWORLDEVENTSCRIPT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	
	local	strLimitQuestID = WideString(kCmdObj:GetAttr("LimitQuestID"))
	local	strWorldEventID = WideString(kCmdObj:GetAttr("WorldEventID"))
	local	kActor = EventScript__Common__GetActor(kCmdObj)
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	
	if strWorldEventID == nil or strWorldEventID:IsNil() then
		return	true
	end
	
	SendCheckWorldEvent(kActor, strLimitQuestID, strWorldEventID)
	return true
end

function EventScript_INGQUESTDOEVENT_OnUpdate(kCmdObj,fAccumTime,fFrameTime)
	local QuestNo = kCmdObj:GetAttrInt("QUEST")
	local EventNo = kCmdObj:GetAttrInt("Event")
	local kActor = EventScript__Common__GetActor(kCmdObj)

	if kActor == nil or kActor:IsNil()  then
		return true
	end
	if EventNo < 0 then
		return true
	end
	if false == IsIngQuest(QuestNo) then
		return true
	end
	GetEventScriptSystem():DeactivateAll()
	GetEventScriptSystem():ActivateEvent(EventNo)
	return false
end