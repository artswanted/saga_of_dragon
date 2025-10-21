function Actor_Mob_Base(actor, keyName)
	local action = actor:GetAction()
	
	
	local actionID = action:GetID()

	-- 공통 처리
	if string.sub(actionID, 1, 11) == "a_mon_melee" and string.sub(keyName, 1, 3) == "hit" then
		Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);
	end
	
--	if string.sub(actionID, 1, 10) == "a_mon_shot" or string.sub(actionID, 1, 10) == "a_mon_cast" then
--		if keyName ~= "fire" and keyName ~= "hit" then
--			return false
--		end
		
--		local skillType = action:GetParam(0)		
--		ODS("Act_Mon_RangeAttack_OnEvent "..keyName..","..skillType.."\n")
--		if action:IsNil() == true or action:GetTargetCount() == 0 then
--			ODS("Act_Mon_RangeAttack_OnEvent TargetCount = "..action:GetTargetCount().."\n")
--			return false
--		end
		
			
		-- Target이 여러개면 어떻게 할까?
--		local kActionTargetInfo = action:GetTargetInfo(0)
--		if kActionTargetInfo == nil or kActionTargetInfo:IsNil() == true then
--			ODS("Act_Mon_RangeAttack_OnEvent ActionTargetInfo is nil\n")
--			return false
--		end
		
--		local kTargetPilot = g_pilotMan:FindPilot(action:GetTargetGUID(0))
--		if kTargetPilot == nil or kTargetPilot:IsNil() == true then
--			ODS("Act_Mon_RangeAttack_OnEvent Can't find pilot"..action:GetTargetGUID(0).."\n")
--			return false
--		end
--		local iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0)
		
--		local kTargetActor = kTargetPilot:GetActor()
--		if kTargetActor == nil or kTargetActor:IsNil() == true then
--			ODS("Act_Mon_RangeAttack_OnEvent Can't find Actor\n")
--			return false
--		end

--		if keyName == "fire" then			
--			--if skillType == 2 or skillType == 3 or skillType == 5 then				
--				local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")
--				local fireStartNode	= actor:GetAnimationInfo("FIRE_START_NODE")		
--				local kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
				
				-- startNode가 없으면 이펙트도 못 붙이고, 쏠 수도 없다.
--				if fireStartNode == nil then
--					ODS("Act_Mon_RangeAttack_OnEvent FireStartNode nil \n")
--					SetBreak()
--					return false
--				end
				
				-- Projectile 을 쏜다.
--				if kProjectileID ~= nil then
--					ODS("Act_Mon_RangeAttack_OnEvent CreateNewProjectile \n")
--					local kPilotGuid = actor:GetPilotGuid();
--					local kProjectileMan = GetProjectileMan();
--					local kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid);
--					if kNewArrow ~= nil and kNewArrow:IsNil() == false then
--						ODS("Act_Mon_RangeAttack_OnEvent Attach Projectile \n")
--						kNewArrow:LoadToHelper(actor,fireStartNode);
--						--action:SetParamInt(3,kNewArrow:GetUID());
--						kNewArrow:SetTargetObject(kActionTargetInfo);
--						kNewArrow:Fire()
--					end
--				end		
				
--				action:ClearTargetList();	
				
--				if fireEffect ~= nil then
--					actor:AttachParticle(123, fireStartNode, fireEffect)
--				end		
--			--end
--		end
--		if keyName == "hit" then
			--if skillType == 0 or skillType == 4 then
--				local targetEffectNode = actor:GetAnimationInfo("TARGET_EFFECT_NODE")
--				local targetEffect = actor:GetAnimationInfo("TARGET_EFFECT")
				
--				if targetEffectNode == nil or targetEffect == nil then
--					return false
--				end

--				kTargetActor:AttachParticle(124, targetEffectNode, targetEffect)
--			--end
--		end
--		ODS("Act_Mon_RangeAttack_OnEvent end\n")
--	end

	return true
end
function	CanTransitToAttackAction(kActor)

	local kAction = kActor:GetAction();
	if kAction:IsNil() == true	then
		return	true;
	end
	
	local kActionID = kAction:GetID();
	
	if kActionID == "a_blow_up" or 
		kActionID == "a_knock_back" or
		kActionID == "a_dmg" then
		return	false;
	end
	
	return	true;
	
end

function	CanTransitToDamageAction(kActor)	-- kActor 가 대미지 모션을 할 수 있는 상태인지 리턴한다.

	local kAction = kActor:GetAction();
	if kAction:IsNil() == true	then
		return	true;
	end
	
	local kActionID = kAction:GetID();
	
	if kActionID == "a_blow_up" or 
		kActionID == "a_knock_back" then
		return	false;
	end
	
	return	true;

end

