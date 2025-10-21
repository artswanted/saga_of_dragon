-- Melee
function Skill_Blitz_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_Blitz_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Blitz_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Blitz_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:CreateActionTargetList(actor);
	
end

function Skill_Blitz_FindGenPoint(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	--	싱하형을 생성할 좌표를 찾아보자.
	local	fDistance = -80;
	local	kTargetPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(fDistance);
	kTargetPos:Add(kMovingDir);
	
	
	return	kTargetPos;

end
function Skill_Blitz_CreateWolf(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 900008, 0)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 9)
		
		--pilot:GetActor():ToLeft(actor:IsToLeft(),true);
		pilot:GetActor():ReserveTransitAction("a_Blitz_Play")
		pilot:GetActor():FreeMove(true)
		local   kLookTarget = actor:GetLookingDir();
		kLookTarget:Multiply(40);
		kLookTarget:Add(kTargetPos);
		pilot:GetActor():LookAt(kLookTarget,true);
		
		local kAction = pilot:GetActor():GetReservedTransitAction();
		if kAction:IsNil() == false then
		
		    GetActionTargetTransferInfoMan():AddTransferInfo_Actor(
		                    actor:GetPilotGuid(),
		                    action:GetActionNo(),
		                    action:GetActionInstanceID(),
		                    pilot:GetGuid());
		                    
            --SetBreak();
			kAction:SetTargetList(action:GetTargetList());
			kAction:AddNewGUID(actor:GetPilotGuid());
			
			local kLookDir = actor:GetLookingDir();
			
			kAction:SetParamInt(10,action:GetSkillRange(0,actor));
			kAction:SetParamFloat(11,kLookDir:GetX());
			kAction:SetParamFloat(12,kLookDir:GetY());
			kAction:SetParamFloat(13,kLookDir:GetZ());

			local	kTargetPos = kLookDir:_Multiply(action:GetSkillRange(0,actor));
			kTargetPos:Add(actor:GetPos());

			kAction:SetParamFloat(30,kTargetPos:GetX());
			kAction:SetParamFloat(31,kTargetPos:GetY());
			kAction:SetParamFloat(32,kTargetPos:GetZ());
			
			action:ClearTargetList();
		
		end
	end	

end

function Skill_Blitz_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	kDir2 = Point3(kDir:GetX(),kDir:GetY(),kDir:GetZ());

	kDir2:Multiply(action:GetSkillRange(0,actor));
	kPos:Add(kDir2);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Blitz_OnEnter(actor, action)

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

	Skill_Blitz_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_Blitz_OnUpdate(actor, accumTime, frameTime)
	
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

function Skill_Blitz_OnCleanUp(actor)
	return true;
end

function Skill_Blitz_OnLeave(actor, action)
	
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

function Skill_Blitz_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
	    if kAction:GetParamInt(2) == 0 then
		    local	kTargetPos = Skill_Blitz_FindGenPoint(actor,kAction);
		    Skill_Blitz_CreateWolf(actor,kAction,kTargetPos);	
		    actor:ClearIgnoreEffectList();
    		
		    kAction:SetParamInt(2,1);

			local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
			if kSoundID~="" then
				actor:AttachSound(2784,kSoundID);
			end
	    end	
	
	end
	
	return	true;
end
