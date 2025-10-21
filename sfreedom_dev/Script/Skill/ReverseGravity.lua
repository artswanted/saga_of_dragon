-- ReverseGravity
-- ani slot : 0 - idle, 1 - casting, 2 - casting idle, 3 - down
-- camera는 SkillTime 동안 천천히 움직이고, Fallen 된 후에 다시 원래대로 돌아간다.
-- SKILL_RANGE : 어느 범위 안에 있는 몬스터들이 맞을 것인가?
-- SKILL_TIME : Skill 시전시간 - 올라가는 시간
-- SKILL_MAX_HEIGHT : 어디까지 들어 올릴것인가?
-- CASTING_EFFECT_ID : 캐스팅 시작할 때 붙는 Effect
-- PC_UP_EFFECT_ID : 올라가기 시작할 때 붙는 Effect
-- PC_DOWN_EFFECT_ID : 내려오기 시작할 때 붙는 Effect
-- MONSTER_UP_EFFECT_ID : 몬스터를 들어올릴때의 Effect
-- MONSTER_DOWN_EFFECT_ID : 몬스터를 들어올릴때의 Effect
-- MONSTER_FALLEN_EFFECT_ID : 몬스터들이 땅에 떨어진 후의 Effect
-- CAMERA_Z_UP : 위로/아래로 움직이는 거리
-- CAMERA_Z_ROT : 위/아래로 회전하는 각도
-- CAMERA_ZOOM : 앞/뒤로 Zoom되는 거리
-- Param 0 : State, 1~4 : Original Camera 위치, 5~6 : floating speed, height, 15~ : 각 target의 start z position 
function Skill_ReverseGravity_OnCastingCompleted(actor, action)
end

function Skill_ReverseGravity_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local skillRange = action:GetScriptParam("SKILL_RANGE")
	local skillTime = action:GetScriptParam("SKILL_TIME")	
	local iCastingTime = action:GetAbil(AT_CAST_TIME)

	if actor:IsMyActor() == true then
		actor:StopNormalAttackFreeze()
		action:LockCurrentTarget(skillRange, 999, true) -- 모든 몬스터를 다 Targeting한다.

		if action:GetTargetCount() <= 0 then
			return false;
		end
		
		-- 보스만 있을 경우에는 못 쓴다.
		--if action:GetTargetCount() == 1 then
		--	targetGUID = action:GetTargetGUID(0)
		--	if targetGUID:IsNil() == false then				
		--		target = g_pilotMan:FindPilot(targetGUID)
		--		if target:IsNil() == false and target:GetMonsterNo() == 23 then
		--			return false
		--		end
		--	end
		--end
	end
	
	actor:SeeFront(true)
	actor:SetComboCount(0);		
	action:SetParam(0, "ready")
	
	Skill_ReverseGravity_CastEffect(actor,action, true)
	action:SetSlot(1)
	action:SetParam(0, "casting")
	if actor:IsMyActor() == true then		
		actor:StartSkillCoolTime(action:GetActionNo())		
		-- 캐스트가 끝나면 SkillStart를 하자.
		action:SetSkillTime(skillTime)
		action:SkillStart()
		action:SetParamFloat(1, GetCameraTrn():GetX())
		action:SetParamFloat(2, GetCameraTrn():GetY())
		action:SetParamFloat(3, GetCameraTrn():GetZ())
		action:SetParamFloat(4, GetCameraRot():GetZ())
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilter(true, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.8)
		GetSkillTargetMan():SetDrawTargetMark(false)
	end

	-- Action의 GUID Container에 target list를 집어 넣는다.
	if action:GetTargetCount() > 0 then
		for iIndex = 0, action:GetTargetCount() - 1 do
			local targetGUID = action:GetTargetGUID(iIndex)
			if targetGUID:IsNil() == false then
				action:AddNewGUID(targetGUID)
				local target = g_pilotMan:FindPilot(targetGUID)
				if target:IsNil() == false then
					local targetActor = target:GetActor()
					if targetActor:IsNil() == false then
						targetActor:ReserveTransitAction("a_reverse_gravity")
					end            
				end
			end
		end
	end
	return true
end

function Skill_ReverseGravity_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_ReverseGravity_OnUpdate Start\n");
	
	local skillTime = action:GetScriptParam("SKILL_TIME")
	local maxHeight = action:GetScriptParam("SKILL_MAX_HEIGHT")
	
	if actor:IsAnimationDone() == true and action:GetParam(0) == "casting" then
      -- 팔을 위로 올리는 동작이 끝났다. 이제 몬스터 들어올리기 시작		
		--action:StartReverseGravity(skillTime, maxHeight)		
		action:SetParamFloat(5, maxHeight/skillTime * 2)		-- floating speed 계산
		action:SetParamFloat(6, maxHeight)
		--Skill_ReverseGravity_CastEffect(actor,action, false)
		Skill_ReverseGravity_UpEffect(actor,action, true)
		action:SetParam(0, "up")
		action:SetSlot(2)
		actor:PlayCurrentSlot()
		action:SetSkillTime(skillTime)
		action:SkillStart()
		QuakeCamera(skillTime, 0.5) -- 미세하게 흔들자.
		if actor:IsMyActor() == true then
			local cameraTrn = GetCameraTrn()
			local cameraRot = GetCameraRot()
			cameraTrn:SetY(cameraTrn:GetY() + action:GetScriptParam("CAMERA_ZOOM"))
			cameraTrn:SetZ(cameraTrn:GetZ() + action:GetScriptParam("CAMERA_Z_UP"))
			cameraRot:SetZ(cameraRot:GetZ() + action:GetScriptParam("CAMERA_Z_ROT"))
			SetCameraSetting(cameraTrn, cameraRot)
		end
		-- monster animation 바꾸자.
		for iIndex = 0, action:GetTotalGUIDInContainer() - 1 do
			local targetGUID = action:GetGUIDInContainer(iIndex)
			local target = g_pilotMan:FindPilot(targetGUID)
			if target:IsNil() == false then
				local targetActor = target:GetActor()
				if targetActor:IsNil() == false then
					targetActor:SetTargetAnimation("float_01")
					action:SetParam(15+iIndex, targetActor:GetTranslate():GetZ())
				end
			end
		end
		return true
	end
	
	if action:GetParam(0) == "up" then		
		if action:IsSkillEnd() == false then -- 들어 올리는 중			
			for iIndex = 0, action:GetTotalGUIDInContainer() - 1 do
				local targetGUID = action:GetGUIDInContainer(iIndex)
				local target = g_pilotMan:FindPilot(targetGUID)				
				if target:IsNil() == false then
					local targetActor = target:GetActor()
					if targetActor:IsNil() == false then
						targetActor:FreeMove(true)
						if action:GetParamFloat(6) >= 0 then
							targetActor:SetMovingDelta(Point3(0,0,action:GetParamFloat(5))) -- 떨어지는 위치 조정							
						else
							targetActor:SetMovingDelta(Point3(0,0,maxHeight * 0.5 * math.sin(iIndex / 3.14 + accumTime * 6.28 / skillTime))) -- 위에서 흔들 흔들
						end
						if targetActor:IsAnimationDone() == true then
							targetActor:SetTargetAnimation("float_04")
						end
					end
				end
			end
			action:SetParamFloat(6, action:GetParamFloat(6) - action:GetParamFloat(5) * frameTime) -- 높이 빼기
		else
			-- 다 들어 올렸으니 떨구자
			--Skill_ReverseGravity_UpEffect(actor,action, false)  -- 캐릭터의 up Effect는 놔둬야 해서. 
			Skill_ReverseGravity_DetachParticle_FromTargets(5)
			Skill_ReverseGravity_DownEffect(actor,action, true)
			action:SetParamFloat(5, -maxHeight/0.2)
			action:SetParam(0, "down")
			--action:SetParam(10, accumTime +0.10)
			action:SetSlot(3)
			actor:PlayCurrentSlot()
			return true
		end
	end
	
	--if action:GetParam(0) == "down_ready" then
	--	if action:GetParamFloat(10) <= accumTime then
	--		action:SetParam(0, "down")
	--	end
	--end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if action:GetParam(0) == "down" then
		if action:GetTargetCount() > 0 then
			for iIndex = 0, action:GetTargetCount() - 1 do
				local targetGUID = action:GetTargetGUID(iIndex)
				local target = g_pilotMan:FindPilot(targetGUID)
				if target:IsNil() == false then
					local targetActor = target:GetActor()
					if targetActor:IsNil() == false and action:CheckGUIDExist(targetGUID) == true then
						targetActor:SetMovingDelta(Point3(0,0,action:GetParamFloat(5))) -- 떨어지는 위치 조정
						if targetActor:GetTranslate():GetZ() + action:GetParamFloat(5) * frameTime * 2 <= action:GetParamFloat(15+iIndex) then 
						--if targetActor:IsMeetFloor() == true then
							-- 떨어졌다.
							if actor:IsMyActor() == true then
								g_world:SetShowWorldFocusFilter(false)
							end
							targetActor:SetTargetAnimation("float_03")
							Net_ReqDamage(action:GetActionNo(),action:GetActionInstanceID(),targetGUID)
							Skill_ReverseGravity_FallenEffect(targetActor, action, true)
							QuakeCamera(0.8, 3.0) -- 짧고 강하게.
							action:RemoveGUID(targetGUID) -- 이제 떨어졌으니 관심 끄고.
							targetActor:FreeMove(false)
							targetActor:PlayWeaponSound(WST_HIT, actor, "a_ReverseGravity", 0, action:GetTargetInfo(iIndex))
						else
							-- 떨어지는 중						
							--targetActor:SetMovingDelta(Point3(0,0,action:GetParamFloat(5))) -- 떨어지는 위치 조정
						end
					end
				end
			end
		end
		
		-- 모두 다 떨어졌으면.
		if action:GetTotalGUIDInContainer() == 0 or actor:IsAnimationDone() == true then
			action:SetParam(0, "fallen")
			if actor:IsMyActor() == true then
				local cameraTrn = GetCameraTrn()
				local cameraRot = GetCameraRot()
				cameraTrn:SetX(action:GetParamFloat(1))
				cameraTrn:SetY(action:GetParamFloat(2))
				cameraTrn:SetZ(action:GetParamFloat(3))
				cameraRot:SetZ(action:GetParamFloat(4))
				SetCameraSetting(cameraTrn, cameraRot)		
			end
		end
		action:SetParamFloat(5, action:GetParamFloat(5) - frameTime * 5000)
		return true
	end
	
	-- 모두 다 떨어지고 애니메이션까지 끝나면.
	if action:GetParam(0) == "fallen" and actor:IsAnimationDone() == true then
		action:SetSlot(0)
		actor:PlayCurrentSlot()
		if action:GetTargetCount() > 0 then
			for iIndex = 0, action:GetTargetCount() - 1 do
				local targetGUID = action:GetTargetGUID(iIndex)
				if targetGUID:IsNil() == false then	         
					local target = g_pilotMan:FindPilot(targetGUID)
					if target:IsNil() == false then
						local targetActor = target:GetActor()
						if targetActor:IsNil() == false then
							targetActor:SetTargetAnimation("StandUp")
						end
					end
				end
			end
		end
		return false
	end
	
	return true
end

function Skill_ReverseGravity_OnCleanUp(actor, action)
end

function Skill_ReverseGravity_ReallyLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then      
		GetSkillTargetMan():SetDrawTargetMark(true)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilter(false) -- 혹시 모르니까.
	end

	if action:GetParam(0) ~= "fallen" then
		Skill_ReverseGravity_CastEffect(actor, action, false)
		Skill_ReverseGravity_UpEffect(actor, action, false)
		Skill_ReverseGravity_DownEffect(actor, action, false)
		if actor:IsMyActor() == true then
			local cameraTrn = GetCameraTrn()
			local cameraRot = GetCameraRot()
			cameraTrn:SetX(action:GetParamFloat(1))
			cameraTrn:SetY(action:GetParamFloat(2))
			cameraTrn:SetZ(action:GetParamFloat(3))
			cameraRot:SetZ(action:GetParamFloat(4))
			SetCameraSetting(cameraTrn, cameraRot)
		end
	end
	
	if action:GetTargetCount() > 0 then
		for iIndex = 0, action:GetTargetCount() - 1 do
			local targetGUID = action:GetTargetGUID(iIndex)
			if targetGUID:IsNil() == false then	         
				local target = g_pilotMan:FindPilot(targetGUID)
				if target:IsNil() == false then
					local targetActor = target:GetActor()
					if targetActor:IsNil() == false then
						if action:GetParam(0) ~= "fallen" then
							Skill_ReverseGravity_FallenEffect(targetActor, action, false)
							targetActor:FreeMove(false)
						end
						targetActor:ReserveTransitAction("a_idle") -- Action/reversegravity.lua의 OnLeave에서 막아놨기 때문에 바꿔준다. (별로 좋은거 같지는 않다.)
					end
				end
			end
		end
	end
	return true
end

function Skill_ReverseGravity_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actionID = action:GetID()
	local current_action = actor:GetAction();

	if actor:IsMyActor() == false then
		return Skill_ReverseGravity_ReallyLeave(actor, current_action);
	end
	
	if 	action:GetActionType()=="EFFECT" or 
		current_action:GetParam(0) == "fallen" then
		return Skill_ReverseGravity_ReallyLeave(actor, current_action);
	end
	
	return false
end

function Skill_ReverseGravity_OnEvent(actor, textKey)
	local action = actor:GetAction();
	--ODS("Skill_ReverseGravity_OnEvent textKey : "..textKey.." slot : "..action:GetCurrentSlot().."\n");	
end

function Skill_ReverseGravity_AttachParticle_ToTargets(action, slot, attachTarget, effectid)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if effectid == "" then
		return
	end
	
	if action:GetTargetCount() > 0 then
		for iIndex = 0, action:GetTargetCount() - 1 do
			local targetGUID = action:GetTargetGUID(iIndex)
			local target = g_pilotMan:FindPilot(targetGUID)      
			if target:IsNil() == false then
				local targetActor = target:GetActor()
				if targetActor:IsNil() == false then
					targetActor:AttachParticle(slot, attachTarget, effectid);
				end
			end
		end
	end
end

function Skill_ReverseGravity_DetachParticle_FromTargets(slot)
	if action:GetTargetCount() > 0 then
		for iIndex = 0, action:GetTargetCount() - 1 do
			local targetGUID = action:GetTargetGUID(iIndex)
			local target = g_pilotMan:FindPilot(targetGUID)
			if target:IsNil() == false then
				local targetActor = target:GetActor()
				if targetActor:IsNil() == false then
					targetActor:DetachFrom(slot)
				end
			end
		end
	end
end

function Skill_ReverseGravity_CastEffect(actor, action, bAttach)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bAttach == true and action:GetScriptParam("CASTING_EFFECT_ID")~="" then
		actor:AttachParticle(1, "char_root", action:GetScriptParam("CASTING_EFFECT_ID"));
	else
		actor:DetachFrom(1);
	end
end

function Skill_ReverseGravity_UpEffect(actor, action, bAttach)	

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bAttach == true and action:GetScriptParam("PC_UP_EFFECT_ID")~="" then
		actor:AttachParticle(5, "char_root", action:GetScriptParam("PC_UP_EFFECT_ID"))
	else
		actor:DetachFrom(5);
	end
	
	if bAttach == true and action:GetScriptParam("MONSTER_UP_EFFECT_ID") ~= "" then
		Skill_ReverseGravity_AttachParticle_ToTargets(action, 5, "char_root", action:GetScriptParam("MONSTER_UP_EFFECT_ID"))
	else
		Skill_ReverseGravity_DetachParticle_FromTargets(5)
	end
end

function Skill_ReverseGravity_DownEffect(actor, action, bAttach)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bAttach == true and action:GetScriptParam("PC_DOWN_EFFECT_ID")~="" then
		actor:AttachParticle(10,"char_root",action:GetScriptParam("PC_DOWN_EFFECT_ID"))
	else
		actor:DetachFrom(10);
	end
	
	if bAttach == true and action:GetScriptParam("MONSTER_DOWN_EFFECT_ID") ~= "" then
		Skill_ReverseGravity_AttachParticle_ToTargets(action, 10, "char_root", action:GetScriptParam("MONSTER_DOWN_EFFECT_ID"))
	else
		Skill_ReverseGravity_DetachParticle_FromTargets(10)
	end
end

function Skill_ReverseGravity_FallenEffect(actor, action, bAttach)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bAttach == true and action:GetScriptParam("MONSTER_FALLEN_EFFECT_ID")~="" then
		actor:AttachParticle(15, "char_root", action:GetScriptParam("MONSTER_FALLEN_EFFECT_ID"));
	else
		actor:DetachFrom(15);
	end
end
