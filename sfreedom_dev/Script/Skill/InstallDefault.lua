-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [DETACH_HIT_CASTER_EFFECT_AT_FINISH] : 스킬이 끝날때 CASTING_CASTER_EFFECT 를 제거할 것인가
-- [INSTALL_EFFECT_TARGET_NODE] : Install Target에 이펙트를 붙일때, 붙일 노드
-- [INSTALL_EFFECT_ID] : Install Target에 붙일 이펙트 ID
-- [USE_SKILL_RANGE] : DB에 설정된 SkillDef에 있는 Range값 만큼 앞에 설치
-- [INTSALL_DELAY] : 설치 후 실제로 나타나는 시간
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : FIRE 키에서 플레이 할 사운드 ID
-- [CASTING_SOUND_ID] : 캐스팅 시에 플레이 할 사운드 ID
-- [BROADCAST_TARGETLIST] : 스킬 발동시에 타겟 리스트를 날릴 것인가?
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [CASTING_EFFECT_TARGET_NODE1] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID1] : 캐스팅 시에 이펙트 붙일 ID
-- [DETACH_CASTING_EFFECT_AT_FINISH] : 스킬이 끝날때 CASTING_CASTER_EFFECT 를 제거할 것인가

function Skill_InstallDefault_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function Skill_InstallDefault_OnCastingCompleted(kActor, kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
--	Skill_InstallDefault_Fire(kActor,kAction);

	local   kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID")
    local   kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE")
	local   kFireEffectTargetNodeIDToPos = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS")
	local   kFireEffectTargetNodeIDToPosWithRotate = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE")

	if kActor:IsMeetFloor() == true then
		kAction:SetSlot(kAction:GetCurrentSlot()+1);
	else
		kAction:SetSlot(kAction:GetCurrentSlot()+2);

		kFireEffectID = "ef_skill_JumpTrrap_01_char_root";
		kFireEffectTargetNodeIDToPosWithRotate = "char_root";
	end
	
	if(1 < kAction:GetSlotCount() ) then 
		kActor:PlayCurrentSlot();
	end 

	local fSize = 1.0
	local kFactorParam = kAction:GetScriptParam("FIRE_CASTER_EFFECT_SIZE_TO_SKILLRANGE_FACTOR")
	if "" ~= kFactorParam then		
		local fFactor = 100		
		fFactor = tonumber(kFactorParam)
		fSize = fSize * (kAction:GetSkillRange(0,kActor) / fFactor)
	end
		
	if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            kActor:AttachParticleToPoint(7212,kDir,kFireEffectID)            
        elseif kFireEffectTargetNodeID ~= "" then
            kActor:AttachParticleS(7212, kFireEffectTargetNodeID, kFireEffectID, fSize)
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			kActor:AttachParticleToPoint(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID)			
		else
		    kActor:AttachParticleToPointWithRotate(7212,kActor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, kActor:GetRotateQuaternion())
        end
    
    end	

    kFireEffectID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2")
    kFireEffectTargetNodeID = kAction:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2")
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = kActor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(kActor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            kActor:AttachParticleToPoint(7213,kDir,kFireEffectID)
            
        else
            kActor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID)
        end    
    end
end

--[[
function Skill_InstallDefault_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kTargetPos;
    if actor:IsMyActor() then
    
        --  생성할 구름의 GUID 를 저장한다.
	    local guid = GUID("123")
	    guid:Generate()
	    action:AddNewGUID(guid);
	    
	    kTargetPos = Skill_InstallDefault_FindSkillActorGenPoint(actor,action)
	    
        action:SetParamFloat(7,kTargetPos:GetX());
        action:SetParamFloat(8,kTargetPos:GetY());
        action:SetParamFloat(9,kTargetPos:GetZ());	    
   	
	else
	
	    --  내 액터가 아닐때는 패킷으로 받은 GUID 를 사용한다.
	    
	    local   kPacket = action:GetParamAsPacket();
	    local   kGUID = kPacket:PopGuid();
	    action:AddNewGUID(kGUID);
	    
	    ODS("Skill_InstallDefault_Fire kGUID:"..kGUID:GetString().."\n");
	    
	    kTargetPos = Point3(0,0,0);
	    kTargetPos:SetX(kPacket:PopFloat());
	    kTargetPos:SetY(kPacket:PopFloat());
	    kTargetPos:SetZ(kPacket:PopFloat());
	
	end
	
	-- 액터를 미리 만들어놓고, 필요하기 전까지 숨겨놓는다.
	Skill_InstallDefault_CreateSkillActor(actor,action,kTargetPos);

	action:SetParamInt(39, 0);
	action:SetParamFloat(40, 0);
	action:SetParamFloat(41, g_world:GetAccumTime()); -- 시작 시간 기록
	local iInstallDelay = tonumber(action:GetScriptParam("INTSALL_DELAY"));
	if iInstallDelay ~= nil then
		action:SetParamFloat(40, iInstallDelay);
		action:SetParamInt(39, 1);
	end		
end
]]--

function Skill_InstallDefault_OnOverridePacket(actor,action,packet)
	--[[
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetActionParam() == AP_FIRE then	
        local   kGUID   =   action:GetGUIDInContainer(0);
        packet:PushGuid(kGUID);
        packet:PushFloat(action:GetParamFloat(7));
        packet:PushFloat(action:GetParamFloat(8));
        packet:PushFloat(action:GetParamFloat(9));
    end
	]]--
    
end

function Skill_InstallDefault_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_InstallDefault_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	local   kFireEffectID1 = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID1");
    local   kFireEffectTargetNodeID1 = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE1");
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" 
	   or (kFireEffectID1 ~= "" and  kFireEffectTargetNodeID1 ~="")
	   then    
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7215,kDir,kFireEffectID);
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticle(7215,kFireEffectTargetNodeID,kFireEffectID);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPoint(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID);			
		else
		    actor:AttachParticleToPointWithRotate(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion())
        end
		
		if kFireEffectTargetNodeID1 ~= "" then 
			actor:AttachParticle(7216,kFireEffectTargetNodeID1,kFireEffectID1)
		end
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end

	local actionID = action:GetID()
	if actionID == "a_LightningMargnet" or actionID == "a_crossblade" then
		if actor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
		end
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_InstallDefault_OnCastingCompleted(actor,action);

	if actor:IsMeetFloor() == false then
		actor:StopJump();
		actor:FreeMove(true);
		local fNowTime = timeGetTime();
		action:SetParamInt(20, fNowTime);
		action:SetParamInt(21, 1);
	end
	
	return true
end

function Skill_InstallDefault_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();

	-- 설치 딜레이가 없으면 바로 설치
	--[[
	if 1 == action:GetParamInt(39) then		
		if 0 ~= action:GetParamFloat(40) then			
			local   fElapsedTIme = accumTime - action:GetParamFloat(41);
			ODS("Skill_InstallDefault_OnUpdate  fElapsedTIme : "..fElapsedTIme.."\n")
			ODS("Skill_InstallDefault_OnUpdate  Install Time : "..action:GetParamFloat(40).."\n")
			if fElapsedTIme > action:GetParamFloat(40) then
				local   kActor = Skill_InstallDefault_GetSkillActor(actor,action)
				if kActor ~= nil then
		   			kActor:SetHide(false);
					kActor:SetHideShadow(false);					
				else
					ODS("Skill_InstallDefault_OnUpdate Skill_InstallDefault_GetSkillActor if Failed\n");
				end
				action:SetParamInt(39, 2);
			end
		end
	end
	]]--

	if action:GetParam(1) == "end" then	
		return	false;
	end
	
	if 1 == action:GetParamFloat(21) then
		local fStartTime = action:GetParamInt(20);
		local fNowTime = timeGetTime();

		-- 공중에서 스킬 사용시 0.15초 동안만 공중에서 멈춘다.
		if (fNowTime - fStartTime) > 150 then
			actor:FreeMove(false);
			action:SetParamFloat(21,2)
		end
	end
	
	if animDone == true then			
	
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false;
		else
		
			action:SetParam(1, "end")
			if actor:IsMeetFloor() == false then
				action:SetNextActionName("a_jump")
				actor:FreeMove(false);
			end	
			return false
		end
	end

	return true
end

function Skill_InstallDefault_OnCleanUp(kActor)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
		
	local   kCurAction = kActor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());

	kActor:FreeMove(false);

	local actionID = kCurAction:GetID();
	if actionID == "a_LightningMargnet"  or actionID == "a_crossblade" then
		if kActor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
		end	
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
	    kActor:DetachFrom(7215);
		kActor:DetachFrom(7216);
	end

	if kCurAction:GetScriptParam("DETACH_HIT_CASTER_EFFECT_AT_FINISH") == "TRUE" then
	    kActor:DetachFrom(7214);
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
	
		local DefaultDetaChID = false
		local DefaultDetaChID2 = false
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true
		end		

	    kActor:DetachFrom(7212,DefaultDetaChID)
	    kActor:DetachFrom(7213,DefaultDetaChID2)
		kActor:DetachFrom(7214)
		kActor:DetachFrom(7217, true) -- 발끌리는 이펙트
	end
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true
		end
		kActor:DetachFrom(7215,DefaultDetaChID)
		kActor:DetachFrom(7216,DefaultDetaChID);
	end
	
	return true;
end

function Skill_InstallDefault_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());	
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return  true;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		actor:FreeMove(false);
		return true;
	end
	
	return false 
end

--[[
function Skill_InstallDefault_FindSkillActorGenPoint(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   iDistance = action:GetAbil(AT_DISTANCE);
 
    if iDistance == 0 then
        iDistance = 50
    end

	if "TRUE" == action:GetScriptParam("USE_SKILL_RANGE") then
		iDistance = action:GetSkillRange(0,actor);
	end

	if action:GetID() == "a_Snow Blues" then
		iDistance = -iDistance;
	end

	if action:GetID() == "a_Energy Ball" then
		iDistance = -17;
	end

	ODS("Skill_InstallDefault_FindSkillActorGenPoint iDistance : "..iDistance.."\n")
    
    local   kTargetPos = actor:GetPos();
    local   kLookDir = actor:GetLookingDir();
    kLookDir:Multiply(iDistance);
    kTargetPos:Add(kLookDir);
    
    kTargetPos = g_world:FindActorFloorPos(kTargetPos);
    
    return kTargetPos;
    
end
]]--

--[[
function Skill_InstallDefault_GetSkillActor(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
		ODS("Skill_InstallDefault_GetSkillActor action:GetTotalGUIDInContainer() == 0\n");
        return nil;
    end    
    
   
    local   kGUID = action:GetGUIDInContainer(0);
    
    local   kActor = g_world:FindActor(kGUID);
    if kActor:IsNil() == false then 
		local   kHitEffectID = action:GetScriptParam("INSTALL_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("INSTALL_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
			local kPos = kActor:GetNodeTranslate(kHitEffectTargetNodeID);
			actor:AttachParticleToPointWithRotate(7214,kPos,kHitEffectID,actor:GetRotateQuaternion());
	    end

        return  kActor
    end
    
	ODS("Skill_InstallDefault_GetSkillActor g_world:FindActor Failed kGUID:"..kGUID:GetString().."\n");
    return  nil;
end
]]--

function Skill_InstallDefault_CreateSkillActor(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
        return
    end
    
    local kGUID = action:GetGUIDInContainer(0);
    local iClass = action:GetAbil(AT_CLASS);
    local iLevel = action:GetAbil(AT_LEVEL);
    
    ODS("Skill_InstallDefault_CreateSkillActor Action:"..action:GetID().." iClass:"..iClass.." iLevel:"..iLevel.."\n");
	
	local pilot = g_pilotMan:NewPilot(kGUID, iClass, 0)
	if pilot:IsNil() == false then
	
	    pilot:SetUnit(kGUID,UT_ENTITY,iClass,iLevel,0);
	
	    local   kActor = pilot:GetActor();
		kActor:FreeMove(true)
	    kActor:SetHide(true);
	    kActor:SetHideShadow(true);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
	    kActor:SetParam("Entity_Create_Time",""..g_world:GetAccumTime());
	
		g_world:AddActor(kGUID, kActor, kTargetPos, 9)
	    if actor:IsMyActor() then
	        kActor:SetUnderMyControl(true);
	    end

		local kUnit = pilot:GetUnit();

		if kUnit ~= nil then
			kUnit:SetCaller(actor:GetPilotGuid());
		end

	    g_pilotMan:InsertPilot(kGUID,pilot);
		local   iActionNo = kActor:GetAbil(AT_MON_SKILL_01)
		
		ODS("AT_MON_SKILL_01 : ".. iActionNo.."\n");
		
		local kAction = kActor:ReserveTransitActionByActionNo(iActionNo);
		if kAction:IsNil() == false then
			kAction:SetDoNotBroadCast(true)
		
			kAction:SetParamFloat(4,actor:GetLookingDir():GetX());
			kAction:SetParamFloat(5,actor:GetLookingDir():GetY());
			kAction:SetParamFloat(6,actor:GetLookingDir():GetZ());
    		
			if actor:IsMyActor() then
				Net_C_M_REQ_CREATE_ENTITY(kGUID,iClass,iLevel,kTargetPos);
			end
		else
			ODS("Mon Action Creation Failed.\n");
		end
	
	end	    

end


function Skill_InstallDefault_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
			
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
		local   kHitEffectID = kAction:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
			local kPos = actor:GetNodeTranslate(kHitEffectTargetNodeID)
			actor:AttachParticleToPointWithRotate(7214,kPos,kHitEffectID,actor:GetRotateQuaternion());
	    end

        actor:ClearIgnoreEffectList();
		local	kSoundID = nil
		if("hit" == textKey) then
			kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		elseif("fire" == textKey) then
			kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID");
		end
		
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end

		-- 설치 딜레이가 없으면 바로 설치
		--[[
		if 0 == kAction:GetParamInt(39) then
			local   kActor = Skill_InstallDefault_GetSkillActor(actor,kAction)
			if kActor ~= nil then
	        
				kActor:SetHide(false);
				kActor:SetHideShadow(false);
			else
				ODS("Skill_InstallDefault_OnEvent kActor is nil\n");
	        end
		end
		]]--

		if "TRUE" == kAction:GetScriptParam("BROADCAST_TARGETLIST") then
			if actor:IsUnderMyControl() then
				kAction:CreateActionTargetList(actor);
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end
		end
	end

	return	true;
end

function Skill_InstallDefault_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end
	end
end

function Skill_InstallDefault_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Skill_InstallDefault_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end
function    Skill_InstallDefault_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
	local   strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");
	if strFindTargetType == "" then
	    strFindTargetType = "RAY"
	end
	
	local iFindTargetType = TAT_RAY_DIR;
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	
	if strFindTargetType == "RAY" then
		if 0==iParam2 then
			iParam2 = 30
		end
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	
	elseif strFindTargetType == "BAR" then
		if 0==iParam2 then
			iParam2 = 60
		end
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	
	elseif strFindTargetType == "SPHERE" then
		if 0==iParam2 then
			iParam2 = 30
		end
	    iFindTargetType = TAT_SPHERE;
    	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
    	kParam:SetParam_2(0,iParam2,iAttackRange,0);
	
	end
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargetsEx(iActionNo,iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end
