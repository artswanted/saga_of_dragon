function Act_LAVALON_breath_l_OnEnter(actor, action)
	return true
end

function Act_LAVALON_breath_l_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	
	actor:SeeFront(true)
	
	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 then
		actor:PlayNext()
	elseif curSlot == 1 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		return false
	end

	return true
end
function Act_LAVALON_breath_l_OnCleanUp(actor, action)
	return true
end
function Act_LAVALON_breath_l_OnLeave(actor, action)
	return true
end

function Act_LAVALON_breath_l_OnEvent(actor, textkey, seq)
	if textkey == "hit" then
--		actor:AttachParticle(12985, "p_ef_head", "e_ef_fossilearth_breath")
	end
	return true
end

function Act_LAVALON_breath_l_OnCastingCompleted(actor, action)
	return true
end

function  Act_LAVALON_breath_l_OnTargetListModified(actor,action,bIsBefore)

    ODS("Act_LAVALON_breath_l_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
        action:GetTargetList():ApplyActionEffects();    
    end
end
