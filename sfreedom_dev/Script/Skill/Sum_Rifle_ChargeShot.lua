function Sum_Rifle_ChargeShot_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
		return	false;
	end
	
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis")
	or actor:CheckStatusEffectExist("se_transform_to_Dancer")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Fig")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Mag")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Arc")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Thi")then
		return false
	end
	
	return		true;
	
end

function Sum_Rifle_ChargeShot_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(1,0);
	action:SetParamInt(5,0);			--캐스팅 완료 체크 파라미터. 초기화해주자.
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	action:SetAutoBroadCastWhenCastingCompleted(false);
	action:SetParamInt(3,0);
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_fChargeFocusFilterValue[2]
		, g_fChargeFocusFilterValue[3]
		, g_fChargeFocusFilterValue[4]
		,true
		,true)
	end
		
   return true
end

function Sum_Rifle_ChargeShot_OnCastingCompleted(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,1);
	
	if actor:IsMyActor() == false then
		Sum_Rifle_ChargeShot_Fire(actor,action);
	end
end

function Sum_Rifle_ChargeShot_Fire(actor,action)

	action:SetParamInt(1,1);
	actor:PlayNext();
	
	if actor:IsMyActor() == true then
		action:BroadCastTargetListModify(actor:GetPilot(), true);
		action:SetActionInstanceID();
		action:BroadCast(actor:GetPilot());
	end

end

function Sum_Rifle_ChargeShot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction();
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(3);
	
	local	bKeyCheck =  KeyIsDown( KEY_CHARGING_UKEY )
	
	if actor:IsMyActor() == false then
		bKeyCheck = true;
	end
			
	if iState == 0 then
		if animDone == true then
			if curSlot == 0 then
				actor:PlayNext();
				local	kActorID = actor:GetID();
				actor:AttachParticle(10,"p_ef_fire","eff_charge_00");
			end
			if curSlot == 2 then
				action:SetParamInt(3,1);
				action:SetParamFloat(4,accumTime);
				return true;
			end
		end
		
		if bKeyCheck == false then
			if curSlot == 0 then
				return	false;
			end
			if curSlot == 1 and action:GetParamInt(5) == 1 then
				actor:DetachFrom(10,false);
				Sum_Rifle_ChargeShot_Fire(actor,action);
			else
				return	false;
			end	
		end
	else
	
		--local	fElapsedTime = accumTime - action:GetParamFloat(4);
		--if fElapsedTime>=g_fChargingFreezeTime then
			action:SetParam(2,"end");
			return	false;
		--end				
	
	end

	return true
end

function Sum_Rifle_ChargeShot_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(10);
	actor:EndWeaponTrail();
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
		, g_world:GetWorldFocusFilterAlpha()
		, 0
		, g_fChargeFilterReturnTime
		,false
		,true);
	end
	
	return true;
end


function Sum_Rifle_ChargeShot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetID() == "a_jump" then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	curAction = actor:GetAction();
	local 	curParam1 = curAction:GetParam(2)
	if curAction:GetParamInt(1) == 1 and curAction:GetParam(2)~="end" then
		return	false;
	end

   return true;
   
end

function Sum_Rifle_ChargeShot_FindTarget(kActor,kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Sum_Rifle_Shot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function Sum_Rifle_ChargeShot_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();
	
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end
function Sum_Rifle_ChargeShot_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Sum_Rifle_ChargeShot_OnTargetListModified\n", false, 3851)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end

function Sum_Rifle_ChargeShot_LoadArrowToWeapon(kActor,kAction)
	
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	if kAction:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = kAction:GetScriptParam("PROJECTILE_ID");
	end
	
	if kProjectileID == "" then
		kProjectileID = kActor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,kAction,kActor:GetPilotGuid());
		
	local	kTargetNodeID = kAction:GetScriptParam("ATTACH_NODE_ID");
	kNewArrow:LoadToHelper(kActor,kTargetNodeID);	--	장전
	
	return	kNewArrow;
end

function Sum_Rifle_ChargeShot_OnEvent(kActor,textKey,seqID)
		
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	local kAction = kActor:GetAction();
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
		
	if textKey == "hit" or textKey == "fire" then
		ODS("Fire\n", false, 3851)
		kActor:AttachParticleS(3851, "p_ef_fire", "eff_sum_skill_smn_muzz00", 1.0)
		
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			kActor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,kActor);		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		local	kArrow = Sum_Rifle_ChargeShot_LoadArrowToWeapon(kActor,kAction);
		
		if kArrow:IsNil() == false then
					
			local	kMovingDir = kActor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
		
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			local	kPosition = kActor:GetNodeWorldPos("p_ef_fire");
			kArrow:SetWorldPos(kPosition);
			
			kArrow:Fire();
			ODS("kArrow:Fire() \n", false, 3851)
		end
					
	end
end
