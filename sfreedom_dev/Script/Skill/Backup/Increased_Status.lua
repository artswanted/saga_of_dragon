
function Skill_Increased_Status_OnCheckCanEnter(actor,action)

	if actor:IsMeetFloor() == false then	
		action:SetParam(100, "finish")
		return	false;
	end

	return		true;
	
end

function Skill_Increased_Status_OnCastingCompleted(actor, action)

	Skill_Increased_Status_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_Increased_Status_Fire(actor,action)
	Skill_Increased_Status_FindTargets(actor,action);
end


function Skill_Increased_Status_OnEnter(actor, action)
	if actor:IsMyActor() == true then
	
		if actor:IsMeetFloor() == false then
			action:SetParam(100,"finish");
			return	true;
		end
	
	end
	
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Increased_Status_OnEnter weapontype : "..weapontype.."\n");

	--Skill_Increased_Status_LoadToWeapon(actor,action);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Increased_Status_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_Increased_Status_OnFindTarget(actor,action,kTargets)
	local kParam = FindTargetParam();	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());	kParam:SetParam_2(action:GetSkillRange(0,actor),3,0,0);	kParam:SetParam_3(true,FTO_NORMAL);		return	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);end

function Skill_Increased_Status_FindTargets(actor,action)

	if actor:IsMyActor() == true then	-- 액터가 나면 나를 타겟잡아서 서버로 보내자.


		local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
		local	iFound = Skill_Increased_Status_OnFindTarget(actor,action,kTargets);
		if iFound > 0 then
			local kUnitArray = NewUNIT_PTR_ARRAY()
			kUnitArray:AddUnit(actor:GetPilot():GetUnit())	-- 나한테 쓰는거니까 내 유닛 넣기
			local kActionResult = NewActionResultVector()

			action:Call_Skill_Result_Func(actor:GetPilot(), kUnitArray, kActionResult);
			local kAResult = kActionResult:GetResult(actor:GetPilotGuid(), false)

			if kAResult:IsNil() == false and kAResult:GetInvalid() == false then
				action:AddTarget(actor:GetPilotGuid(),actor:GetHitObjectABVIndex(), kAResult)
			end
			DeleteActionResultVector(kActionResult)
			DeleteUNIT_PTR_ARRAY(kUnitArray)
		end
		DeleteActionTargetList(kTargets);

	else
		GetActionResultCS(action, actor)	
	end
end

function Skill_Increased_Status_OnUpdate(actor, accumTime, frameTime)
	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	weapontype = actor:GetEquippedWeaponType();
	local	actionID = action:GetID();
	if action:GetParam(100) == "finish" then
		return	false;
	end

	if animDone == true then
	
		local	curActionSlot = action:GetCurrentSlot()
		local	curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			action:SetNextActionName(nextActionName)
			return false;
		else

			action:SetParam(1, "end")

			return false
		end
	end

	return true
end
function Skill_Increased_Status_OnCleanUp(actor)

	actor:FreeMove(false);
	
	return true;
end
function Skill_Increased_Status_OnLeave(actor, action)
	local	curAction = actor:GetAction();
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()

	if actor:IsMyActor() == false then
		return true;
	end
	if action:GetActionType()=="EFFECT" or curAction:GetParam(100)=="finish" then
		return true;
	end	
	if actionID == "a_jump" then 
		return false;
	end	
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
	
		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);
		
		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Increased_Status_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		local	iAttackRange = action:GetSkillRange(0,actor);
		
		actor:AttachSound(7283,"IncStat");

		actor:FreeMove(false);	
		
		local	iTargetCount = action:GetTargetCount();
--		if iTargetCount > 0 then
--			actor:AttachParticle(90, "p_ef_heart", "ef_bang" )
--			actor:AttachParticle(99, "p_pt_hair", "ef_skill_increased_status_01" )
--			actor:HideParts(1, true)	--머리카락
--			actor:HideParts(2, true)	--얼굴
--			actor:HideParts(20, true)	--투구
--		end
	end
end
