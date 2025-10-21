------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 13[float] : fire 시간
-- 14[float] : 폭파시간, 폭파하고 약간의 텀을 두기 위해

-- Param Point
-- 101 : 시전자 초기 위치

function Skill_Striker_SelfBomber_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	-- if actor:IsMeetFloor() == true then
		-- return	false;
	-- end

	return true;

end

function Skill_Striker_SelfBomber_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Striker_SelfBomber_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == true then		-- 지상에서는 사용할 수 없다.
			return	false;
		end
	end
	
	action:SetParamInt(10,0)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamFloat(14,0);
	action:SetParamAsPoint(101,actor:GetPos());
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
	actor:StopJump();
	actor:FreeMove(true);
	Skill_Striker_SelfBomber_Fire(actor,action);

	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
    return true
end

function Skill_Striker_SelfBomber_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
	
	local   kMan = GetStatusEffectMan();
	kMan:AddStatusEffectToActor(actor:GetPilotGuid(),"se_transform_to_selfbomber",0,0);
	
	action:SetSlot(1);
	actor:PlayCurrentSlot();
end

function Skill_Striker_SelfBomber_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(10);
	
	actor:StopJump();		-- 다른 액션의 Leave를 실행할때 StopJump를 풀어버리는 경우가 있음
	actor:FreeMove(true);	
	
	if iState == 0 then
		if animDone == true then
			if curSlot == 1 then
				action:SetSlot(2);
				actor:PlayCurrentSlot();
				action:SetParamFloat(13, accumTime);
				if 1 == actor:GetAbil(AT_GENDER) then
					actor:AttachSound(2785,"voice_male3");
				--여성용 사운드
				else
					actor:AttachSound(2785,"voice_female5");
				end
				if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
					actor:StartWeaponTrail();
				end
			end
		end
		
		if curSlot == 1 then
			actor:SetTranslate(action:GetParamAsPoint(101));
		elseif curSlot == 2 then
			local bombTime = action:GetParamFloat(14);
			if actor:IsMeetFloor() == true and bombTime == 0 then
				Skill_Striker_SelfBomber_HitBomb(actor);
				action:SetParamFloat(14,accumTime);
			elseif bombTime == 0 then
				local kMovingVec = actor:GetLookingDir();
				kMovingVec:SetZ(-1);
				kMovingVec:Multiply(400);
				actor:SetMovingDelta(kMovingVec);
			elseif accumTime - bombTime > 0.2 then
				action:SetParamInt(10,1);
			end
		end
	else
		return false;
	end

	return true;
end

function Skill_Striker_SelfBomber_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	actor:DetachFrom(201);

	if actor:GetAction():GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
	local   kMan = GetStatusEffectMan();
	kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),0);

	actor:FreeMove(false);
	return true;
end


function Skill_Striker_SelfBomber_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local actionID = action:GetID()

	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	

	local	curAction = actor:GetAction();
	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_Striker_SelfBomber_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	true;
end

function Skill_Striker_SelfBomber_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action)
	end
end

function Skill_Striker_SelfBomber_HitBomb(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
		
	if actor:IsMyActor() then
		kAction:CreateActionTargetList(actor);
		kAction:BroadCastTargetListModify(actor:GetPilot());
		kAction:ClearTargetList();
	end

	actor:AttachParticleToPointS(201,actor:GetPos(),"ef_boom_01", 1.15);
	actor:AttachSound(2790,"SelfBomber_Bomb");
	QuakeCamera(0.4, 3)
	
 	if IsSingleMode() then
    	Skill_WarLord_SoulDrain_OnTargetListModified(actor,kAction,false);
	end
end


