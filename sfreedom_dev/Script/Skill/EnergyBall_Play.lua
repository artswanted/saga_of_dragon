function Skill_Energy_Ball_Play_OnCastingCompleted(actor, action)

end

function Skill_Energy_Ball_Play_OnEnter(actor, action)

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
	action:SetParamInt(3, 0);

	if actor:IsUnderMyControl() then
		local kSkillDef = GetSkillDef(action:GetActionNo());
		local fTickTime = 0.5;
		if kSkillDef:IsNil() == false then
			local iNeedBaseSkill = kSkillDef:GetAbil(AT_CHILD_SKILL_NUM_01);
			local iUpgradeSkill = actor:CheckSkillExist(iNeedBaseSkill);

			if 0 ~= iUpgradeSkill then
				local kChildSkillDef = GetSkillDef(iUpgradeSkill);
				fTickTime = fTickTime + kChildSkillDef:GetAbil(AT_ADD_TICK_ATTACK_TIME)/1000.0; 
				if fTickTime <= 0.3 then
					fTickTime = 0.3;
				end
			end
		end
		
		action:SetParamFloat(100,fTickTime);
	end
	
	return true
end

function Skill_Energy_Ball_Play_OnFindTarget(actor,action,kTargets)

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

function Skill_Energy_Ball_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_Energy_Ball_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
--	local iCurrentSlot = action:GetCurrentSlot();
	local iState = action:GetParamInt(3);
	
	local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	if fMoveSpeed == 0 then
	    fMoveSpeed = 50
    end
	
	kMoveDir:Multiply(fMoveSpeed);	
	actor:SetMovingDelta(kMoveDir);

	if iState == 0 then
		local fElapsedTIme = accumTime - action:GetParamFloat(1);
		if actor:IsUnderMyControl() then
			local fTickTime = action:GetParamFloat(100);

			if fTickTime == nil then
				fTickTime = 0.5;
			end
			
			if fElapsedTIme > fTickTime then

				action:SetParamFloat(1,accumTime);
				action:CreateActionTargetList(actor);
			    
				if IsSingleMode() or action:GetTargetCount() == 0 then
					Skill_Energy_Ball_Play_OnTargetListModified(actor,action,false);	    
				else		    
					action:BroadCastTargetListModify(actor:GetPilot());
					action:ClearTargetList();
				end
			end
		end
	end

	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 2.0;
    end	

    local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);

	if iState == 0 then
		if fLifeTime - fTotalElapsedTime < 1.5 then
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.5);
			action:SetParamInt(3, 1);
			actor:SetHideShadow(true);
		end
	end    

    if fTotalElapsedTime > fLifeTime then
        return  false
    end	

	return true
end

function Skill_Energy_Ball_Play_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_Energy_Ball_Play_OnLeave(actor, action)
	return true;
end
