-- Melee
function Skill_BrotherIsGone_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_BrotherIsGone_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_BrotherIsGone_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_BrotherIsGone_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_BrotherIsGone_Fire\n");
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
	end

end

function Skill_BrotherIsGone_FindGenPoint(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	싱하형을 생성할 좌표를 찾아보자.
	
	local	iRange = action:GetSkillRange(0,actor);
	ODS("Skill_BrotherIsGone_FindGenPoint iRange:"..iRange.."\n");
	
	local	fDistance = iRange - 70;
	local	kTargetPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(fDistance);
	kTargetPos:Add(kMovingDir);
	
	
	return	kTargetPos;

end
function Skill_BrotherIsGone_CreateSinghaBro(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 900004, 0)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 9)
		pilot:GetActor():FreeMove(true)
		pilot:GetActor():ToLeft(actor:IsToLeft());
		pilot:GetActor():ReserveTransitAction("a_Brother is Gone_Play")
		pilot:GetActor():ClearActionState()
		
		local kAction = pilot:GetActor():GetReservedTransitAction();
		if kAction:IsNil() == false then
		
			kAction:SetTargetList(action:GetTargetList());
			kAction:AddNewGUID(actor:GetPilotGuid());
			kAction:SetParamInt(10,action:GetSkillRange(0,actor));
			if actor:IsMyActor() then
				kAction:SetParamInt(3,1);
			end
			
			local	iSkillLevel = action:GetAbil(AT_LEVEL);
			if 0 == iSkillLevel then
				iSkillLevel = 1;
			end

			kAction:SetParamInt(16, iSkillLevel);

			action:ClearTargetList();
		
		end
	end	

end

function Skill_BrotherIsGone_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local	iRange = action:GetSkillRange(0,actor);
	ODS("Skill_BrotherIsGone_OnFindTarget iRange:"..iRange.."\n");
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iRange,40,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_BrotherIsGone_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_BrotherIsGone_OnCastingCompleted(actor,action)
	
	return true
end

function Skill_BrotherIsGone_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
	
	
		local curActionParam = action:GetParam(0)
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

function Skill_BrotherIsGone_OnCleanUp(actor,action)
	return true;
end

function Skill_BrotherIsGone_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_BrotherIsGone_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
			
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" then

		-- 싱하형 만들자
			
		local	kTargetPos = Skill_BrotherIsGone_FindGenPoint(actor,kAction);
		Skill_BrotherIsGone_CreateSinghaBro(actor,kAction,kTargetPos);
		actor:ClearIgnoreEffectList();
			
			
	end

	return	true;
end
