-- Fireball
-- 캐스팅 이후 자동 샷!

function Skill_Fireball_OnEnter(actor,action)
	actor:Stop()
	actionid = action:GetID();
	if actionid == "a_fireball_casting" then	

		
		if actor:IsMyActor() == true then
		
			weapontype = actor:GetEquippedWeaponType();
			if weapontype ~= 10 then
				AddWarnDataStr(GetTextW(36),0);
				return	false;
			end		
			
			if actor:GetAbil(2)<action:GetAbil(2009) then		-- 마나 포인트 부족
				AddWarnDataStr(GetTextW(35),0);
				return	false;
			end
		end
		
		if actor:GetRemainCoolTime() > 0 then
			return false;
		end
	
		actor:SetTotalMagicCastingTime(2000);
		if actor:IsMyActor() == true then 
			SetCoolTime(2,"Spell Casting");
		end
		
		actor:AttachParticle(200, "p_wp_l_hand", "e_spell_fire");
		actor:AttachParticle(201, "p_wp_r_hand", "e_spell_fire");		
	else

		actor:AttachParticle(2123, "char_root", "e_fireball")

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		actorTarget = g_world:FindTarget(actor, 2, 300.0)
		if actorTarget:IsNil() == false then
			actorTarget:ToLeft(not actor:IsToLeft())
			actorTarget:FindPathNormal()
			actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg_fire")
			actorTarget:ReserveTransitAction("a_dmg")
			if actor:IsMyActor() == true then
					QuakeCamera(0.3, 1.0)
				Net_ReqDamage(actor, actorTarget, 1015)
			end
		end
	end

	actor:SetMagicFireActionID("");

	return true
end

function Skill_Fireball_OnUpdate(actor, accumTime, frameTime)

	action = actor:GetAction();
	
	actionid = action:GetID();

	if actionid == "a_fireball_casting" then
			if actor:GetRemainMagicCastingTime() == 0 then
				return false;
			end
	else 
		--	마법 시전
		if actor:IsAnimationDone() == true then
			return false
		end
	end

	return true
end

function Skill_Fireball_OnLeave(actor, action)
	actionID = action:GetID()
	
	bNowFiring = false;
	if actor:GetAction():GetID() == "a_fireball_fire" then	
		bNowFiring = true;
	end

	if 	actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_handclaps" or
		actionID == "a_fireball_fire" then
		actor:DetachFrom(200);
		actor:DetachFrom(201);
		
		if bNowFiring == true then
			if actor:IsMyActor() == true then 
				SetCoolTime(1,"CoolTime");
			end
			actor:SetTotalCoolTime(1000);
		end
		
		return true
	elseif actionID == "a_dmg" then
		if actor:GetAction():GetID() == "a_fireball_casting" then 
			actor:DetachFrom(200);
			actor:DetachFrom(201);
			return true;
		end
		return false;
    elseif actor:GetAction():GetID() == "a_fireball_casting" then
		if actionID == "a_run_right" or
			actionID == "a_run_left" then
			actor:DetachFrom(200);
			actor:DetachFrom(201);
			return true;
		end
	end
	return false 
end

function Skill_Fireball_OnEvent(actor, textKey)
	action = actor:GetAction();
	if action:GetID() ~= "a_fireball_fire" then
		return false;
	end
	
	if textKey == "hit" then
		--SetCoolTime(actor:GetCurrentAnimTime())
		actor:AttachParticle(2123, "char_root", "e_fireball")

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		actorTarget = g_world:FindTarget(actor, 2, 300.0)
		if actorTarget:IsNil() == false then
			actorTarget:ToLeft(not actor:IsToLeft())
			actorTarget:FindPathNormal()
			actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg_fire")
			actorTarget:ReserveTransitAction("a_dmg")
			if actor:IsMyActor() == true then
					QuakeCamera(0.3, 1.0)
				Net_ReqDamage(actor, actorTarget, 1015)
			end
		end
	end

	return false
end
