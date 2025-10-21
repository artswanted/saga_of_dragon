function Skill_InstallSelf_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
			
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_InstallSelf_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_InstallSelf_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_InstallSelf_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kTargetPos;
    --if actor:IsMyActor() then
    
        --  생성할 구름의 GUID 를 저장한다.
	    local guid = GUID("123")
	    guid:Generate()
	    action:AddNewGUID(guid);
	    
	    kTargetPos = Skill_InstallSelf_FindSkillActorGenPoint(actor,action)
	    
--        action:SetParamFloat(7,kTargetPos:GetX());
--        action:SetParamFloat(8,kTargetPos:GetY());
--        action:SetParamFloat(9,kTargetPos:GetZ());	    
   	
--	else
	
	    --  내 액터가 아닐때는 패킷으로 받은 GUID 를 사용한다.
	    
--[[	    local   kPacket = action:GetParamAsPacket();
	    local   kGUID = kPacket:PopGuid();
	    action:AddNewGUID(kGUID);
	    
	    ODS("Skill_InstallSelf_Fire kGUID:"..kGUID:GetString().."\n");
	    
	    kTargetPos = Point3(0,0,0);
	    kTargetPos:SetX(kPacket:PopFloat());
	    kTargetPos:SetY(kPacket:PopFloat());
	    kTargetPos:SetZ(kPacket:PopFloat());
	
	end
]]--
	-- 액터를 미리 만들어놓고, 필요하기 전까지 숨겨놓는다.
	Skill_InstallSelf_CreateSkillActor(actor,action,kTargetPos)
	
	
end

function Skill_InstallSelf_OnOverridePacket(actor,action,packet)
	
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
    
end

function Skill_InstallSelf_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_InstallSelf_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_InstallSelf_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_InstallSelf_OnUpdate(actor, accumTime, frameTime)
	
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

function Skill_InstallSelf_OnCleanUp(actor)
    	
	return true;
end

function Skill_InstallSelf_OnLeave(actor, action)

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
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	return false 
end

function Skill_InstallSelf_FindSkillActorGenPoint(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   iDistance = action:GetAbil(AT_DISTANCE);
    
    if iDistance == 0 then
        iDistance = 50
    end
    
    local   kTargetPos = actor:GetPos();
    local   kLookDir = actor:GetLookingDir();
    kLookDir:Multiply(iDistance);
    kTargetPos:Add(kLookDir);
    
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    kTargetPos = g_world:FindActorFloorPos(kTargetPos);
    
    return kTargetPos;
    
end

function Skill_InstallSelf_GetSkillActor(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
		ODS("Skill_InstallSelf_GetSkillActor action:GetTotalGUIDInContainer() == 0\n");
        return nil;
    end    
    
   
    local   kGUID = action:GetGUIDInContainer(0);
    
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    local   kActor = g_world:FindActor(kGUID);
    if kActor:IsNil() == false then 
        return  kActor
    end
    
	ODS("Skill_InstallSelf_GetSkillActor g_world:FindActor Failed kGUID:"..kGUID:GetString().."\n");
    return  nil;

end

function Skill_InstallSelf_CreateSkillActor(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetTotalGUIDInContainer() == 0 then
        return
    end
    
    local   kGUID = action:GetGUIDInContainer(0);
   
    local   iClass = actor:GetAbil(AT_CLASS);
    local   iLevel = actor:GetAbil(AT_LEVEL);
	local	iGender = actor:GetAbil(AT_GENDER);
    
    ODS("Skill_InstallSelf_CreateSkillActor Action:"..actor:GetID().." iClass:"..iClass.." iLevel:"..iLevel.."\n");
	
	local pilot = g_pilotMan:NewPilot(kGUID, iClass, iGender)
	if pilot:IsNil() == false then
	
	    pilot:SetUnit(kGUID,UT_PLAYER,iClass,1,iGender);
		local   kActor = pilot:GetActor();
		kActor:FreeMove(true)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(kGUID, kActor, kTargetPos, 9);
		kActor:ClearActionState();
		kActor:LockBidirection(false);
			    
	    g_pilotMan:InsertPilot(kGUID,pilot);
		kActor:CopyEquipItem(actor);
		local kAction = kActor:UntransitAction("a_idle");
				
        kActor:SetUnderMyControl(true);
		kActor:SetTargetAlpha(0, 0.5,0.2);

		kActor:SetAbil(AT_C_MOVESPEED, actor:GetAbil(AT_C_MOVESPEED));
		kActor:FollowActor(actor);

		kActor:SetAutoDeleteActorTimer(60);

		actor:SetParam("ShadowCopyGuid", kGUID:GetString());		
	end	    

end


function Skill_InstallSelf_OnEvent(actor,textKey)
	
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

        local   kActor = Skill_InstallSelf_GetSkillActor(actor,kAction)
        if kActor ~= nil then
        
	        kActor:SetHide(false);
	        kActor:SetHideShadow(false);            
		else
			ODS("Skill_InstallSelf_OnEvent kActor is nil\n");        
        end        
	end

	return	true;
end
