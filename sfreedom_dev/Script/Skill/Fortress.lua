-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [UPGRADE_PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function Fortress_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Fortress_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    ODS("Fortress_OnCastingCompleted\n");
    
	Fortress_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	Fortress_LoadToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");

	local iUpgrade = action:GetParamInt(100);
	if 1 == iUpgrade then
		kProjectileID = action:GetScriptParam("UPGRADE_PROJECTILE_ID");
	end
	
	ODS("Fortress_LoadToWeapon kProjectileID:"..kProjectileID.."\n");
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	if actor:GetID() == "sa_auto_shoot_system" then
    	local   kPos = actor:GetNodeWorldPos("Siege_Cannon:0");
    	kNewArrow:LoadToPosition(kPos);
	else
    	kNewArrow:LoadToHelper(actor,"Bip01 Head");	--	장전
	end
	return kNewArrow;

end

function Fortress_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    ODS("Fortress_Fire\n");
	
	actor:ResetAnimation();

	if actor:IsMyActor() == true then
		local iUpgrade = 0
		local iProjectileSpeedRate = 0;
		local	kSkillDef = GetSkillDef(action:GetActionNo());
		if kSkillDef:IsNil() == false then
			iProjectileSpeedRate = kSkillDef:GetAbil(AT_PROJECTILE_SPEED_RATE);
			local iNeedBaseSkill = kSkillDef:GetAbil(AT_CHILD_SKILL_NUM_01);
			local iUpgradeSkill = actor:CheckSkillExist(iNeedBaseSkill);

			if 0 ~= iUpgradeSkill then
				iUpgrade = 1;
			end

			iNeedBaseSkill = kSkillDef:GetAbil(AT_CHILD_SKILL_NUM_02);
			iUpgradeSkill = actor:CheckSkillExist(iNeedBaseSkill);

			if 0 ~= iUpgradeSkill then
				local	kSkillDef2 = GetSkillDef(iUpgradeSkill);
				if kSkillDef2:IsNil() == false then					
					iProjectileSpeedRate = iProjectileSpeedRate + kSkillDef2:GetAbil(AT_PROJECTILE_SPEED_RATE);
				end
			end
		end

		action:SetParamInt(100,iUpgrade);
		action:SetParamInt(101,iProjectileSpeedRate);
	else
		local kPacket = action:GetParamAsPacket();
		local iValue = kPacket:PopInt();
		action:SetParamInt(100, iValue);
		iValue = kPacket:PopInt();
		action:SetParamInt(101,iValue);
	end	
end

function Fortress_OnOverridePacket(actor,action,packet)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    if action:GetActionParam() == AP_FIRE then
		local iValue = action:GetParamInt(100);		
		packet:PushInt(iValue);
		iValue = action:GetParamInt(101);
		packet:PushInt(iValue);
    end    
end


function Fortress_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--	먼저 가장 앞에 있는 한놈을 찾는다.
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),50,0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iFoundCount = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);		
	
	if iFoundCount == 0 then
		return	iFoundCount;
	end
	
	--	찾은 녀석을 중심으로 나머지 타겟을 찾는다.
	local	kTargetInfo = kTargets:GetTargetInfo(0);
	local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());			
	
	kParam:SetParam_1(kTargetPilot:GetActor():GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,g_fFortress_Blow_Range,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);		

end

function Fortress_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	actor:ResetAnimation();

	Fortress_OnCastingCompleted(actor,action);

	local iUpgrade = action:GetParamInt(100);		
	local iInstanceID = 0;
	if 1 == iUpgrade then
		iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_fortress_head_powerUp",0,1);	
	else
		iInstanceID = GetStatusEffectMan():AddStatusEffectToActor(actor:GetPilotGuid(),"se_fortress_head",0,1);
	end
	
	action:SetParamInt(2,iInstanceID);	
	if(0 == Config_GetValue(HEADKEY_ETC, SUBKEY_DISPLAY_HELMET)) then -- 투구 보이기 옵션이 꺼져있을때
		if actor:CheckStatusEffectExist("se_hide_face") then	-- 얼굴이 감추어져있다면
			actor:HideParts(EQUIP_POS_FACE, false);	-- 얼굴을 보이게 한다 
		end
	end
	return true
end

function Fortress_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	
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

function Fortress_OnCleanUp(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kAction = actor:GetAction();
	local	iInstanceID = kAction:GetParamInt(2);
	
	GetStatusEffectMan():RemoveStatusEffectFromActor2(actor:GetPilotGuid(),iInstanceID);
	if(0 == Config_GetValue(HEADKEY_ETC, SUBKEY_DISPLAY_HELMET)) then -- 투구 보이기 옵션이 꺼져있을때
		if actor:CheckStatusEffectExist("se_hide_face") then	-- 얼굴이 감추어져 있는 이펙트가 존재한다면
			actor:HideParts(EQUIP_POS_FACE, true);	-- 얼굴을 감춘다
		end
	end
	return true;
end

function Fortress_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
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
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
		
	if action:GetEnable() == true then
	

		if curAction:GetScriptParam("NO_AUTO_CONNECT") ~= "TRUE" then
		
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
			
		end

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Fortress_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Fortress_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		ODS("Fortress_OnEvent iAttackRange:"..iAttackRange.."\n");
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		actor:AttachParticle(378,"char_root","ef_mag_cannon_char_root");
		actor:AttachParticle(379,"char_root","ef_mag_cannon_01_char_root");		
		
		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Fortress_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			ODS("kMovingDir:"..kMovingDir:GetX()..","..kMovingDir:GetY()..","..kMovingDir:GetZ().."\n");
			
			if kMovingDir:IsZero() then
			    kMovingDir:SetX(1);
			end
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
				
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
				
			
			kArrow:Fire();
			local iSpeedRate = kAction:GetParamInt(101);
			iSpeedRate = iSpeedRate / ABILITY_RATE_VALUE;
			if 0 < iSpeedRate then
				kArrow:SetSpeed(kArrow:GetSpeed() * iSpeedRate);
			end

			ODS("Fire\n");
			
		else
			
			ODS("Arrow is nil\n");
		
		end
	end

	return	true;
end
