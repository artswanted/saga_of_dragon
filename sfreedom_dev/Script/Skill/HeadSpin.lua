
-- Melee
function Skill_HeadSpin_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
		
	if(true == actor:IsInCoolTime(action:GetActionNo(), false) ) then
		return false
	end
	
	local curAction = actor:GetAction();
	if( false == curAction:IsNil() ) then
		local kActionName = curAction:GetID();		
	--	ODS("Skill_HeadSpin_OnCheckCanEnter  ActionName : "..kActionName.."\n");
		if("a_Windmill" == kActionName or "a_Nineteen Ninety" == kActionName) then		
			curAction:SetNextActionName("a_head_spin");
			curAction:ChangeToNextActionOnNextUpdate(true, true);
		end
	end
	return		true;
	
end
function Skill_HeadSpin_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_HeadSpin_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();		
end

function Skill_HeadSpin_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:HideParts(EQUIP_POS_WEAPON, true);
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end
	
	SkillHelpFunc_AttachParticleToNode(19002, "FIRE_CASTER_EFFECT_ID", "FIRE_CASTER_EFFECT_TARGET_NODE", 1, actor, action)
	SkillHelpFunc_AttachParticleToNode(19003, "FIRE_CASTER_EFFECT_ID2", "FIRE_CASTER_EFFECT_TARGET_NODE2", 1, actor, action)
		
end

function Skill_HeadSpin_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	--ODS("Skill_HeadSpin_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,iAttackRange,0);
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_HeadSpin_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_HeadSpin_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(30, g_world:GetAccumTime())
	Skill_HeadSpin_OnCastingCompleted(actor,action);
	
	local fTimerTotalTime = action:GetScriptParamAsFloat("ENTER_TIMER_TIME")
	if(0 == fTimerTotalTime) then
		fTimerTotalTime = 2
	end
	
	local fTimeTick = action:GetScriptParamAsFloat("ENTER_TIMER_TICK")
	action:StartTimer(fTimerTotalTime, fTimeTick, 0)
	
	return true
end
function Skill_HeadSpin_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_HeadSpin_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end
end

function Skill_HeadSpin_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
--	local actionID = action:GetID();
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();
	local nextActionName = action:GetNextActionName();
	local iSkillLevel = action:GetAbil(AT_LEVEL);

	if actor:IsMyActor() and 2 < iSkillLevel then
		local	bKeyUp = KeyIsDown(KEY_UP);
		local	bKeyDown = KeyIsDown(KEY_DOWN);
		local	bKeyLeft = KeyIsDown(KEY_LEFT);
		local	bKeyRight = KeyIsDown(KEY_RIGHT);
		
		local	iDir = 0
		if bKeyUp then 
			iDir = iDir + DIR_UP
		end
		if bKeyDown then
			iDir = iDir + DIR_DOWN
		end
		if bKeyLeft then
			iDir = iDir + DIR_LEFT
		end
		if bKeyRight then
			iDir = iDir + DIR_RIGHT
		end

		if iDir ~= 0 then
			local iMoveSpeed = actor:GetAbil(AT_C_MOVESPEED) * 0.5
			local kDirection = actor:GetDirectionVector(iDir);

			kDirection:Multiply(iMoveSpeed);
			actor:SetMovingDelta(kDirection);
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local fTimeDelta = g_world:GetAccumTime() - action:GetParamFloat(30)
			if(fTimeDelta > 0.02) then -- 서버로 이동 위치 갱신 보내는 시간 간격
				g_pilotMan:BroadcastDirection(actor:GetPilot(), iDir)
				action:SetParamFloat(30, g_world:GetAccumTime())
			end
		end
	end	
		
	
	if bAnimDone == true then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면
			local curActionParam = action:GetParam(0)
			if curActionParam == "GoNext" then
				action:SetParam(0, "null")
				action:SetParam(1, "end")
				return false;
			else
				action:SetParam(1, "end")
				return false
			end
		end		
	end	

	return true
end

function Skill_HeadSpin_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(19002);
	actor:DetachFrom(19003);
	
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end

function Skill_HeadSpin_OnLeave(actor, action)

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


function Skill_HeadSpin_OnTimer(actor,fAccumTime,action,iTimerID)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	ODS("Skill_HeadSpin_OnTimer", false, 912)
	if 0 == iTimerID or 1 == iTimerID then

		action:CreateActionTargetList(actor)
		
		if IsSingleMode() or action:GetTargetCount() == 0 then	    
			Skill_HeadSpin_OnTargetListModified(actor,action,false)
		elseif actor:IsUnderMyControl() then
			action:BroadCastTargetListModify(actor:GetPilot())
			action:ClearTargetList()
		end
	end	
	return true
end