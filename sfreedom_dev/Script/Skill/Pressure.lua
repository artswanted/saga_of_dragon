-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID

-- Melee
function Skill_Pressure_OnCheckCanEnter(actor,action)

    CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_Pressure_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	Skill_Pressure_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Pressure_Fire(actor,action)
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_Pressure_Fire\n");
	
	action:CreateActionTargetList(actor);
end

function Skill_Pressure_FindGenPoint(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local   fFrontDistance = action:GetSkillRange(0,actor);
	if fFrontDistance == 0 then
	    fFrontDistance = 94
	end

	ODS("fFrontDistance : "..fFrontDistance.."\n", false, 1509)
	
	local   fHeight = 150;

    local   kTargetPos = actor:GetLookingDir();
    kTargetPos:Multiply(fFrontDistance);
    kTargetPos:SetZ(kTargetPos:GetZ()+fHeight);
    kTargetPos:Add(actor:GetPos());
	
	return	kTargetPos;

end
function Skill_Pressure_CreateSkillActor(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123")
	guid:Generate()
	
	ODS("MyPos :"..actor:GetPos():GetX()..","..actor:GetPos():GetY()..","..actor:GetPos():GetZ().."\n");
	ODS("TargetPos :"..kTargetPos:GetX()..","..kTargetPos:GetY()..","..kTargetPos:GetZ().."\n");
	
	local pilot = g_pilotMan:NewPilot(guid, 900010, 0)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 9)
		pilot:GetActor():FreeMove(true)
		pilot:GetActor():ReserveTransitAction("a_Pressure_Play")
		pilot:GetActor():ClearActionState()
		pilot:GetActor():SeeFront(true,true);
		
		local kAction = pilot:GetActor():GetReservedTransitAction();
		if kAction:IsNil() == false then
		
		    GetActionTargetTransferInfoMan():AddTransferInfo_Actor(
		                    actor:GetPilotGuid(),
		                    action:GetActionNo(),
		                    action:GetActionInstanceID(),
		                    pilot:GetGuid());
		                    
			kAction:SetTargetList(action:GetTargetList());
			kAction:AddNewGUID(actor:GetPilotGuid());
			kAction:SetParamFloat(1,kTargetPos:GetZ());

			local	iSkillLevel = action:GetAbil(AT_LEVEL);
			if 0 == iSkillLevel then
				iSkillLevel = 1;
			end

			kAction:SetParamInt(16, iSkillLevel);
			
			action:ClearTargetList();
		
		end
	end	

end

function Skill_Pressure_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	local	kPos = Skill_Pressure_FindGenPoint(actor,action);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,Point3(0,0,-1));
	kParam:SetParam_2(0,0,50,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargets(TAT_RAY_DIR_COLL_PATH,kParam,kTargets,kTargets);
end

function Skill_Pressure_OnEnter(actor, action)

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

	Skill_Pressure_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_Pressure_OnUpdate(actor, accumTime, frameTime)
	
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

function Skill_Pressure_OnCleanUp(actor)
	return true;
end

function Skill_Pressure_OnLeave(actor, action)

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
	
	if action:GetActionType()=="EFFECT"  then
		
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

function Skill_Pressure_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" then
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end	

		local	kTargetPos = Skill_Pressure_FindGenPoint(actor,kAction  );
		Skill_Pressure_CreateSkillActor(actor,kAction,kTargetPos);		
	end
	
	return	true;
end
