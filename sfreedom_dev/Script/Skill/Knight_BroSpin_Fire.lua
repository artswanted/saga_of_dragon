function Skill_Knight_Bro_Spin_Fire_OnCastingCompleted(actor, action)

end

function Skill_Knight_Bro_Spin_Fire_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()

	if actor:IsUnderMyControl() then
		action:StartTimer(1.5,0.3,0);
	end
	
	action:SetParamInt(10,0); -- count
	action:SetParamInt(11, action:GetAbil(AT_COUNT)); -- MaxCount
		
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());
	
	action:SetParamFloat(13,actor:GetPos():GetX());
	action:SetParamFloat(14,actor:GetPos():GetY());
	action:SetParamFloat(15,actor:GetPos():GetZ());
	
	--action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParamInt(9,0);
	
	actor:StopJump();
	actor:FreeMove(true);
	
	local	pt = actor:GetNodeTranslate("char_root");
	actor:AttachParticleToPoint(581,pt,"ef_Bro_Spin_char_root");
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end
	actor:FreeMove(true)
	return true
end

function Skill_Knight_Bro_Spin_Fire_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Skill_Knight_Bro_Spin_Fire_OnTimer fAccumTime:"..fAccumTime.."\n");

	local	iHitCount = action:GetParamInt(10);
	local	iTotalHit = action:GetParamInt(11);
	ODS("hit수:".. iHitCount.."\n");
	if iHitCount == iTotalHit then
		return
	end

	action:CreateActionTargetList(actor);
		
	ODS("타겟 갯수:".. action:GetTargetCount().."\n");
		
	if IsSingleMode() then
    	Skill_Knight_Bro_Spin_Fire_OnTargetListModified(actor,action,false)
    	return true;
	else
		
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();

		if 0 < iTargetCount then
			action:BroadCastTargetListModify(actor:GetPilot());
			action:ClearTargetList();
		end
	end	
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(10,iHitCount);
	
	if iHitCount == iTotalHit then
		return	true
	end	
	
	return	true	

end

function Skill_Knight_Bro_Spin_Fire_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	Skill_Knight_Bro_Spin_Fire_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function Skill_Knight_Bro_Spin_Fire_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = Point3(action:GetParamFloat(13),action:GetParamFloat(14),action:GetParamFloat(15) - 30);
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,Point3(0,0,1));
	kParam:SetParam_2(400,iAttackRange*2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP + FTO_NORMAL);

	local iFound = action:FindTargetsEx(iActionNo,TAT_BAR,kParam,kTargets,kTargets);
	return iFound;
end

function Skill_Knight_Bro_Spin_Fire_OnTargetListModified(actor,action,bIsBefore)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_Knight_Bro_Spin_Fire_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	local iActionState = action:GetParamInt(9);
	--공중으로 올라가게 
	actor:SetMovingDelta(Point3(0,0,12));
	--회전
	local DegToRadUnit = 3.141592/180.0;
	local SpinScalar = 4*360
			
	local   fElapsedTime = accumTime - action:GetParamFloat(0);
	
	actor:IncRotate(frameTime*SpinScalar*DegToRadUnit);

	if fElapsedTime> 1.0 then
		if( iActionState == 0) then
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.7);
			action:SetParamInt(9,1);
			actor:DetachFrom(581)
		end
	end

	return true
end

function Skill_Knight_Bro_Spin_Fire_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_Knight_Bro_Spin_Fire_OnLeave(actor, action)
	actor:FreeMove(false);
	actor:StopJump();
	actor:DetachFrom(581)
	return true;
end
