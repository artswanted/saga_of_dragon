-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
function Skill_Ice_Shower_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	if weapontype == 0 then
		return false
	end

	return actor:IsMeetFloor()
	
end
function Skill_Ice_Shower_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(1,0)	--스킬 엔티티가 외부에서 넣어줄 파람. 초기화

	actor:StopJump()
	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000652, 0) -- sa_iceshower
	if false == pilot:IsNil() then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		local kBird = pilot:GetActor()
		if false == kBird:IsNil() then
			kBird:FreeMove(true)
			local kTrans = actor:GetTranslate()
			kTrans:SetZ(kTrans:GetZ()+5)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AddActor(guid, kBird, kTrans, 9);
			pilot:GetUnit():SetCaller(actor:GetPilotGuid())
			local	kLookAt = actor:GetLookingDir()
			kBird:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
			kBird:ReserveTransitAction("a_IceShowerSA")
			pilot:GetActor():ClearActionState();
			if actor:IsMyActor() then
	        	pilot:GetActor():SetUnderMyControl(true);
		    end

			local fScale = (action:GetSkillLevel()%10)*0.05 + 1.0	--스킬레벨에 따라 커지게
			kBird:SetTargetScale(fScale, 0.56)

			local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
			if nil~=kSoundID and kSoundID~="" then
				actor:AttachSound(2783,kSoundID);
			end
		end
	end

	actor:AttachParticle(199, "char_root", "ef_skill_iceshower_01_char_root")
	actor:AttachParticle(1196, "char_root", "ef_skill_iceshower_02_char_root")

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end
	
	return true
end

function Skill_Ice_Shower_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Ice_Shower_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+3);
	actor:PlayCurrentSlot();

end

function Skill_Ice_Shower_Shot_Fire(actor,action)
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end

function Skill_Ice_Shower_LoadToWeapon(actor,action, kPos)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	kProjectileID = actor:GetEquippedWeaponProjectileID()
	local	kAttackNodeName = ""
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID")
	end

	local	kPilotGuid = actor:GetPilotGuid()
	local	kProjectileMan = GetProjectileMan()
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid)
	if nil~=kNewArrow and false==kNewArrow:IsNil() then
		kNewArrow:LoadToPosition(kPos)
	end
	
	return kNewArrow;
end

function Skill_Ice_Shower_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	if 1==action:GetParamInt(1) then
		Skill_Ice_Shower_FireProjectile(actor, action)
		action:SetParamInt(1,0)
	end
	if action:GetParam(300) == "finish" then
		return	false;
	end
	if actor:IsMyActor() and action:GetParamInt(6) == 0 then
		if KeyIsDown(action:GetActionKeyNo(0)) == false then
			action:SetParamInt(6,3);
		end
	end

	if animDone == true then
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
function Skill_Ice_Shower_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:FreeMove(false)

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	

	return true;
end
function Skill_Ice_Shower_OnLeave(actor, action)

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

function Skill_Ice_Shower_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		
		CheckNil(nil==action)
		CheckNil(action:IsNil())
		
		actor:ClearIgnoreEffectList()
	end
end

function Skill_Ice_Shower_OnOverridePacket(actor,action,packet)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if action:GetActionParam() == AP_FIRE then
		packet:PushInt(3);
    end    
end

function Skill_Ice_Shower_FireProjectile(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local	iAttackRange = action:GetAbil(AT_2ND_AREA_PARAM2)
	if 0==iAttackRange then
		iAttackRange = 50
	end

	local kActorPos = actor:GetTranslate()

	local	kLookingDir = actor:GetLookingDir()
	kLookingDir:Unitize()
	kLookingDir:Multiply(-1)
	
	-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir()
	kRotateAxis:Cross(Point3(0,0,1));
	kRotateAxis:Unitize()
	
	local kAddZ = iAttackRange*0.5+30
	local kRange = action:GetAbil(AT_1ST_AREA_PARAM3)
	if 0==kRange then
		kRange = kAddZ
	end

	local kAngle = 90-math.atan(kRange/kAddZ)*180/math.pi
	fRotateAngle = -kAngle*math.pi/180.0;

	local Projectile_cnt = action:GetAbil(AT_PROJECTILE_NUM)
	if nil==Projectile_cnt or 0==Projectile_cnt then
		Projectile_cnt = 20
	end

	local	kDelay = actor:GetAnimationInfo("PROJECTILE_DELAY")
	if nil==kDelay or ""==kDelay then
		kDelay = (1667*0.001)/Projectile_cnt--tonumber(kDelay)	--같은 시간동안 더 많이 쏴야 하기 때문에 시간계산을 해야 함
	else
		kDelay = tonumber(kDelay)	--같은 시간동안 더 많이 쏴야 하기 때문에 시간계산을 해야 함
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local i = 0
	while i<Projectile_cnt do
		local kOriginPos = Point3(math.random(-iAttackRange,iAttackRange), math.random(-iAttackRange,iAttackRange), 0)
		kOriginPos:Rotate(kRotateAxis, kAngle*math.pi/180.0)
		local	dir = actor:GetLookingDir()
		dir:Rotate(kRotateAxis, fRotateAngle)
		dir:Unitize()

		kOriginPos:Add(kActorPos)
		kOriginPos:SetZ(kOriginPos:GetZ()+kAddZ)
		local	kArrow = Skill_Ice_Shower_LoadToWeapon(actor,action, kOriginPos)

		local TargetPos = g_world:ThrowRay(kOriginPos,dir,1500)
	
		kArrow:SetParamValue("index", tostring(i))
		kArrow:SetParentPilotGuid(actor:GetPilotGuid());
		kArrow:SetHide(true)
		kArrow:SetTargetLoc(TargetPos)
		kArrow:SetScale(math.random(0.7, 1.0))
		kArrow:DelayFire(i*kDelay);--	발사!
		i=i+1
	end
end
