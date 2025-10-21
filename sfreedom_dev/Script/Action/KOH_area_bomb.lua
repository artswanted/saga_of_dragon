
function Skill_KOH_area_bomb_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function Skill_KOH_area_bomb_OnEnter(actor, action)
	--ODS("Skill_KOH_area_bomb_OnEnter\n",false,6482)
	if SkillFunc_OnEnter(actor, action)  then
		if action:GetActionParam() ~= AP_CASTING then
			Skill_KOH_area_bomb_OnCastingCompleted(actor, action);

			local kEffect = actor:GetPilot():GetUnit():GetEffect( action:GetActionNo())
			if false == kEffect:IsNil() then
				action:SetParam(22, kEffect:GetCaster():GetString())
				--ODS("Find Skill_KOH_area_bomb_OnEnter\n",false,6482)
			end			 
		end
		return true
	end
	return false
end

function Skill_KOH_area_bomb_OnCastingCompleted(actor, action)
	return SkillFunc_OnCastingCompleted(actor, action)
end

function Skill_KOH_area_bomb_OnUpdate(actor, accumTime, frameTime)
	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)
end

function Skill_KOH_area_bomb_OnCleanUp(actor, action)
	--ODS("Skill_KOH_area_bomb_OnCleanUp ".. action:GetActionName().."\n",false,6482)
	return SkillFunc_OnCleanUp(actor, action)
end

function Skill_KOH_area_bomb_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore)
end

function Skill_KOH_area_bomb_OnLeave(actor, action)
	--ODS("Skill_KOH_area_bomb_OnLeave\n",false,6482)
	return SkillFunc_OnLeave(actor, action)
end

function Skill_KOH_area_bomb_OnEvent(actor,textKey)
	--ODS("Skill_KOH_area_bomb_OnEvent ".. textKey.."\n",false,6482)

	if "effpos" == textKey or "hit" == textKey then
		local action = actor:GetAction()

		actor:AttachSound(12784,"bomb_explosion_01")
		local kTargetGuid = GUID(action:GetParam(22))
		if nil~=kTargetGuid and false==kTargetGuid:IsNil() then
			local kTargetActor = g_pilotMan:FindPilot( kTargetGuid ):GetActor()
			if kTargetActor:IsNil() == false then
				action:ClearTargetList()
			
				local kActionResult = NewActionResultVector()
				local kResult = kActionResult:GetResult(kTargetGuid, false)
				action:AddTarget(kTargetGuid, kTargetActor:GetHitObjectABVIndex(), kResult)
				DeleteActionResultVector(kActionResult)
				
				kTargetActor:AttachParticleS(10001, "char_root", "efx_domination_ground_fire_01", 1)
				kTargetActor:AttachSound(12784,"bomb_blaze")
				
				local QuakeTime = 1.5
				local QuakeFactor = 4.0
				QuakeCamera(QuakeTime, QuakeFactor)

				--타겟을 찾음..뻥 터트림
				--서버에 보냄
				if actor:IsMyActor() then
					action:BroadCastTargetListModify(actor:GetPilot())
				end
			end
		end
		return true
	end
	return SkillFunc_OnEvent(actor,textKey)
end

function Skill_KOH_area_bomb_OnFindTarget(actor,action,kTargets)
	return SkillFunc_OnFindTarget(actor,action,kTargets)
end
