-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function Skill_LightningMargnetPlay_OnCheckCanEnter(actor,action)
	return		true;
	
end
function Skill_LightningMargnetPlay_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end
function Skill_LightningMargnetPlay_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
end

function Skill_LightningMargnetPlay_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true)
	
	actor:SeeFront(true, true)
	actor:FreeMove(true)
	return true
end

function Skill_LightningMargnetPlay_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local iSlot = action:GetCurrentSlot()
	local bDone = actor:IsAnimationDone()
	if true==bDone then
		if 2==iSlot	then
			return false == bDone
		elseif 0==iSlot	then
			actor:AttachParticle(123, "char_root", "ef_skill_LightingMagnet_01_char_root")
		end

		actor:PlayNext()
	end
	local guid = actor:GetCallerGuid()
	if false==guid:IsNil() then
		local Caller = g_pilotMan:FindPilot(guid)
		if true==Caller:IsNil() and 2~=iSlot then		
			actor:AttachSoundToPoint(120, "LightningMagnet_End", actor:GetTranslate())
			action:SetSlot(2)
			actor:PlayCurrentSlot()
			return true
		else
			return true
		end
	else
		return false
	end
	return true
end

function Skill_LightningMargnetPlay_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)
	actor:DetachFrom(123)	--혹시나 하는 마음에
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	
	return true;
end

function Skill_LightningMargnetPlay_OnLeave(actor, action)
	
	return true
end

function Skill_LightningMargnetPlay_OnEvent(actor,textKey)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	if textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0.0, 0.25); -- 일단 처음엔 투명하게
		actor:DetachFrom(123)
	end

	return	true;
end

function Skill_LightningMargnetPlay_OnTargetListModified(actor,action,bIsBefore)
	
end
