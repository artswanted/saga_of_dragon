function Actor_PC_Base_DoDefaultMeleeAttack(actor, action,fAttackHoldingTime,bNoHold)

	local	iTargetCount = action:GetTargetCount();

	if 	iTargetCount == 0 then
		return true;
	end

	local	i = 0;
	
	if bNoHold == nil then
	    bNoHold = false
	end
		
	if fAttackHoldingTime == nil or fAttackHoldingTime == 0 then
		fAttackHoldingTime = g_fAttackHitHoldingTime;
	end
	
	local bWeaponSoundPlayed = false;
	
	while i<iTargetCount do
		
		local	kTargetGUID = action:GetTargetGUID(i);
		local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			
			local	actorTarget = kTargetPilot:GetActor();
			if actorTarget:IsNil() == false and actorTarget:GetPilotGuid() ~= actor:GetPilotGuid() then
			
				local kActionResult = action:GetTargetActionResult(i);
				
				if kActionResult:IsMissed() == false then
					
					local	kActionTargetInfo = action:GetTargetInfo(i);	
					
	                if bWeaponSoundPlayed == false then
	                    bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID();
						if kActionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
						actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, kActionTargetInfo);
                    end	
					
					local	iSphereIndex = action:GetTargetABVShapeIndex(i);
					local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);		

					local bCri = kActionResult:GetCritical()
					local DamageEff = actor:GetDamageEffectID(bCri)
					if nil==DamageEff or 0==string.len(DamageEff) then
						if bCri then
							DamageEff = "e_dmg_cri"
						else
							DamageEff = "e_dmg"
						end
					end
					actorTarget:AttachParticleToPoint(12, pt, DamageEff)

					local	atAction = actorTarget:GetAction()
					
					local	guidPilot = actorTarget:GetPilotGuid()
					local	pilot = g_pilotMan:FindPilot(guidPilot)
					
					local iHoldingTime = fAttackHoldingTime * 1000;
					local	iShakeTime = g_fAttackHitHoldingTime * 1000;
					
					-- 충격 효과 적용
					if bNoHold == false then
					    actor:SetAnimSpeedInPeriod(0.00001,iHoldingTime);
					end
					
					actor:SetShakeInPeriod(5,iShakeTime/2);
					actorTarget:SetShakeInPeriod(5,iShakeTime);		
					
				end

			end
		end
		
		i = i + 1;
		
	end
	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects();
	end
	action:ClearTargetList();
end

function Actor_PC_Base(actor, keyName)
	local action = actor:GetAction()
	local actionID = action:GetID()

	--if actor:IsMyActor() then
	--	ODS("Action ID : " .. actionID .. " keyName : " .. keyName .. "\n")
	--end
	
	-- 일반 공격 처리
	if (string.sub(actionID, 1, 7) == "a_melee" or
		string.sub(actionID, 1, 11) == "a_thi_melee") and 
		(keyName == "hit" or keyName == "additional_hit") then
		Actor_PC_Base_DoDefaultMeleeAttack(actor, actor:GetAction());
	end
	
	return true
end


