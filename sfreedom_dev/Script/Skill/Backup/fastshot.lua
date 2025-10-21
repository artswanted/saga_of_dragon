-- Shot

function Skill_FastShot_OnEnter(actor, action)

	if actor:IsMyActor() == true then
		if actor:GetAbil(3)<action:GetAbil(2010) then		-- 차지 포인트 부족
			AddWarnDataStr(WideString("............"),0);
			return	false;
		end
	end

	actorID = actor:GetID()
	actor:SetMagicFireActionID("a_fastshot");
	return true
end

function Skill_FastShot_OnUpdate(actor, accumTime, frameTime)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()

	if actor:IsMyActor() and actor:GetLastAPressDelay()>500 then
		action:SetParam(0,"end");
	end

	if animDone == true then
		if action:GetParam(0) == "end" then
			return false;
		end
		
		actor:PlayCurrentSlot();
	end

	return true
end

function Skill_FastShot_OnLeave(actor, action)
	actionID = action:GetID()
	cur_action = actor:GetAction()

	if actionID == "a_fastshot"  then
		return false
	else
		actor:SetMagicFireActionID("");
		return true
	end
	
	return false 
end

function Skill_FastShot_OnEvent(actor, textKey)
	if textKey == "hit" then
		actor:AttachParticle(100, "char_root", "e_arrow")
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		actorTarget = g_world:FindTarget(actor, 2, 300.0)
		if actorTarget:IsNil() == false then
			Net_ReqDamage(actor, actorTarget, 1)
			actorTarget:ReserveTransitAction("a_dmg")
		end

	end
end
