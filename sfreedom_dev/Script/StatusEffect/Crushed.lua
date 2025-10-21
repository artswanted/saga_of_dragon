function Effect_Crushed_Begin(actor, iEffectID, iEffectKey)
	if false == actor:IsMyActor() then
		return
	end

	InitEffectEscapeKeyDown()
	LockPlayerInput(100);--EPLT_StatusEffect1

	--CallUI("FRM_LIFT_THE_CHAOS")
end

function Effect_Crushed_End(actor, iEffectID, iEffectKey)
	local kID = actor:GetStatusEffectParam(iEffectID, "RELEASE_PARTICLE")
	if false==kID:IsNil() then
		kID = kID:GetStr()
		local kNode = actor:GetStatusEffectParam(iEffectID, "RELEASE_PARTICLE_TARGET_NODE")
		if kNode:IsNil() or ""==kNode:GetStr() then
			kNode = "char_root"
		else
			kNode = kNode:GetStr()
		end

		actor:AttachParticle(98601, kNode, kID)
	end
	
	if false == actor:IsMyActor() then
		return
	end

	UnLockPlayerInput(100);--EPLT_StatusEffect1
	
	--CloseUI("FRM_LIFT_THE_CHAOS")
end

function Effect_Crushed_OnTick(actor, iEffectID, fAccumTime)
	if false == actor:IsMyActor() then
		return
	end

	ProcessEffectEscapeKeyDown(iEffectID)
end
