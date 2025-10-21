function	Skill_AutoShootSystem_Fire_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return	kNewArrow;

end

function Skill_AutoShootSystem_Fire_FindTarget(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	
    kParam:SetParam_1(kPos,actor:GetLookingDir());
    kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
    kParam:SetParam_3(true,FTO_NORMAL);            	
    
    local iFoundNum = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
    
    if iFoundNum == 0 then
		return	nil;
	end
	
	local	kTargetInfo = kTargets:GetTargetInfo(0);
	if kTargetInfo:IsNil() then
		return	nil;
	end
	
	local	kTargetGUID = kTargetInfo:GetTargetGUID();
	if kTargetGUID:IsNil() then
		return	nil;
	end
	
	local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
	if kTargetPilot:IsNil() then
		return	nil
	end
	
	local	kTargetActor = kTargetPilot:GetActor();
	if kTargetActor:IsNil() then
		return	nil
	end
    
    DeleteActionTargetList(kTargets);
    
    return  kTargetActor;
end

function Skill_AutoShootSystem_Fire_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	actor:LockBidirection(false);
	action:SetDoNotBroadCast(true)
	
	actor:AttachSound(120,"AutoShootSystem1");
	
	action:SetParamInt(2,4)
	
	-- Looking Dir
--	local kLookDir = actor:GetLookingDir();
--	action:SetParamFloat( 4, kLookDir:GetX() );
--	action:SetParamFloat( 5, kLookDir:GetY() );
--	action:SetParamFloat( 6, kLookDir:GetZ() );
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kEffectDef = GetEffectDef(action:GetActionNo());
	local fDurationTime = kEffectDef:GetDurationTime()/1000.0;
	action:SetParamFloat(4, fDurationTime); -- 동작 시간을 세팅
	action:SetParamFloat(5, g_world:GetAccumTime()); --현재 시간을 기록
	action:SetParamInt(10, 0);
	actor:SetParam("End","FALSE"); -- 클라이언트에 의해서 스킬을 종료하는지 체크
	actor:FreeMove(true)
	return true
end

function Skill_AutoShootSystem_Fire_GetAimingTarget(actor,action)
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetTotalGUIDInContainer() == 0 then
		return nil;
	end
	
	local	kTargetGUID = action:GetGUIDInContainer(0);
	if kTargetGUID:IsNil() then
		return	nil;
	end
	
	local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
	if kTargetPilot:IsNil() then
		return	nil
	end
	
	local	kTargetActor = kTargetPilot:GetActor();
	if kTargetActor:IsNil() then
		return	nil
	end
	
	return	kTargetActor;

end
function Skill_AutoShootSystem_Fire_OnCastingCompleted(actor,action)
end

function Skill_AutoShootSystem_Fire_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if bIsBefore == false then
		ODS("Skill_AutoShootSystem_Fire_OnTargetListModified\n");
		Skill_AutoShootSystem_Fire_FireToTarget(actor,action);
	end

end

function Skill_AutoShootSystem_Fire_FireToTarget(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:ResetAnimation();
	action:SetParamInt(2,2)
	action:SetSlot(1)
	actor:PlayCurrentSlot();

	local   kPacket = action:GetParamAsPacket();
	
	if kPacket ~= nil and kPacket:IsNil() == false then
		action:SetParamFloat(7,kPacket:PopFloat());
		action:SetParamFloat(8,kPacket:PopFloat());
		action:SetParamFloat(9,kPacket:PopFloat());
	end
	
	actor:LookAt(Point3(action:GetParamFloat(7),action:GetParamFloat(8),action:GetParamFloat(9)),true,false,true);
	
end

function Skill_AutoShootSystem_Fire_BroadCastFire(actor,action,kTargetPos)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetParamInt(2,1)
	
	action:SetParamFloat(7,kTargetPos:GetX());
	action:SetParamFloat(8,kTargetPos:GetY());
	action:SetParamFloat(9,kTargetPos:GetZ());
	
	if IsSingleMode() then
	
		Skill_AutoShootSystem_Fire_OnTargetListModified(actor,action,false)
		
	else	
		action:BroadCastTargetListModify(actor:GetPilot(),true);		
	end

end

function Skill_AutoShootSystem_Fire_OnOverridePacket(actor,action,packet)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    if action:GetParamInt(2) == 1 then
		packet:PushFloat(action:GetParamFloat(7));
		packet:PushFloat(action:GetParamFloat(8));
		packet:PushFloat(action:GetParamFloat(9));
    end
    
end

function Skill_AutoShootSystem_Fire_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	iState = action:GetParamInt(2);	
	
	if accumTime - action:GetParamFloat(5) >  action:GetParamFloat(4) then
		actor:SetParam("End","TRUE");
		return false;
	end

	if accumTime - action:GetParamFloat(5) > action:GetParamFloat(4) - 1 then
		if 0 == action:GetParamInt(10) then
			actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.5);
			action:SetParamInt(10,1);
		end
	end

	ODS("Skill_AutoShootSystem_Fire_OnUpdate iState:"..iState.."\n");
	
	if iState == 0 then	--	Idle State
	
		if actor:IsUnderMyControl() then
			--	0.5 초에 한번씩 주변의 적을 찾는다
			local	fLastTime = action:GetParamFloat(3)
			local	fElapsedTime = accumTime - fLastTime;
			
			if fElapsedTime > 0.5 then
				action:SetParamFloat(3,accumTime);
			
				local	kTargetActor = Skill_AutoShootSystem_Fire_FindTarget(actor,action)
				if kTargetActor ~= nil then					
					kTargetActor:AttachParticle(7215, "p_ef_heart", "ef_select_01");
				
					Skill_AutoShootSystem_Fire_BroadCastFire(actor,action,kTargetActor:GetPos());
					return	true;
				
				end
			
			end
			
		end
	
	elseif iState == 1 then	-- Waiting State
	elseif iState == 2 then -- Firing State
	
		if actor:IsAnimationDone() then
			--	To Idle State
			action:SetParamInt(2,0);
			action:SetSlot(0);
			actor:PlayCurrentSlot();
			return	true;
		end		
		
	elseif iState == 4 then
		
		if accumTime - action:GetActionEnterTime() >1 then
			action:SetParamInt(2,0)
		end
	end

	return true
end

function Skill_AutoShootSystem_Fire_OnCleanUp(actor)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());

	return true;
end

function Skill_AutoShootSystem_Fire_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if actor:GetParam("End") == "TRUE" then
		return true;
	end

	return false;
end

function Skill_AutoShootSystem_Fire_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	

	ODS("Skill_AutoShootSystem_Fire_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then
	
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);

--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_AutoShootSystem_Fire_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			ODS("kMovingDir :"..kMovingDir:GetX()..","..kMovingDir:GetY()..","..kMovingDir:GetZ().."\n");
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			kArrow:Fire();
			
		else
			
			ODS("Arrow is nil\n");
		
		end
		
		kAction:ClearTargetList();
		actor:ClearIgnoreEffectList();
					
	end

	return	true;
end
