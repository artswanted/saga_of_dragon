-- Melee
function ThiefFloatAttack_OnCastingCompleted(actor,action)
end

function ThiefFloatAttack_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if (actor:CheckStatusEffectExist("se_transform_to_metamorphosis")) then
		return false;
	end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == true then
			return	false;
	end

	return		true;
	
end
function ThiefFloatAttack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_BLOWUP);
	
	local iTargetCount = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
	ODS("Skill_ThiefFloatMelee_OnFindTarget iTargetCount:"..iTargetCount.."\n");
	
	return	iTargetCount;
end

function ThiefFloatAttack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("ThiefFloatAttack_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(2,g_world:GetAccumTime());
	
	--	마지막 때린 시간
	action:SetParamFloat(3,g_world:GetAccumTime());
	
	actor:StopJump();
	actor:FreeMove(true);
	
	actor:AttachParticle(9182,"char_root","ef_jump_attk_01_char_root");
	
	if actor:IsMyActor() then
	
		GetComboAdvisor():OnNewActionEnter(action:GetID());
		GetComboAdvisor():AddNextAction("a_thi_melee_drop");
    
    end
	
	return true
end

function ThiefFloatAttack_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	local	fMaxTime = 2;	--	최대 때릴 수 있는 시간
	local	fMaxTimeFromRecentAttack = 0.5;	--	마지막 때린 시간부터 최소한 0.5초내에 때려야한다.
	local	fAutoFireDelay = 0.1;
	
	if action:GetParamInt(5) == 0 then
		ThiefFloatAttack_AttackKeyInput(actor,action)
		action:SetParamInt(5,1)
	end
	
	if actor:IsMyActor() then
	
		local	fElapsedTime = accumTime - action:GetParamFloat(2)
		--ODS("ThiefFloatAttack_OnUpdate fElapsedTime :"..fElapsedTime.."\n");
		
		if 	fElapsedTime>fMaxTime then
			action:SetParam(1,"end");
			return	false
		end
		
		fElapsedTime = accumTime - action:GetParamFloat(3)
		if fElapsedTime>fMaxTimeFromRecentAttack then
			action:SetParam(1,"end");
			return	false
		end
		
		if fElapsedTime>fAutoFireDelay and KeyIsDown(KEY_FIRE) then
			actor:ReserveTransitAction(actor:GetNormalAttackActionID(),DIR_NONE);
		end
	
	end

	return true
end

function ThiefFloatAttack_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	end

end

function ThiefFloatAttack_AttackKeyInput(actor,CurAction,kNewAction)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==CurAction);
	CheckNil(CurAction:IsNil());
	
	if Act_Melee_IsToDownAttack(actor,kNewAction) then
		return	true
	end

	local	iMaxHit = 15;	--	최대 때릴수 있는 횟수
	local	iCurrentHit = CurAction:GetParamInt(4);
	if iCurrentHit>=iMaxHit then
		return
	end
	
	if actor:IsMyActor() then
		GetComboAdvisor():OnNewActionEnter(CurAction:GetID());
		GetComboAdvisor():AddNextAction("a_thi_melee_drop");
    end
	
	iCurrentHit = iCurrentHit+1;
	CurAction:SetParamInt(4,iCurrentHit);

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	CurAction:SetParamFloat(3,g_world:GetAccumTime());
	CurAction:CreateActionTargetList(actor);
	
	if CurAction:GetTargetCount()>0 then
		if actor:IsMyActor() then
			QuakeCamera(0.1,1,0,0.01,10);
		end	
	end
	
	if IsSingleMode() or CurAction:GetTargetCount() == 0 then
	
		ThiefFloatAttack_OnTargetListModified(actor,CurAction,false);
	
	else
        CurAction:BroadCastTargetListModify(actor:GetPilot());
        CurAction:ClearTargetList();	
	end
	
	
	return	false;

end

function ThiefFloatAttack_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:FreeMove(false);
	actor:SetParam("FLOAT_ATTACK_ENABLE","FALSE");
	actor:DetachFrom(9182);

	return true;
end

function ThiefFloatAttack_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
		
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()

	if actionID == "a_jump" then 
		if curParam == "end" or actor:IsMyActor() == false then
			action:SetSlot(2)	
			return	true;
		end
		return false;
	end
	
	if actor:IsMyActor() == false then
		return true;
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
	
		if string.sub(action:GetID(),1,11) == "a_thi_melee" then
			return ThiefFloatAttack_AttackKeyInput(actor,curAction,action);
		end

	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

