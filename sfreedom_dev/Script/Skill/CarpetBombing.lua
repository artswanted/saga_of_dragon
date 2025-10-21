function Skill_CarpetBombing_OnCheckCanEnter(actor,action)
    	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_CarpetBombing_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(10, 0)
	
--	Skill_CarpetBombing_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	--actor:IncRotate(fRotationRadian);
end

--[[
function Skill_CarpetBombing_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Skill_CarpetBombing_Fire\n")
	local  kTargetPos;
	local  kTargetPos2;
	local kCarpetNum = 1;
    if actor:IsMyActor() then

		local	kSkillDef = GetSkillDef(action:GetActionNo());
		if kSkillDef:IsNil() == false then
			local iNeedBaseSkill = kSkillDef:GetAbil(AT_CHILD_SKILL_NUM_01);
			local iUpgradeSkill = actor:CheckSkillExist(iNeedBaseSkill);

			if 0 ~= iUpgradeSkill then
				kCarpetNum = 2;
			end
		end
    
        --  생성할 구름의 GUID 를 저장한다.
	    local guid = GUID("123")
	    guid:Generate();
	    action:AddNewGUID(guid);

		if 2 == kCarpetNum then
			local guid = GUID("456")
			guid:Generate();
			action:AddNewGUID(guid);
		end
	    
	    kTargetPos = Skill_CarpetBombing_FindGenPoint(actor,action)
	    
		action:SetParamInt(6, kCarpetNum); -- 갯수

		if 1 == kCarpetNum then
		    action:SetParamFloat(7,kTargetPos:GetX());
	        action:SetParamFloat(8,kTargetPos:GetY());
			action:SetParamFloat(9,kTargetPos:GetZ());
		elseif 2 == kCarpetNum then
			local	kMovingDir = actor:GetLookingDir();
			local	kMovingDir2 = actor:GetLookingDir();

			kTargetPos2 = Point3(kTargetPos:GetX(), kTargetPos:GetY(), kTargetPos:GetZ());
			
			kMovingDir:Cross(Point3(0,0,1));

			kMovingDir:Multiply(70);
			kTargetPos:Add(kMovingDir);
			
			action:SetParamFloat(7,kTargetPos:GetX());
	        action:SetParamFloat(8,kTargetPos:GetY());
			action:SetParamFloat(9,kTargetPos:GetZ());
		

			kMovingDir2:Cross(Point3(0,0,-1));
			kMovingDir2:Multiply(70);
			kTargetPos2:Add(kMovingDir2);

			action:SetParamFloat(15,kTargetPos2:GetX());
	        action:SetParamFloat(16,kTargetPos2:GetY());
			action:SetParamFloat(17,kTargetPos2:GetZ());	
			
		end
   	
	else
	
	    --  내 액터가 아닐때는 패킷으로 받은 GUID 를 사용한다.
	    
	    local   kPacket = action:GetParamAsPacket();
		kCarpetNum = kPacket:PopInt();
		action:SetParamInt(6, kCarpetNum); -- 갯수

		local   kGUID = kPacket:PopGuid();
	    action:AddNewGUID(kGUID);

		if kCarpetNum == 2 then
			local   kGUID = kPacket:PopGuid();
			action:AddNewGUID(kGUID);
		end
	    
	    kTargetPos = Point3(0,0,0);
	    kTargetPos:SetX(kPacket:PopFloat());
	    kTargetPos:SetY(kPacket:PopFloat());
	    kTargetPos:SetZ(kPacket:PopFloat());

		kTargetPos2 = Point3(0,0,0);
		if kCarpetNum == 2 then
			kTargetPos2:SetX(kPacket:PopFloat());
			kTargetPos2:SetY(kPacket:PopFloat());
			kTargetPos2:SetZ(kPacket:PopFloat());
		end	
	end
	
	-- 액터를 미리 만들어놓고, 필요하기 전까지 숨겨놓는다.
	if kCarpetNum == 1 then
		Skill_CarpetBombing_Create(actor,action,kTargetPos)
	elseif kCarpetNum == 2 then
		Skill_CarpetBombing_Create(actor,action,kTargetPos);
		Skill_CarpetBombing_Create2(actor,action,kTargetPos2);
	end
end
]]--

function Skill_CarpetBombing_OnOverridePacket(actor,action,packet)
	
--[[	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetActionParam() == AP_FIRE then
		local kCarpetNum = action:GetParamInt(6);

		local   kGUID2   =  action:GetGUIDInContainer(1);
		packet:PushInt(kCarpetNum);
		local   kGUID   =   action:GetGUIDInContainer(0);
        packet:PushGuid(kGUID);

		if kCarpetNum == 2 then
			packet:PushGuid(kGUID2);
		end

        packet:PushFloat(action:GetParamFloat(7));
        packet:PushFloat(action:GetParamFloat(8));
        packet:PushFloat(action:GetParamFloat(9));

		if kCarpetNum == 2 then
			packet:PushFloat(action:GetParamFloat(15));
	        packet:PushFloat(action:GetParamFloat(16));
		    packet:PushFloat(action:GetParamFloat(17));
		end
		
    end]]--
    
end

function Skill_CarpetBombing_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	actor:HideParts(EQUIP_POS_WEAPON, true);
	actor:AttachParticle(100,"p_wp_l_hand","Carpet_Bombing");

	if 1 == actor:GetAbil(AT_GENDER) then
		local iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_CarpetBombing_Male",0,1);
		action:SetParamInt(3,iInstanceID);
	else
		local iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_CarpetBombing_Female",0,1);
		action:SetParamInt(3,iInstanceID);
	end

	if(0 == Config_GetValue(HEADKEY_ETC, SUBKEY_DISPLAY_HELMET)) then -- 투구 보이기 옵션이 꺼져있을때
		if actor:CheckStatusEffectExist("se_hide_face") then	-- 얼굴이 감추어져있다면
			actor:HideParts(EQUIP_POS_FACE, false);	-- 얼굴을 보이게 한다 
		end
	end

	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
		
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_CarpetBombing_OnCastingCompleted(actor,action);	
			  	
	return true
end

function Skill_CarpetBombing_OnUpdate(actor, accumTime, frameTime)

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
	
	if action:GetParam(1) == "end" then
		return	false;
	end

	if 0 == action:GetParamInt(10)  and  action:GetActionParam() ~= AP_CASTING then
		actor:LockBidirection(false);
		actor:SeeFront(true);

		-- 방향 제한이 없는곳에서 문제가 생긴다.
		local fRotationRadian = 0;
		local kLastDirection = actor:GetLastDirection();
		if  kLastDirection== DIR_LEFT then
			fRotationRadian = math.pi / 180 * 90;
		elseif kLastDirection == DIR_RIGHT then
			fRotationRadian = math.pi / 180 * -90;
		elseif kLastDirection == DIR_UP then
			fRotationRadian = math.pi / 180 * 180;
		end

		action:SetParamInt(10, 1);
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
			return false
		end
	end

	return true
end

function Skill_CarpetBombing_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:HideParts(EQUIP_POS_WEAPON, false);
	actor:DetachFrom(100,true);

	local	kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	iInstanceID = kAction:GetParamInt(3);
	--(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
	GetStatusEffectMan():RemoveStatusEffectFromActor2(actor:GetPilotGuid(),iInstanceID);

	if(0 == Config_GetValue(HEADKEY_ETC, SUBKEY_DISPLAY_HELMET)) then -- 투구 보이기 옵션이 꺼져있을때
		if actor:CheckStatusEffectExist("se_hide_face") then	-- 얼굴이 감추어져 있는 이펙트가 존재한다면
			actor:HideParts(EQUIP_POS_FACE, true);	-- 얼굴을 감춘다
		end
	end
	actor:RestoreLockBidirection();
	return true;
end

function Skill_CarpetBombing_OnLeave(actor, action)

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
		if( false == actor:PlayNext()) then 
			actor:GetAction():SetNextActionName("a_battle_idle");
		end
		actor:GetAction():ChangeToNextActionOnNextUpdate(true,true);
		return false;
	end
	
	if action:GetActionType()=="EFFECT"  then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == true then
		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);
		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return true 
end

function Skill_CarpetBombing_FindGenPoint(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	fDistance = -196;
	local	kTargetPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(fDistance);
	kTargetPos:Add(kMovingDir);
		
	return	kTargetPos;
end

function Skill_CarpetBombing_GetSkillActor(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
		ODS("Skill_CarpetBombing_GetSkillActor action:GetTotalGUIDInContainer() == 0\n");
        return nil;
    end    
    
   
    local   kGUID = action:GetGUIDInContainer(0);
    
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    local   kActor = g_world:FindActor(kGUID);
    if kActor:IsNil() == false then 
        return  kActor
    end
    
	ODS("Skill_CarpetBombing_GetSkillActor g_world:FindActor Failed kGUID:"..kGUID:GetString().."\n");
    return  nil;
end

function Skill_CarpetBombing_GetSkillActor2(actor,action)
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
		ODS("Skill_CarpetBombing_GetSkillActor action:GetTotalGUIDInContainer() == 0\n");
        return nil;
    end    
    
   
    local   kGUID = action:GetGUIDInContainer(1);
    
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    local   kActor = g_world:FindActor(kGUID);
    if kActor:IsNil() == false then 
        return  kActor
    end
    
	ODS("Skill_CarpetBombing_GetSkillActor g_world:FindActor Failed kGUID:"..kGUID:GetString().."\n");
    return  nil;
end

--[[
function Skill_CarpetBombing_Create(actor,action,kTargetPos)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetTotalGUIDInContainer() == 0 then
        return
    end
    
    local   kGUID = action:GetGUIDInContainer(0);

	local   iClass = action:GetAbil(AT_CLASS);
    local   iLevel = action:GetAbil(AT_LEVEL);
    
    ODS("\nSkill_CarpetBombing_Create Action:"..action:GetID().." iClass:"..iClass.." iLevel:"..iLevel.."\n");
	
	local pilot = g_pilotMan:NewPilot(kGUID, iClass, 0)
	if pilot:IsNil() == false then

		pilot:SetUnit(kGUID,UT_ENTITY,iClass,iLevel,0);
	
		local   kActor = pilot:GetActor();
	    kActor:SetParam("Entity_Create_Time",""..g_world:GetAccumTime());
	
		g_world:AddActor(kGUID, kActor, kTargetPos, 9)
	    if actor:IsMyActor() then
	        kActor:SetUnderMyControl(true);
	    end

		local   kLookTarget = actor:GetLookingDir();
		kLookTarget:Multiply(40);
		kLookTarget:Add(kTargetPos);
		pilot:GetActor():LookAt(kLookTarget,true);
		ODS("\nPos1 : "..kLookTarget:GetX() .." ".. kLookTarget:GetY().." ".. kLookTarget:GetZ().."\n");
		
		kActor:SetHide(true);	    

	    g_pilotMan:InsertPilot(kGUID,pilot);
		local   iActionNo = kActor:GetAbil(AT_MON_SKILL_01)
		
		ODS("AT_MON_SKILL_01 : ".. iActionNo.."\n");
		
		local kAction = kActor:ReserveTransitActionByActionNo(iActionNo);
		if kAction:IsNil() == false then
			kAction:SetDoNotBroadCast(true);
		
			local kLookDir = actor:GetLookingDir();
			
			kAction:SetParamFloat(11,kLookDir:GetX());
			kAction:SetParamFloat(12,kLookDir:GetY());
			kAction:SetParamFloat(13,kLookDir:GetZ());
    		
			if actor:IsMyActor() then
				Net_C_M_REQ_CREATE_ENTITY(kGUID,iClass,iLevel,kTargetPos);
			end
		else
			ODS("Mon Action Creation Failed.\n");
		end
	end	
end

function Skill_CarpetBombing_Create2(actor,action,kTargetPos)	

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetTotalGUIDInContainer() == 0 then
        return
    end
    
    local   kGUID = action:GetGUIDInContainer(1);

	local   iClass = action:GetAbil(AT_CLASS);
    local   iLevel = action:GetAbil(AT_LEVEL);
    
    ODS("\nSkill_CarpetBombing_Create2 Action:"..action:GetID().." iClass:"..iClass.." iLevel:"..iLevel.."\n");
	
	local pilot = g_pilotMan:NewPilot(kGUID, iClass, 0)
	if pilot:IsNil() == false then

		pilot:SetUnit(kGUID,UT_ENTITY,iClass,iLevel,0);
	
		local   kActor = pilot:GetActor();
	    kActor:SetParam("Entity_Create_Time",""..g_world:GetAccumTime());
	
		g_world:AddActor(kGUID, kActor, kTargetPos, 9)
	    if actor:IsMyActor() then
	        kActor:SetUnderMyControl(true);
	    end

		local   kLookTarget = actor:GetLookingDir();
		kLookTarget:Multiply(40);
		kLookTarget:Add(kTargetPos);
		pilot:GetActor():LookAt(kLookTarget,true);
		ODS("\nPos2 : "..kLookTarget:GetX() .." ".. kLookTarget:GetY().." ".. kLookTarget:GetZ().."\n");
		
		kActor:SetHide(true);	    

	    g_pilotMan:InsertPilot(kGUID,pilot);
		local   iActionNo = kActor:GetAbil(AT_MON_SKILL_01)
		
		ODS("AT_MON_SKILL_01 : ".. iActionNo.."\n");
		
		local kAction = kActor:ReserveTransitActionByActionNo(iActionNo);
		if kAction:IsNil() == false then
			kAction:SetDoNotBroadCast(true);
		
			local kLookDir = actor:GetLookingDir();
			
			kAction:SetParamFloat(11,kLookDir:GetX());
			kAction:SetParamFloat(12,kLookDir:GetY());
			kAction:SetParamFloat(13,kLookDir:GetZ());
    		
			if actor:IsMyActor() then
				Net_C_M_REQ_CREATE_ENTITY(kGUID,iClass,iLevel,kTargetPos);
			end
		else
			ODS("Mon Action Creation Failed.\n");
		end
	end	
end
]]--

function Skill_CarpetBombing_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
        actor:ClearIgnoreEffectList();
		
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end

        local   kActor = Skill_CarpetBombing_GetSkillActor(actor,kAction)
        if kActor ~= nil then
        
	        kActor:SetHide(false);
			ODS("Skill_CarpetBombing_OnEvent add\n");
		else
			ODS("Skill_CarpetBombing_OnEvent kActor is nil\n");        
        end

		local kCarpetNum = kAction:GetParamInt(6);

		if kCarpetNum == 2 then
			local   kActor = Skill_CarpetBombing_GetSkillActor2(actor,kAction)
			if kActor ~= nil then
	        
				kActor:SetHide(false);
				ODS("Skill_CarpetBombing_OnEvent add2\n");
			else
				ODS("Skill_CarpetBombing_OnEvent kActor2 is nil\n");        
	        end
		end
        
	end

	return	true;
end
