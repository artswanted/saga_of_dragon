-- [FIRE_SOUND_ID] : 새가 하강하고 화면으로 날아올때의 사운드

function Skill_MultiStrape_Melee_OnCastingCompleted(kActor,kAction)
end

function Skill_MultiStrape_Melee_OnCheckCanEnter(kActor,kAction)

	ODS("Skill_MultiStrape_Melee_OnCheckCanEnter \n",false, 1509)
	return Skill_Master_Summon_Finish_OnCheckCanEnter(kActor,kAction)
end

function Skill_MultiStrape_Melee_OnTimer(kActor,fAccumTime,kAction,iTimerID)
	return Skill_Master_Summon_Finish__OnTimer(kActor,fAccumTime,kAction,iTimerID)
end

function Skill_MultiStrape_Melee_OnEnter(kActor, kAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil())
	kActor:FreeMove(false)
	kActor:StartWeaponTrail()
	return Skill_Master_Summon_Finish_OnEnter(kActor, kAction)
end

function Skill_MultiStrape_Melee_OnUpdate(kActor, accumTime, frameTime)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	kActor:FreeMove(false)
	return 	Skill_Master_Summon_Finish_OnUpdate(kActor, accumTime, frameTime)
end

function Skill_MultiStrape_Melee_OnCleanUp(kActor, kAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	Skill_Master_Summon_Common_OnCleanUp(kActor,kAction)
	
	return Skill_Master_Summon_Finish_OnCleanUp(kActor)
end

function Skill_MultiStrape_Melee_OnLeave(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	kActor:FreeMove(false)
	kActor:EndWeaponTrail()

	return Skill_Master_Summon_Finish_OnLeave(kActor, kAction)
end

function Skill_MultiStrape_Melee_OnEvent(kActor, strTextKey)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
		
	local kAction = kActor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if strTextKey == "fire" then	--발사 모션(fire메세지)에 화살을 쏘고
		QuakeCamera(1, 1, 1, 1, 1)
		local kArrow = Skill_MultiStrape_Melee_LoadArrowToWeapon(kActor, kAction)
		if (false == kArrow:IsNil()) then 
			kArrow:SetParentActionInfo(0, kAction:GetActionInstanceID(), kAction:GetTimeStamp());
			local	kYUnitVec = Point3(0,1,0)
			local kTargetPos = kActor:GetLookingDir()
			kTargetPos:Rotate(kYUnitVec, -90*DEGTORAD)

			local kDist = Point3(0,0,1)
			kDist:Multiply(270)
			kTargetPos:Add(kArrow:GetWorldPos())
			kTargetPos:Add(kDist)
			kArrow:SetTargetLoc(kTargetPos)
			kArrow:Fire(true)
		end
		
		-- 떨어지는 소환수동작을 부른다
		if "" ~= kAction:GetScriptParam("SUMMON_CLASSNO") then
			local iSummonClassNo = tonumber(kAction:GetScriptParam("SUMMON_CLASSNO"));
			local kSummonReserveAction = kAction:GetScriptParam("SUMMON_RESERVE_ACTION");
			Skill_Master_Summon_Common_Create_Summon(kActor,iSummonClassNo,kSummonReserveAction);
		end	
		local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID");
		if kSoundID~="" then
			kActor:AttachSound(2783,kSoundID);
		end
		
	elseif strTextKey == "hit" then	--hit 메세지에 실제 데미지를 준다
		QuakeCamera(1, 5, 1, 1, 2)
		-- 이펙트를 넣는다
		
		local iSkillRange = kAction:GetParamInt(21);
		local iIndex = kAction:GetParamInt(20);

		local kPos = kActor:GetPos();
		--[[
		kPos:SetX(kPos:GetX() + math.random(-iSkillRange,iSkillRange));
		kPos:SetY(kPos:GetY() + math.random(-iSkillRange,iSkillRange));
		kPos:SetZ(kPos:GetZ() + 200);
		]]

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			--위로도 쏴 본다.
			ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				return true;
			end			
		end
		ptcl:SetZ(ptcl:GetZ()-50)
		
		local fScale = 5
		kActor:AttachParticleToPointS(1000 + iIndex, ptcl, "ef_skill_dragon_rush_lightning_01", fScale);
		--kActor:AttachParticleToPointS(1000 + iIndex, kPos, "ef_skill_dragon_rush_lightning_01", fScale);
		
		kAction:SetParamInt(20, iIndex + 1);


		return Skill_Master_Summon_Finish_OnEvent(kActor, strTextKey)
	end
	
	return true
end

function Skill_MultiStrape_Melee_OnTargetListModified(kActor, kAction, bIsBefore)
	return Skill_Master_Summon_Finish_OnTargetListModified(kActor, kAction, bIsBefore)
end

function Skill_MultiStrape_Melee_LoadArrowToWeapon(kActor,kAction)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_MULTISTRAPE_LAST_ARROW", kAction, kActor:GetPilotGuid());
	
	-- 발사체가 타겟을 잡지 못하더라도 삭제되지 않게함(XML로 옮김)
	-- local kProjectileOption = kNewArrow:GetOptionInfo()
	-- if(false == kProjectileOption:IsNil()) then
		-- kProjectileOption:SetAutoRemoveIfNoneTarget(false)
		-- kNewArrow:SetOptionInfo(kProjectileOption)
	-- end
	
	kNewArrow:LoadToWeapon(kActor) -- 무기 위치에 화살을 생성함
	return	kNewArrow
end
