function Skill_FrozenShot_Play_OnCastingCompleted(actor, action)

end

function Skill_FrozenShot_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());	
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );

	action:SetSlot(0);
	actor:PlayCurrentSlot();

	local iLearnSkillLv = action:GetSkillLevel()
	local fProjectileFireInter = 1
	
	local iProectileNum = 4
	if(1 < iLearnSkillLv) then
		iProectileNum = 6		
	end
	
	if(2 < iLearnSkillLv) then
		fProjectileFireInter = 0.6
	end
	
	action:SetParamInt(20, iLearnSkillLv);				-- Caller가 몇레벨 스킬을 배웠는가 확인
	action:SetParamFloat(21, fProjectileFireInter);		-- 발사체 발사 시간 간격	
	action:SetParamInt(23, iProectileNum);				-- 발사체 숫자
	
	action:SetDoNotBroadCast(true)
	
	return true
end

function Skill_FrozenShot_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 50
	end	

	local tat_type = TAT_SPHERE

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	if 0==action:GetAbil(AT_1ST_AREA_TYPE) then
		kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	else
		kParam:SetParam_2(action:GetSkillRange(0,actor),iParam2,0,0);
		tat_type = TAT_BAR
	end
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(tat_type,kParam,kTargets,kTargets);
end

function Skill_FrozenShot_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_FrozenShot_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
--	local iCurrentSlot = action:GetCurrentSlot();
	
	local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	if fMoveSpeed == 0 then
	    fMoveSpeed = 50
    end
	
	kMoveDir:Multiply(fMoveSpeed);	
	actor:SetMovingDelta(kMoveDir);


	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 2.0;
    end	

    local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);
    if fTotalElapsedTime > fLifeTime then
        return  false
    end	

	local iLearnSkillLv = action:GetParamInt(20);				-- Caller가 몇레벨 스킬을 배웠는가 확인
	if( 0 < iLearnSkillLv) then
		local fElapsedTime = accumTime - action:GetParamFloat(2)
		local fProjectileFireInter = action:GetParamFloat(21);		-- 발사체 발사 시간 간격
		ODS("fProjectileFireInter:"..fProjectileFireInter.."\n", false, 912)
		if fProjectileFireInter < fElapsedTime then 
			action:SetParamFloat(2, accumTime)		
			SkillHelpFunc_PlaySound(actor, action, "FIRE_SOUND_ID")
			local iProjectileRange = action:GetSkillRange(0,actor);			-- 발사체 거리
			local iProectileNum = action:GetParamInt(23);				-- 발사체 숫자
			local	kProjectileMan = GetProjectileMan();
			local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
			local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");

		-- 회전축 구하기
			local	kRotateAxis = Point3(0,0,1);
			local fRotateAngle = 60.0*(DEGTORAD)
			local	i = 0;
			
			while(i < iProectileNum) do 
	
				local	kArrow = kProjectileMan:CreateNewProjectile(kProjectileID, action, actor:GetPilotGuid());
				kArrow:LoadToHelper(actor, kTargetNodeID);	--	장전
					
				if kArrow:IsNil() == false then
					action:BroadCastTargetListModify(actor:GetPilot(),true);
					
					local	kMovingDir = Point3(0,0,0)
					kMovingDir:SetX(action:GetParamFloat(4));
					kMovingDir:SetY(action:GetParamFloat(5));
					kMovingDir:SetZ(action:GetParamFloat(6));
					local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
					kMovingDir:Multiply(iProjectileRange);
					kMovingDir:Rotate(kRotateAxis,fRotateAngle*(i)+30);
					kMovingDir:Add(arrow_worldpos);
					
					kArrow:SetTargetLoc(kMovingDir);
					kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
					
					-- 관통
					local iPentrationCount = action:GetAbil(AT_PENETRATION_COUNT);
					kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
					if iPentrationCount ~= 0 then
						kArrow:SetMultipleAttack(true);
					end
					kArrow:Fire();
				end
				i=i+1
			end	

		end
	end

	return true
end

function Skill_FrozenShot_Play_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_FrozenShot_Play_OnLeave(actor, action)
	return true;
end
