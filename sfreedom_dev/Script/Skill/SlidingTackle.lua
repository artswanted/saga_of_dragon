-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Skill_Sliding_Tackle_OnCastingCompleted(actor, action)
end

function Skill_Sliding_Tackle_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_Sliding_Tackle_OnEnter\n");
    
    --	시작 위치 기록
    action:SetParamAsPoint(0,actor:GetPos());
    
    if actor:IsMyActor() then
    
		action:StartTimer(10.0,0.1,0);
    
    end
    
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
    
    action:SetParamInt(3,0) -- State
	return	true;
end

function Skill_Sliding_Tackle_OnTimer(actor,fAccumTime,action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsUnderMyControl() then
		ODS("Skill_Sliding_Tackle_OnTimer fAccumTime:"..fAccumTime.."\n");
		
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());        
	end
			
	return	true;
end

function Skill_Sliding_Tackle_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	ODS("Skill_Sliding_Tackle_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	
	if iAttackRange == 0 then
		iAttackRange = 30
	end
	
	local	kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,30,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	
	local iFound=action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);

	kTargets:DeleteInActionGUIDCont(action);
	kTargets:AddToActionGUIDCont(action);
	
	return	iFound;
	
end

function Skill_Sliding_Tackle_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	local iState = action:GetParamInt(3)
	
	if iState == 0 then
	

		local	kStartPos = action:GetParamAsPoint(0)
		local	kCurrentPos = actor:GetPos()
		local	fDistance = kCurrentPos:Distance(kStartPos)
		local	fMaxDistance = action:GetSkillRange(0,actor);
		if fMaxDistance == 0 then
			fMaxDistance = 2000
		end
		
		if fDistance>fMaxDistance then
			action:DeleteTimer(0);
			action:SetParamInt(3,1);
			actor:PlayNext();
			return	true;
		end
		
		local fAccel = -800 * frameTime;
		local fVelocity = action:GetParamFloat(1);
		
		if fVelocity>0 then
		
			local kMovingDir = actor:GetLookingDir()
			kMovingDir:Multiply(fVelocity);
			
			actor:SetMovingDelta(kMovingDir);
			
			fVelocity = fVelocity+fAccel;
			action:SetParamFloat(1,fVelocity);
			
		else 
			
			action:DeleteTimer(0);
			action:SetParamInt(3,1);
			actor:PlayNext();
			return	true;
		
		end
		
	elseif iState == 1 then
	
		if actor:IsAnimationDone() then
		
			action:SetParam(2,"end");
			return	false;
		
		end

	end

	return	true;
end

function Skill_Sliding_Tackle_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:EndBodyTrail();

	return	true;
end

function Skill_Sliding_Tackle_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	
	if actor:IsMyActor() == false then
		return	true;
	end

	if kCurAction:GetParam(2) == "end" then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	

	return	false;
end

function Skill_Sliding_Tackle_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    
    end

end
