-- Icebolt casting
-- 캐스팅

function Skill_Icebolt_Casting_OnEnter(actor,action)
	actor:Stop()
	
	if actor:IsMyActor() == true then
		weapontype = actor:GetEquippedWeaponType();
		if weapontype ~= 10 then
			AddWarnDataStr(GetTextW(36),0);
			return	false;
		end
		
		if actor:IsMyActor() == true then
			if actor:GetAbil(2)<action:GetAbil(2009) then		-- 마나 포인트 부족
				AddWarnDataStr(GetTextW(35),0);
				return	false;
			end
		end
	
		if actor:GetRemainCoolTime() >0 then
			return false;
		end
	
		if actor:GetMagicFireActionID()=="a_Icebolt_fire" then -- 캐스팅이 이미 완료된 스킬이다.
			return false;
		end
		actor:SetTotalMagicCastingTime(2000);
		SetCoolTime(2,"Spell Casting");
		actor:SetMagicFireActionID("");
	end
	
	actor:AttachParticle(200, "p_wp_l_hand", "e_spell_ice");
	actor:AttachParticle(201, "p_wp_r_hand", "e_spell_ice");
	
	return true
end

function Skill_Icebolt_Casting_OnUpdate(actor, accumTime, frameTime)

	action = actor:GetAction()
	
	if actor:GetRemainMagicCastingTime() == 0 then
		actor:SetMagicFireActionID("a_Icebolt_fire");		-- 캐스팅 완료. A 키에 발사 스크립트 장착!
		actor:SetTotalMagicContinueShotTime(10000);	-- 반복 발사 가능 시작 시간 설정
		return false;	
	end
	
	return true
end

function Skill_Icebolt_Casting_OnLeave(actor, action)
	actionID = action:GetID()

	if actionID == "a_dmg" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_handclaps" then
		
		actor:DetachFrom(200);
		actor:DetachFrom(201);
		
		return true
   end
    
    if  actionID == "a_run_right" or	actionID == "a_run_left" then
			actor:DetachFrom(200);
			actor:DetachFrom(201);
			return true;
	end

	return false 
end