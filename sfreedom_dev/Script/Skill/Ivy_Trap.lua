-- [INSTALL_EFFECT_TARGET_NODE] : Install Target에 이펙트를 붙일때, 붙일 노드
-- [INSTALL_EFFECT_ID] : Install Target에 붙일 이펙트 ID
-- [FIRE_CASTER_SCALE_NORMALIZE] : 스킬이 실제 발동될때 캐스터의 크기를 실제 범위와 맞추기 위해 보정하는값
-- [ENTER_TIMER_TIME] : OnEnter에서 타이머의 유지시간을 세팅
-- [ENTER_TIMER_TICK] : OnEnter에서 타이머의 Tick을 세팅

function Ivy_Trap_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return	true;
	
end

function Ivy_Trap_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
			
	local	kMovingVec = actor:GetLookingDir();
	local	fDistance = action:GetSkillRange(0,actor);
	if 0 == fDistance then
		fDistance = 100
	end
	--kMovingVec:Multiply(fDistance);
	kMovingVec:Add(actor:GetNodeWorldPos("char_root"))
	actor:AttachParticleToPoint(9,kMovingVec,"eff_sum_skill_mon_mana_burn_01");	
	return true
end

function Ivy_Trap_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    if bIsBefore == false then
		ODS("Ivy_Trap_OnTargetListModified\n", false, 3851)
		SkillHelpFunc_DefaultHitOneTime(actor,action);
    end

end

function Ivy_Trap_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local animDone = actor:IsAnimationDone()
	if animDone == true then
		return false;
	end
	
	return true
end

function Ivy_Trap_OnCleanUp(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	kCurAction = actor:GetAction()	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	if(kCurAction:GetParam(2) == "EnterPassed") then 
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		actor:DetachFrom(7212)
		actor:DetachFrom(7213)
	end
	
	actor:DetachFrom(9)
	
	return true
end

function Ivy_Trap_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	kCurAction = actor:GetAction()	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
		
	return true
end

function Ivy_Trap_OnCastingCompleted(actor, action)
end

function Ivy_Trap_OnEvent(actor,textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetActionParam() == AP_CASTING then
		return true;
	end
			
	if textKey == "hit" or textKey == "fire" then
		if actor:IsUnderMyControl() then
			local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
				action:CreateActionTargetList(actor);
				if IsSingleMode() then
					Ivy_Trap_OnTargetListModified(actor,action,false)
				else
					action:BroadCastTargetListModify(actor:GetPilot());
				end			
			end	
		end
	end
end