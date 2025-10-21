
-- 좌우로 여러발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_COUNT] : 몇발을 발사할 것인가(현재는 2,3만 지원함)

function MagMultiShot_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function MagMultiShot_OnCastingCompleted(actor, action)
	MagMultiShot_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+3);
	actor:PlayCurrentSlot();

end

function MagMultiShot_Fire(actor,action)
	action:SetParamInt(5,3)

end

function MagMultiShot_OnEnter(actor, action)
	if actor:IsMyActor() == true then
	
		action:SetParamInt(6,0);	--	공격키를 뗄 경우 3로 셋팅된다. 모아서 쏘기 액션 호출을 위한 체크
	

	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	MagMultiShot_OnCastingCompleted(actor,action);
	
	actor:StopJump();
	
	return true
end
function MagMultiShot_OnFindTarget(actor,action,kTargets)

	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
		iAttackRange = 350
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 30;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local	arrow_worldpos = actor:GetPos();--kNewArrow:GetWorldPos();	--	화살의 위치
	
-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1)
	local	fRotateAngle = 30.0*math.pi/180.0;

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,30,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);		

	local	kFireTarget = actor:GetLookingDir()

    --SetBreak();
	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(-1))
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	kFireTarget = actor:GetLookingDir()
	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(0));
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	kFireTarget = actor:GetLookingDir()
	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(1));
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
end

function MagMultiShot_FindTargets(actor,action)

	action:CreateActionTargetList(actor);
	
end

function MagMultiShot_LoadToWeapon(actor,action)

	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	local	kPilotGuid = actor:GetPilotGuid();
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid);
	kNewArrow:LoadToWeapon(actor);
	kNewArrow:SetScale(0.7);
	
	return	kNewArrow;
end
function MagMultiShot_LoadToPos(actor,action,pos)

	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	local	kPilotGuid = actor:GetPilotGuid();
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid);
	kNewArrow:LoadToPosition(pos);
	kNewArrow:SetScale(0.7);
	
	return	kNewArrow;
end
function MagMultiShot_OnUpdate(actor, accumTime, frameTime)	
	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	weapontype = actor:GetEquippedWeaponType();
	local	actionID = action:GetID();
	if action:GetParam(300) == "finish" then
		return	false;
	end
	if actor:IsMyActor() and action:GetParamInt(6) == 0 then
		if KeyIsDown(action:GetActionKeyNo(0)) == false then
			action:SetParamInt(6,3);
		end
	end

	if animDone == true then
		
		if actor:IsMyActor() and actor:GetAction():GetParamInt(6) == 0 then	--	액션키를 계속 누르고 있었다. 레이져 쏘기 액션으로 전이하자
			action:SetNextActionName("a_magician_charge_laser");
		end	
			
		local	curActionSlot = action:GetCurrentSlot()
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

	-- 무빙샷이 가능하도록
	local fMovingSpeed = 0;

	if IsSingleMode() == true then
		fMovingSpeed = 120
	else
	    fMovingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	end	

	local dir = actor:GetDirection()
	if dir ~= DIR_NONE then
		actor:Walk(dir, fMovingSpeed)
	end

	return true
end
function MagMultiShot_OnCleanUp(actor)
	return true;
end
function MagMultiShot_OnLeave(actor, action)
	local	curAction = actor:GetAction();
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

	if actionID == "a_MagicianShot_01" then 
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

function MagMultiShot_Fire3(actor,action)

	local	kProjectileMan = GetProjectileMan();
	
	-- 회전축 구하기
	local	kRotateAxis = Point3(0,0,1);
	local fRotateAngle = 30.0*(math.pi/180.0)
	local	i = 0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	while i<3 do
	
		local	kArrow = MagMultiShot_LoadToWeapon(actor,action);
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

function MagMultiShot_Fire2(actor,action)

	local	kProjectileMan = GetProjectileMan();
	
	local	kPathNormal = actor:GetLookingDir();
	kPathNormal:Cross(Point3(0,0,1));
	local	kCenterPos = actor:GetPos();

	local	fDistance = 10.0;
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	if iAttackRange == 0 then
		iAttackRange = 100
	end
	
	local	i=0
	
	while i<2 do
	
		local	kArrowPos = kCenterPos:_Add(kPathNormal:_Multiply(fDistance));

		local	kArrow = MagMultiShot_LoadToPos(actor,action,kArrowPos);

		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange-30);
		kFireTarget:Add(kArrowPos);			
		kArrow:SetTargetLoc(kFireTarget);	
		kArrow:Fire();--	발사!
		
		i=i+1;
		
		fDistance=fDistance*-1;

	end
	
end

function MagMultiShot_OnEvent(actor, textKey)

	if textKey == "fire" or textKey=="hit" or textKey == "end" then
	
		if actor:GetAction():GetParam(7) == "FIRED" then
			return true
		end
	
		local	action = actor:GetAction()
		
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(3784,kSoundID);
		end	

		actor:AttachParticle(433,"char_root","shot_rebound_01");
		
		local	iFireCount = tonumber(action:GetScriptParam("FIRE_COUNT"));
		if iFireCount == nil or iFireCount == 0 then
			iFireCount = 3
		end
		
		if iFireCount == 3 then
			MagMultiShot_Fire3(actor,action);
		elseif iFireCount == 2 then
			MagMultiShot_Fire2(actor,action);
		end
		
		
		actor:GetAction():SetParam(7,"FIRED");

	end
end

