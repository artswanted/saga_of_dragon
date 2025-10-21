-- skill

function Skill_BigShot_OnEnter(actor, action)



	if actor:IsMyActor() == true then
	
		weapontype = actor:GetEquippedWeaponType();
		if weapontype ~= 4 then
			AddWarnDataStr(GetTextW(34),0);
			return	false;
		end

		if actor:GetAbil(2)<action:GetAbil(2009) then		-- 마나 포인트 부족
			AddWarnDataStr(GetTextW(35),0);
			return	false;
		end
	end
	
	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:AttachParticle(1, "char_root", "e_bigshot")	
	
	return true
end

function Skill_BigShot_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Skill_BigShot_OnLeave(actor, action)
	actionID = action:GetID()

	if 	actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true
	elseif actor:IsAnimationDone() == true then
		if actionID == "a_run_right" or
			actionID == "a_run_left" then
		end
		return true
	end
	return false 
end

function Skill_BigShot_OnEvent(actor, textKey)
	
	if textKey == "hit" then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		actorTarget = g_world:FindTarget(actor, 2, 300.0)
		if actorTarget:IsNil() == false then
			Net_ReqDamage(actor, actorTarget, 1022)
			actorTarget:ReserveTransitAction("a_dmg")
			actorTarget:AttachParticle(21857, "char_root", "e_dmg_msg_02")
		end
	end
	
	return true
end

