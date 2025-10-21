-- Action Key
function Act_Action_OnCheckCanEnter(actor,action)

	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	--	현재 상태가 IDLE 또는 달리기,걷기 상태일때에만 가능하다.
	local kCurAction = actor:GetAction();
	if( false == kCurAction:IsNil() ) then
		local	kCurrentActionID = kCurAction:GetID();
		if actor:IsMeetFloor() == false 
			and "a_telejump" ~= kCurrentActionID
		then
			return  false
		end
	end
	if actor:GetPilot():IsNil() then return false end
	
	local iBaseClassID = actor:GetPilot():GetBaseClassID();
	if iBaseClassID == CT_FIGHTER and actor:IsMyActor() then
		if false==kCurAction:IsNil() then
			local kID = kCurAction:GetID()
			local kSlotNo = kCurAction:GetCurrentSlot()
			if kID=="a_DemolitionCharge" then
				local guid = GUID(kCurAction:GetParam(123))
				local Death = g_pilotMan:FindPilot(guid)	--키반응으로 액션이 끊겼을 경우 엔티티는 끝나는 액션
				if (0==kSlotNo or 3==kSlotNo) then
					return false
				end
			end
		end
	end
	
	return	true;
end

function Act_Action_OnEnter(actor, action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);	
	CheckNil(action:IsNil());
	local curAction = actor:GetAction()
	if( curAction:IsNil() ) then
		return false;
	end
	
	if actor:IsRidingPet() then --펫에 탑승 중이라면 탑승용 액션으로 유지
		action:SetSlot(1)
		actor:PlayCurrentSlot()
	end
	
	action:SetDoNotBroadCast(true);
	
	-- 실행 가능한 트리거가 있는지 체크해보자.
	local	kTrigger = actor:GetCurrentTrigger();
	if kTrigger:IsNil() == false then
		local	kConditionAction = kTrigger:GetConditionAction();
		if kConditionAction == action:GetID() and 
			kTrigger:GetConditionType() == CT_ACTION then
			if(kTrigger:GetTriggerType() == TRIGGER_TYPE_JOB_SKILL) 
			   and not (IsJobSkill( actor:GetAction() ) ) then -- 채집 트리거라면, 코드에서 말고 여기에서 요청을 보내준다
				kTrigger:OnAction(actor)
			end
			return true
		end
	end
	
	if actor:IsRidingPet() then --실행 가능한 트리거가 없고 펫에 탑승 중이면 차저로 전이시키지 않고 현재 액션을 취소시킨다.
		return false
	end
	--	실행 가능한 트리거가 없다.
	--	이때에는 각 클래스별 차징 공격을 시작한다.
		
	if actor:CheckStatusEffectExist("se_transform_to_metamorphosis") 
	or actor:CheckStatusEffectExist("se_transform_to_Dancer")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Fig")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Mag")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Arc")
	or actor:CheckStatusEffectExist("se_transform_to_mahalka_Thi")then
		action:SetNextActionName("a_idle");
		action:ChangeToNextActionOnNextUpdate(true,true);  
		return true
	end

	local	iBaseClassID = actor:GetPilot():GetBaseClassID();
	local iNewRaceBaseID = 50
		
	local kTransitID = "a_idle"
	if ( true == actor:CheckEffectExist(99803795, false) )then 
		kTransitID = "a_bear_emergency_escape" -- 러브모드 곰 긴급회피 스킬
	elseif iBaseClassID == CT_MAGICIAN then
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			local iWeaponType = actor:GetEquippedWeaponType();
			if 3 == iWeaponType then
				action:SetNextActionName("a_st_combo_z_1");
				action:ChangeToNextActionOnNextUpdate(true,true); 
				return true;
			elseif 4 == iWeaponType then
				action:SetNextActionName("a_sp_combo_z_1");
				action:ChangeToNextActionOnNextUpdate(true,true); 
				return true;
			end
		end
	elseif iBaseClassID == CT_SHAMAN then
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			action:SetNextActionName("a_gs_combo_z_1");
			action:ChangeToNextActionOnNextUpdate(true,true); 
			return true;
		end
	end
	
	if iBaseClassID == CT_FIGHTER and actor:IsMyActor() then
		if false==curAction:IsNil() then
			local kID = curAction:GetID()
			local kSlotNo = curAction:GetCurrentSlot()
			if kID=="a_DemolitionCharge" then
				local guid = GUID(curAction:GetParam(123))
				local Death = g_pilotMan:FindPilot(guid)	--키반응으로 액션이 끊겼을 경우 엔티티는 끝나는 액션
				if (1==kSlotNo or 2==kSlotNo) then
					if false==Death:IsNil() and false==Death:GetActor():IsNil() then
						Death:GetActor():GetAction():SetSlot(3)
						Death:GetActor():PlayCurrentSlot()
						kTransitID = "a_DemolitionCharge"..kSlotNo
					end
				elseif 0==kSlotNo then
					if nil~= g_world then
						g_world:RemoveActorOnNextUpdate(guid)
					end
				elseif 3==kSlotNo then
					return true
				end
			end
		end
	end
	
	--ODS("Act_Action_OnEnter - "..kTransitID.."\n", false, 3851)
		
	action:SetNextActionName(kTransitID);
	action:ChangeToNextActionOnNextUpdate(true,true);
	
	return true
end

function Act_Action_OnUpdate(actor, accumTime, frameTime)
	return false
end

function Act_Action_OnCleanUp(actor, action)
	return true
end

function Act_Action_OnLeave(actor, action)
	return true
end