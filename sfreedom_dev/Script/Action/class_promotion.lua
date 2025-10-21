-- spawn
function ClassPromotion_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function ClassPromotion_OnEnter(actor, action)
	
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
			actor:SetParam("CP_PET_RIDE", "T")
			actor:UnmountPet()
		end
	end
	
	if SkillFunc_OnEnter(actor, action)  then
		if action:GetActionParam() ~= AP_CASTING then
			ClassPromotion_OnCastingCompleted(actor, action);
		end
		return true
	end
	return false
end

function ClassPromotion_OnCastingCompleted(actor, action)
	SkillFunc_OnCastingCompleted(actor, action)
	return true
end

function ClassPromotion_OnUpdate(actor, accumTime, frameTime)
	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)	
end
function ClassPromotion_OnCleanUp(actor, action)
	SkillFunc_OnCleanUp(actor, action)
	if( CheckNil(actor:IsNil()) ) then return end
	if( "T" == actor:GetParam("CP_PET_RIDE") ) then
		actor:MountPet()
		actor:SetParam("CP_PET_RIDE", 0)	
	end
	
end

function ClassPromotion_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore)
end

function ClassPromotion_OnLeave(actor, action)
	return SkillFunc_OnLeave(actor, action)
end

function ClassPromotion_OnEvent(actor,textKey)
	return SkillFunc_OnEvent(actor,textKey)
end

function ClassPromotion_OnFindTarget(actor,action,kTargets)
	return 0;
end