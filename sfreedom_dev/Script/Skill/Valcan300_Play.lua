function	Skill_Valcan300_Play_LoadToWeapon(actor,action)

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
	if( CheckNil(kNewArrow == nil) ) then return false end
	if( CheckNil(kNewArrow:IsNil()) ) then return false end
	
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return	kNewArrow;

end

function Skill_Valcan300_Play_FindTarget(actor,action)

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

function Skill_Valcan300_Play_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	actor:LockBidirection(false);
	action:SetDoNotBroadCast(true)

	action:SetSlot(0);
	actor:PlayCurrentSlot();
	
	--actor:AttachSound(120,"AutoShootSystem1");
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );
	
	action:SetParamInt(2,1)
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local kEffectDef = GetEffectDef(action:GetActionNo());
	if( CheckNil(kEffectDef == nil) ) then return false end
	if( CheckNil(kEffectDef:IsNil()) ) then return false end
	
	local fDurationTime = kEffectDef:GetDurationTime()/1000.0;
	action:SetParamFloat(8, fDurationTime); -- 동작 시간을 세팅
	action:SetParamFloat(9, g_world:GetAccumTime()); --현재 시간을 기록
	action:SetParamFloat(10, g_world:GetAccumTime()); --현재 시간을 기록
	actor:SetParam("End","FALSE"); -- 클라이언트에 의해서 스킬을 종료하는지 체크
	actor:FreeMove(true)
	return true
end

function Skill_Valcan300_Play_GetAimingTarget(actor,action)
	
	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
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
function Skill_Valcan300_Play_OnCastingCompleted(actor,action)
end

function Skill_Valcan300_Play_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	if bIsBefore == false then
		ODS("Skill_Valcan300_Play_OnTargetListModified\n");
--		Skill_Valcan300_Play_FireToTarget(actor,action);
	end

end

function Skill_Valcan300_Play_FireToTarget(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	actor:ResetAnimation();
	action:SetSlot(1)
	actor:PlayCurrentSlot();

	--actor:LookAt(Point3(action:GetParamFloat(7),action:GetParamFloat(8),action:GetParamFloat(9)),true,false,true);
end

function Skill_Valcan300_Play_BroadCastFire(actor,action,kTargetPos)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	action:SetParamInt(2,1)
	
	if IsSingleMode() then
		Skill_Valcan300_Play_OnTargetListModified(actor,action,false)
	else
		action:BroadCastTargetListModify(actor:GetPilot());
	end
end

function Skill_Valcan300_Play_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
--	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	iState = action:GetParamInt(2);	
	
	if 1 == iState then
		if accumTime - action:GetParamFloat(9) > 0.2 then
			action:SetParamFloat(9, g_world:GetAccumTime());
			action:SetParamInt(2,4)
		end
	elseif 4 == iState then
		if accumTime - action:GetParamFloat(9) > 0.5 then
			action:SetSlot(1);
			actor:PlayCurrentSlot();
			action:SetParamFloat(9, g_world:GetAccumTime());
		end

		if accumTime - action:GetParamFloat(10) > action:GetParamFloat(8) - 0.7 then
			action:SetSlot(0);
			actor:PlayCurrentSlot();
			action:SetParamFloat(10, g_world:GetAccumTime());
			action:SetParamInt(2,3)
			actor:SetTargetAlpha(actor:GetAlpha(),0.0, 0.3);

		end
	end

	ODS("Skill_Valcan300_Play_OnUpdate iState:"..iState.."\n");
	
	return true
end

function Skill_Valcan300_Play_OnCleanUp(actor)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end

--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());

	return true;
end

function Skill_Valcan300_Play_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if actor:GetParam("End") == "TRUE" then
		return true;
	end

	return false;
end

function Skill_Valcan300_Play_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end

	ODS("Skill_Valcan300_Play_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then
	
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);

		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_Valcan300_Play_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then

			kAction:BroadCastTargetListModify(actor:GetPilot(),true);
		
			local	kMovingDir = Point3(0,0,0)
			kMovingDir:SetX(kAction:GetParamFloat(4));
			kMovingDir:SetY(kAction:GetParamFloat(5));
			kMovingDir:SetZ(kAction:GetParamFloat(6));
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			ODS("kMovingDir :"..kMovingDir:GetX()..","..kMovingDir:GetY()..","..kMovingDir:GetZ().."\n");
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
						
			-- 관통
			local iPentrationCount = kAction:GetAbil(AT_PENETRATION_COUNT);
			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
			
			kArrow:Fire();
			
		else
			ODS("Arrow is nil\n");
		end
		
		kAction:ClearTargetList();
		actor:ClearIgnoreEffectList();
					
	end

	return	true;
end
