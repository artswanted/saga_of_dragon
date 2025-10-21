function	Skill_Bro_Adogen_Fire_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

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

function Skill_Bro_Adogen_Fire_FindTarget(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	
    kParam:SetParam_1(kPos,actor:GetLookingDir());
    kParam:SetParam_2(action:GetSkillRange(0,actor),60,0,0);
    kParam:SetParam_3(true,FTO_NORMAL);
    
    local iFoundNum = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
    
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

function Skill_Bro_Adogen_Fire_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	actor:LockBidirection(false);
	action:SetDoNotBroadCast(true)

	action:SetSlot(0);
	actor:PlayCurrentSlot();
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );	
	action:SetParamInt(2,1)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(9, g_world:GetAccumTime()); --현재 시간을 기록
	action:SetParamInt(10, 1);
	actor:SetParam("End","TRUE"); -- 클라이언트에 의해서 스킬을 종료하는지 체크
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end
	actor:FreeMove(true)
	return true
end

function Skill_Bro_Adogen_Fire_GetAimingTarget(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
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
function Skill_Bro_Adogen_Fire_OnCastingCompleted(actor,action)
end

function Skill_Bro_Adogen_Fire_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if bIsBefore == false then
		ODS("Skill_Bro_Adogen_Fire_OnTargetListModified\n");
--		Skill_Bro_Adogen_Fire_FireToTarget(actor,action);
	end
end

function Skill_Bro_Adogen_Fire_FireToTarget(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	actor:ResetAnimation();
	action:SetSlot(1)
	actor:PlayCurrentSlot();
end

function Skill_Bro_Adogen_Fire_BroadCastFire(actor,action,kTargetPos)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetParamInt(2,1)
	
	if IsSingleMode() then
		Skill_Bro_Adogen_Fire_OnTargetListModified(actor,action,false)
	else
		action:BroadCastTargetListModify(actor:GetPilot());
	end
end

function Skill_Bro_Adogen_Fire_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	
	local   fElapsedTime = accumTime - action:GetParamFloat(9);
	local iState = action:GetParamInt(10);	
	if true == animDone then
		if( iState == 1) then 
			actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.2);
			action:SetParamInt(10,2);
			return true;
		elseif iState == 2 then
			if( true == actor:IsAlphaTransitDone()) then
				return false
			end			
		end
	end
	--ODS("Skill_Bro_Adogen_Fire_OnUpdate iState:"..iState.."\n");
	
	return true
end

function Skill_Bro_Adogen_Fire_OnCleanUp(actor)
	
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

function Skill_Bro_Adogen_Fire_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
		
	if actor:GetParam("End") == "TRUE" then		
		return true;
	end

	return false;
end

function Skill_Bro_Adogen_Fire_OnEvent(actor,textKey)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end

	--ODS("Skill_Bro_Adogen_Fire_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then		
--[[		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end	
]]
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);

		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_Bro_Adogen_Fire_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then

			kAction:BroadCastTargetListModify(actor:GetPilot(),true);
		
			local	kMovingDir = Point3(0,0,0)
			kMovingDir:SetX(kAction:GetParamFloat(4));
			kMovingDir:SetY(kAction:GetParamFloat(5));
			kMovingDir:SetZ(kAction:GetParamFloat(6));
			local	arrow_worldpos = kArrow:GetWorldPos();	--	장풍의 위치
			
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
