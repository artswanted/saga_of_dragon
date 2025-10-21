-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID

function Skill_Magician_Charge_Laser_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
		return	false;
	end
	
	return		true;

end

function Skill_Magician_Charge_Laser_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1);
	if actor:IsMyActor() == false then
		Skill_Magician_Charge_Laser_Fire(actor,action);
	end				
end

function Skill_Magician_Charge_Laser_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(3,0);
	action:SetParamInt(1,0);
	action:SetParamInt(5,0);
				
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	

	if action:GetActionParam() == AP_CASTING then
		-- 양손에 캐스팅 이펙트를 붙인다.
		actor:AttachParticle(11,"p_wp_r_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
		actor:AttachParticle(12,"p_wp_l_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
	end
		
	action:SetAutoBroadCastWhenCastingCompleted(false);	
	action:SetParamInt(6,0);
	
   return true
end

function Skill_Magician_Charge_Laser_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(1,1);
	
	actor:PlayNext();
	
	actor:DetachFrom(11);
	actor:DetachFrom(12);
	
	action:SetParamInt(3,0);		
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		
		action:SetActionInstanceID();
		action:BroadCast(actor:GetPilot());
	end

    actor:AttachParticle(15,"char_root","ef_mag_gathering_01");
    
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end    
    
end

function Skill_Magician_Charge_Laser_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor)
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,50,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_Magician_Charge_Laser_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	bKeyCheck = KeyIsDown(action:GetActionKeyNo(0))
	local	iState = action:GetParamInt(6);
	
	if actor:IsMyActor() == false then
		bKeyCheck = true;
	end
	
	if iState == 0 then
	
		if animDone == true then
			if curSlot == 0 then
				actor:PlayNext();
				local	kMovingVec = actor:GetLookingDir();
				local	fDistance = 100;
				kMovingVec:Multiply(fDistance);
				kMovingVec:Add(actor:GetTranslate());
				kMovingVec:SetZ(kMovingVec:GetZ() - 20)	
				action:SetParamInt(3,1);							
			end
			if curSlot == 2 then
				action:SetParamInt(6,1);
				action:SetParamFloat(7,accumTime);
				return true;
			end
		end
		
		if bKeyCheck == false then
			if curSlot == 0 then
				return	false;
			end
		
			-- 키를 텠고, 캐스팅이 끝났다면 챠지 어택 시전.
			if curSlot == 1 and action:GetParamInt(5) == 1 then
				Skill_Magician_Charge_Laser_Fire(actor,action);
			else
				return false
			end	
		end
		
	else
	
		local	fElapsedTime = accumTime - action:GetParamFloat(7);
		if fElapsedTime>=g_fChargingFreezeTime then
			action:SetParam(2,"end");
			return	false;					
		end		
		
	
	end

	return true
end

function Skill_Magician_Charge_Laser_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(9);
	actor:DetachFrom(11);
	actor:DetachFrom(12);
	
	actor:EndWeaponTrail();
	return true;
end


function Skill_Magician_Charge_Laser_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetID() == "a_jump" or action:GetID() == "a_MagicianShot_01" then
	
		if actor:GetAction():GetParam(10) == "HIT" then
			return true;
		end
	
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
		
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(1) == 1 and curAction:GetParam(2)~="end" then
		return false;
	end

	return true;
end


function ASkill_Magician_Charge_Laser_DoDamage(actor, action,fAttackHoldingTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iTargetCount = action:GetTargetCount();

	if 	iTargetCount == 0 then
		return true;
	end

	local	i = 0;
		
	if fAttackHoldingTime == nil or fAttackHoldingTime == 0 then
		fAttackHoldingTime = g_fAttackHitHoldingTime;
	end
	while i<iTargetCount do
		
		local	kTargetGUID = action:GetTargetGUID(i);
		local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			
			local	actorTarget = kTargetPilot:GetActor();
			if actorTarget:IsNil() == false and actorTarget:GetPilotGuid() ~= actor:GetPilotGuid() then
			
				local kActionResult = action:GetTargetActionResult(i);
--				local	kActionTargetInfo = action:GetTargetInfo(i);	
				
				local	iSphereIndex = action:GetTargetABVShapeIndex(i);
				local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);		

				actorTarget:AttachParticleToPoint(12, pt, "ef_mag_gathering_02")

--				local	atAction = actorTarget:GetAction()
				
--				local	guidPilot = actorTarget:GetPilotGuid()
--				local	pilot = g_pilotMan:FindPilot(guidPilot)
				
				local iHoldingTime = fAttackHoldingTime * 1000;
				local	iShakeTime = g_fAttackHitHoldingTime * 1000;
				
				-- 충격 효과 적용
				actor:SetAnimSpeedInPeriod(0.00001,iHoldingTime);
				actor:SetShakeInPeriod(5,iShakeTime/2);
				actorTarget:SetShakeInPeriod(5,iShakeTime);		
				
				-- 피격 소리 재생
				local kTargetAction = actor:GetAction()
				if kTargetAction ~= nil and kTargetAction:IsNil() == false then
					local actionName = kTargetAction:GetID()
					if kActionResult:GetCritical() then
						actionName = "Critical_dmg"
					end
					actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
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
function Skill_Magician_Charge_Laser_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetParamInt(1) ~= 1 then
		return true;
	end
	
	
	if textKey == "hit" or textKey == "fire" then
	
		action:SetParam(10,"HIT");
		
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end		
	
		ASkill_Magician_Charge_Laser_DoDamage(actor,actor:GetAction())
		
	end
	
	return	true;
end
