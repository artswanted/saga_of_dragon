function Skill_ChainHurricaneKick_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
			
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function Skill_ChainHurricaneKick_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetSlot(0);
	actor:PlayCurrentSlot();
	actor:AttachParticle(7210,"p_pt_ride","ef_ChainHurricaneKick_01_p_pt_ride");
	actor:AttachParticle(7211,"char_root","ef_ChainHurricaneKick_01_char_root");

	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end

end

function Skill_ChainHurricaneKick_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(3,g_world:GetAccumTime());
	action:SetParamFloat(4,g_world:GetAccumTime());
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_ChainHurricaneKick_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_ChainHurricaneKick_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,50,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_ChainHurricaneKick_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_ChainHurricaneKick_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iCurrentSlot = action:GetCurrentSlot();

	local   kMoveDir = actor:GetLookingDir();
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);

	fMoveSpeed = fMoveSpeed * 0.7;
	
	kMoveDir:Multiply(fMoveSpeed);	
	actor:SetMovingDelta(kMoveDir);

	local   fElapsedTIme = accumTime - action:GetParamFloat(4);
	if fElapsedTIme > 0.25 then

		action:SetParamFloat(4,accumTime);
		action:CreateActionTargetList(actor);
	    
		if IsSingleMode() or action:GetTargetCount() == 0 then
	    
			Skill_ChainHurricaneKick_OnTargetListModified(actor,action,false);
	    
		else
	    
			action:BroadCastTargetListModify(actor:GetPilot());
			action:ClearTargetList();
	    
		end
	end

	local fElapsedTIme2 = accumTime - action:GetParamFloat(3);
	if fElapsedTIme2 > 1.5 then
		actor:DetachFrom(7210,true)
		actor:DetachFrom(7211,true)
	
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

function Skill_ChainHurricaneKick_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:DetachFrom(7210,true)
	actor:DetachFrom(7211,true)

	return true;
end

function Skill_ChainHurricaneKick_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
			
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	actor:EndBodyTrail();

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
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
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

	return 
end
