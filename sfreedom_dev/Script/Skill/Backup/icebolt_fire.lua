-- Icebolt
-- 캐스팅 이후 샷 반복!

function Skill_Icebolt_Fire_OnEnter(actor,action)

	actor:Stop()
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

	if actor:GetRemainCoolTime()>0 then	
		return false;	
	end
		
	if actor:IsMyActor() == true and actor:GetRemainMagicContinueShotTime() == 0 then
		actor:SetMagicFireActionID("");	-- 샷 지속시간이 끝나면 A 키에서 이 스킬을 떼어낸다.
		return false;	--	스킬 시전을 종료한다.
	end

		actor:AttachParticle(2123, "char_root", "e_icebolt")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		actorTarget = g_world:FindTarget(actor, 2, 300.0)
		if actorTarget:IsNil() == false then
			actorTarget:ToLeft(not actor:IsToLeft())
			actorTarget:FindPathNormal()
			actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg_ice")
			actorTarget:ReserveTransitAction("a_dmg")
			if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) == true then
					QuakeCamera(0.3, 1.0)
				Net_ReqDamage(actor, actorTarget, 1017)
			end
		end
	return true
end

function Skill_Icebolt_Fire_OnUpdate(actor, accumTime, frameTime)

	if actor:IsAnimationDone() == true then
		return false
	end

	return true
end

function Skill_Icebolt_Fire_OnLeave(actor, action)
	
	actionID = action:GetID()

	if 	actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_handclaps" then
		
		if actor:IsMyActor() == true then 
			SetCoolTime(0.5,"CoolTime");
		end
		actor:SetTotalCoolTime(500);
		return true
	elseif actionID =="a_Icebolt_fire" then
		if actor:GetRemainMagicContinueShotTime() >0 then
			return false;
		else
			actor:SetMagicFireActionID("");	-- 샷 지속시간이 끝나면 A 키에서 이 스킬을 떼어낸다.
		end
		
	end
	return false 
	
end
function Skill_Icebolt_Fire_OnEvent(actor, textKey)
	if textKey == "hit"  then

	end

	return false 
end
