
function Skill_Dragon_Dive_Finish_OnCastingCompleted(actor,action)
end

function Skill_Dragon_Dive_Finish_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local weapontype = actor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end
	
	if not actor:IsMeetFloor() then
		return	false
	end
	
	return		true
end

function Skill_Dragon_Dive_Finish_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	ODS("Skill_Dragon_Dive_Finish_OnEnter\n", false, 912)
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType()
	
	action:SetParamInt(0, 0)
	
--	actor:StopJump()
--	actor:FreeMove(true)
	actor:StartWeaponTrail()
	actor:PlayCurrentSlot()
	return true
end

function Skill_Dragon_Dive_Finish_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local iAttackRange = action:GetSkillRange(0,actor)	
	if iAttackRange == 0 then
		iAttackRange = 100
	end
	
	local kParam = FindTargetParam()
	kParam:SetParam_1(actor:GetPos(),Point3(0,0,-1))
	kParam:SetParam_2(0,0,iAttackRange,0)
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN)
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)
end

function Skill_Dragon_Dive_Finish_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	ODS("Skill_Dragon_Dive_Finish_OnUpdate\n", false, 912)
	--Act_Melee_Default_OnUpdate(actor, accumTime, frameTime)
	if action:GetParamInt(0) == 0 and actor:IsMeetFloor() then
		--ODS("피니쉬 땅만남\n", false, 912)
		Skill_Dragon_Dive_Finish_OnMeetFloor(actor,action,accumTime)
	end
	if action:GetParamInt(0) == 1 
		--and actor:IsAnimationDone() 
		then
		action:SetParam(1, "end")
		return false
	end
	
	return true
end

function Skill_Dragon_Dive_Finish_OnCleanUp(actor)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	ODS("Skill_Dragon_Dive_Finish_OnCleanUp\n", false, 912)

	actor:FreeMove(false)
	actor:EndWeaponTrail()
	
--	actor:DetachFrom(12)
--	actor:DetachFrom(13)
	return true
end
function Skill_Dragon_Dive_Finish_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	curAction = actor:GetAction()
			
	CheckNil(nil==curAction)
	CheckNil(curAction:IsNil())
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Skill_Dragon_Dive_Finish_OnLeave actionID:"..actionID.."\n", false, 912)
		if actor:IsMyActor() == false then
		return true
	end

	if actionID == "a_jump" then 
		--ODS("여기1\n", false, 912)
		return false
	end
	
	if action:GetActionType()=="EFFECT" then
		return true
	end

	if curParam == "end" then 
		return true
	end	

	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(1,"")
		end
		
	end	
	
	if action:GetEnable() == true then
		
		if actionID == "a_run" or 
			string.sub(actionID,1,7) == "a_melee" or 
			string.sub(actionID,1,11) == "a_thi_melee" then
	
			curAction:SetParam(3,"GoNext")
			curAction:SetParam(1,actionID)	
			
		elseif actionID == "a_telejump" then
			actor:DetachFrom(32)
			return true				
		elseif curParam == "end" and actionID == "a_run" then
			return true
		elseif actionID == "a_idle" or
			actionID == "a_die" or
			actionID == "a_handclaps" or
			actionID == "a_battle_idle" then
			return true
		end
	end
	
	return false 
end

function Skill_Dragon_Dive_Finish_DoDamage(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())	
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID())
		action:StartTimer(0.5,0.1,0)
	    --GetComboAdvisor():AddNextAction("a_jump")
    end

end

function Skill_Dragon_Dive_Finish_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
     if bIsBefore == false then
		
		Act_Melee_Default_HitOneTime(actor,action)
        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true, true)
			return
		end

		local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"))		
		if iTotalHit == nil then
			iTotalHit = 1
		end
        if iTotalHit>1 then
            Act_Melee_Default_HitOneTime(actor,action)
        end        
	end

end

function Skill_Dragon_Dive_Finish_OnMeetFloor(actor,action,accumTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	action:SetParam(6,"HIT")
	action:SetParamInt(0,1)
	actor:FindPathNormal()
	action:SetParamFloat(5,accumTime)
	actor:FreeMove(false)
	
	if actor:IsMyActor() then
		QuakeCamera(g_fMeleeDropQuakeValue[1]
		, g_fMeleeDropQuakeValue[2]
		, g_fMeleeDropQuakeValue[3]
		, g_fMeleeDropQuakeValue[4]
		, g_fMeleeDropQuakeValue[5])
	end
	
	local	kPos = actor:GetPos()	
	actor:AttachParticleToPointS(12, kPos, "ef_thi_jump_down_attk", 1.5)
	
	--스킬 사용후 바로 사용가능 하게 되면 이동시에 용이 안따라와야 하므로 위치값 기반으로 붙임
	kPos = actor:GetNodeWorldPos("char_root")
	local kQuatRotate = actor:GetNodeWorldRotate("char_root")
	actor:AttachParticleToPointWithRotate(13, kPos, "ef_dragon_dive",kQuatRotate, 1)
	actor:AttachSound(2784,"DragonDive_FIRE")
	 
	if actor:IsUnderMyControl() then
	
		action:CreateActionTargetList(actor)
		if IsSingleMode() then
			Act_Melee_OnTargetListModified(actor,action,false)
		else
			action:BroadCastTargetListModify(actor:GetPilot())
		end			
	
	end
	
	Skill_Dragon_Dive_Finish_DoDamage(actor,action)
end

function   Skill_Dragon_Dive_Finish_OnTimer(actor,accumtime,action,iTimerID)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	action:CreateActionTargetList(actor)
	action:BroadCastTargetListModify(actor:GetPilot())
	return true

end