function Skill_Multiple_Shot_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());	

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function Skill_Multiple_Shot_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Multiple_Shot_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+3);
	actor:PlayCurrentSlot();

end

function Skill_Multiple_Shot_Fire(actor,action)
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(5,3)	
end


function Skill_Multiple_Shot_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	if action:GetID() ~= "a_Throwing Dagger" and 
	action:GetID() ~= "a_dragon_tooth" then
		actor:AttachParticle(7215,"char_root","ef_Multishot_01_char_root");
		actor:AttachSound(200,"MultiShot_Start");
	end

	MagMultiShot_OnCastingCompleted(actor,action);

	if actor:IsMyActor() then
		local	iSkillLevel = action:GetAbil(AT_LEVEL);

		if iSkillLevel == nil then
			iSkillLevel = 5;
		end

		local	iFireCount = 3;

		if iSkillLevel >= 5 then
			iFireCount = 7;
		elseif iSkillLevel >= 3 then
			iFireCount = 5;
		else
			iFireCount = 3;
		end

		if action:GetID() == "a_Throwing Dagger" then
			iFireCount = 1;
			local	kSkillDef = GetSkillDef(action:GetActionNo());
			if kSkillDef:IsNil() == false then
				local iNeedBaseSkill = kSkillDef:GetAbil(AT_CHILD_SKILL_NUM_01);
				local iUpgradeSkill = actor:CheckSkillExist(iNeedBaseSkill);
				
				if 0 ~= iUpgradeSkill then
					local kChildSkillDef = GetSkillDef(iUpgradeSkill);
					if kChildSkillDef:IsNil() == false then
						local iChildSkillLevel = kChildSkillDef:GetAbil(AT_LEVEL);

						if iChildSkillLevel >= 5 then
							iFireCount = 7;
						elseif iChildSkillLevel >= 3 then
							iFireCount = 5;
						else
							iFireCount = 3;
						end						
					end			
				end
			end
		elseif action:GetID() == "a_dragon_tooth" then
			iFireCount = 1;
			local	kSkillDef = GetSkillDef(action:GetActionNo());
			iFireCount = kSkillDef:GetAbil(AT_LEVEL);
			--ODS("iFireCount : "..iFireCount.."\n", false, 3851)
		end
		
		action:SetParamInt(100,iFireCount);
	else
		local kPacket = action:GetParamAsPacket();
		local iFireCount = kPacket:PopInt();
		action:SetParamInt(100,iFireCount);

	end
	
	actor:StopJump();
	
	return true
end

function Skill_Multiple_Shot_OnOverridePacket(actor,action,packet)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetActionParam() == AP_FIRE then
		local iFireCount = action:GetParamInt(100);
		packet:PushInt(iFireCount);
    end    
end

function Skill_Multiple_Shot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();
	if action:GetParam(300) == "finish" then
		return	false;
	end
	if actor:IsMyActor() and action:GetParamInt(6) == 0 then
		if KeyIsDown(action:GetActionKeyNo(0)) == false then
			action:SetParamInt(6,3);
		end
	end

	if animDone == true then
		
--		local	curActionSlot = action:GetCurrentSlot()
		local	curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(3);

			action:SetParam(0, "null")
			action:SetParam(3, "end")
			action:SetNextActionName(nextActionName)
			return false;
		else

			action:SetParam(3, "end")

			return false
		end
	end

	return true
end

function Skill_Multiple_Shot_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(7215,true)
	return true;
end

function Skill_Multiple_Shot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
		local	curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());	
	
	local	curParam = curAction:GetParam(3)
	local	actionID = action:GetID()

	if actor:IsMyActor() == false then
		return true;
	end
	if action:GetActionType()=="EFFECT" or curAction:GetParam(300)=="finish" then
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
		curAction:SetParam(3,actionID);
		
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


function Skill_Multiple_Shot_Fire7(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	kProjectileMan = GetProjectileMan();
	
	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 12.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<7 do
	
		local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-3));
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

		i=i+1;
	end
	
end

function Skill_Multiple_Shot_Fire5(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 12.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<5 do
	
		local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-2));
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

		i=i+1;
	end	
end

function Skill_Multiple_Shot_Fire4(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 12.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<4 do
	
		local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-1.5));
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

		i=i+1;
	end	
end

function Skill_Multiple_Shot_Fire3(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 12.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<3 do
	
		local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-1));
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

		i=i+1;
	end	
end

function Skill_Multiple_Shot_Fire2(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	

	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 12.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<2 do
	
		local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-0.5));
		kFireTarget:Add(arrow_worldpos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!

		i=i+1;
	end	
end

function Skill_Multiple_Shot_Fire1(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	-- 회전축 구하기
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	local	kArrow = Projectile_Default_LoadToWeapon(actor,action);
	local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
	local	kFireTarget = actor:GetLookingDir()
	kFireTarget:Multiply(iAttackRange-30);
	kFireTarget:Add(arrow_worldpos);			
	kArrow:SetTargetLoc(kFireTarget);	
	kArrow:Fire();--	발사!
end


function Skill_Multiple_Shot_OnEvent(actor, textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if textKey == "fire" or textKey=="hit" or textKey == "end" then
	
		if actor:GetAction():GetParam(7) == "FIRED" then
			return true
		end
	
		local	action = actor:GetAction()
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(3784,kSoundID);
		end

		if actor:GetPilot():GetBaseClassID() == CT_SHAMAN then
			actor:AttachParticleS(3851, "p_ef_fire", "eff_sum_skill_smn_muzz00", 1.0)
		end
		

		local iFireCount = action:GetParamInt(100);
		if iFireCount == 7 then
			Skill_Multiple_Shot_Fire7(actor,action);
		elseif iFireCount == 5 then
			Skill_Multiple_Shot_Fire5(actor,action);
		elseif iFireCount == 4 then
			Skill_Multiple_Shot_Fire4(actor,action);
		elseif iFireCount == 3 then
			Skill_Multiple_Shot_Fire3(actor,action);
		elseif iFireCount == 2 then
			Skill_Multiple_Shot_Fire2(actor,action);
		elseif iFireCount == 1 then
			Skill_Multiple_Shot_Fire1(actor,action);
		end		
		
		actor:GetAction():SetParam(7,"FIRED");

	end
end