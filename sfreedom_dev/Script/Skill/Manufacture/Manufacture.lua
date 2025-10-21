--[[
function Manufacture_OnOverridePacket(actor, action, packet)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	--local kLookAtPos = actor:GetLookingDir()
	--kLookAtPos:Add(actor:GetPos())
	--packet:PushPoint3(kLookAtPos)
	
	--ODS("본인 X:"..kLookAtPos:GetX().." Y:"..kLookAtPos:GetY().." Z:"..kLookAtPos:GetZ().."\n", false, 912)
	
end
]]--
function Manufacture_OnEvent(actor,textKey)
	return true
end

function Manufacture_OnCheckCanEnter(actor,action)
	local kCurAction = actor:GetAction()
	if kCurAction == nil or kCurAction:IsNil() then
		return false
	end
	
	if actor:IsRidingPet() then --펫 탑승 중에 사용 불가
		return false
	end
	
	local kCurActionID = kCurAction:GetID() --액션 상태가 아이들이 아닌 경우 무조건 불가
	if (kCurActionID ~= "a_idle" and kCurActionID ~= "a_Man_Cook" and
		kCurActionID ~= "a_Man_Spell" and kCurActionID ~= "a_Man_Workmanship") or
		actor:IsMeetFloor() == false then
		return false
	end

	return CheckCanUseManufacture() --아이템 사용 가능 체크
end

function Manufacture_OnEnter(actor,action)
	SkillHelpFunc_HideWeapon(actor, action)		-- 무기 감추기 
	
	actor:ResetAnimation()
	action:SetSlot(0)
	actor:PlayCurrentSlot()
	SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND1")
	Manufacture_OnNextStep(actor, action, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
	Manufacture_OnNextStep(actor, action, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)


	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(2, g_world:GetCameraZoom())
		g_world:SetCameraZoom(0)
		local kCalledUI = CallUI("FRM_MANUFACTURE_GAUGE")
		if kCalledUI == nil or kCalledUI:IsNil() then
			ActivateUI("FRM_MANUFACTURE_GAUGE")
		end
	end
	return true
end

function Manufacture_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	if action == nil or action:IsNil() then
		return true
	end
	
	local animDone = actor:IsAnimationDone()
	local iCurSlot = action:GetCurrentSlot()
	if animDone then
		if iCurSlot == 0 then
			actor:ResetAnimation()
			action:SetSlot(1)
			actor:PlayCurrentSlot()
			action:SetParamInt(0, 0)
			SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND1")
			SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND2")	
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)
			return true
		elseif iCurSlot == 1 then
			actor:ResetAnimation()
			actor:PlayCurrentSlot()
			SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND2")
			return true
		elseif iCurSlot > 1 then
			return false
		end
	end

	if iCurSlot == 1 and action:GetParamInt(0) == 0 and IsFinishedManufactureGaugeUI() == true then --게이지가 멈춰져 있다면..
		SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND2")
		local iScore = GetScoreManufactureGaugeUI()
		if iScore == 0 then
			local kWnd = GetUIWnd("FRM_MANUFACTURE_GAUGE")
			if kWnd ~= nil and kWnd:IsNil() == false then
				OnHitEffectManufactureGaugeUI(kWnd)
			end
			action:SetSlot(2)
			SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND3")
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)

		else
			action:SetSlot(3)
			SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND4")
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
			Manufacture_OnNextStep(actor, action, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)
		end
		actor:ResetAnimation()
		actor:PlayCurrentSlot()
		action:SetParamInt(0, 1)
	elseif iCurSlot > 0 and action:GetParamInt(0) == 1 and IsFinishedManufactureGaugeUI() == false then --게이지가 다시 시작 되었다면...
		actor:ResetAnimation()
		action:SetSlot(0) --액션도 다시 시작해주자
		actor:PlayCurrentSlot()
		SkillHelpFunc_PlaySound(actor, action, "MANUFACTURE_TOOL_SOUND1")
		Manufacture_OnNextStep(actor, action, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
		Manufacture_OnNextStep(actor, action, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)
	end
	
	return true
end

function Manufacture_OnLeave(actor, action)

	local kCurAction = actor:GetAction();
	local kNextActionID = action:GetID()
	
	if(false == action:GetEnable() ) then 
		return false --채팅 입력 중 방향키가 들어왔을때를 위한 방어코드
	end
	
	if CheckCanUseManufacture() == false then --아이템 수량이 모자르거나 사용할 수 없다면 액션 종료
		return true
	elseif kNextActionID == "a_idle" or kCurAction:GetID() == kNextActionID then
		InitManufactureGaugeUI()
		actor:ResetAnimation()
		kCurAction:SetSlot(1)
		actor:PlayCurrentSlot()
		kCurAction:SetParamInt(0, 0)
		SkillHelpFunc_StopSound(actor, kCurAction, "MANUFACTURE_TOOL_SOUND1")
		SkillHelpFunc_PlaySound(actor, kCurAction, "MANUFACTURE_TOOL_SOUND2")	
		Manufacture_OnNextStep(actor, kCurAction, "MANUFACTURE_TOOL_EFFECT_ID", "MANUFACTURE_TOOL_TARGET_NODE", 10001)
		Manufacture_OnNextStep(actor, kCurAction, "MANUFACTURE_EFFECT_ID", "MANUFACTURE_EFFECT_TARGET_NODE", 10002)
		return false
	elseif kNextActionID == "a_Man_Cook" or
	kNextActionID == "a_Man_Spell" or
	kNextActionID == "a_Man_Workmanship" then
		return true
	elseif kNextActionID == "a_run" then
--		local kChatBarWnd = GetUIWnd("ChatBar") --슈발 이런 개같은 경우가 있나. 채팅 입력 도중 방향키 입력하면 "a_walk"가 들어온다. 이것땜에 개고생했네
--		if kChatBarWnd ~= nil and kChatBarWnd:IsNil() == false then
--			local kEditChat = kChatBarWnd:GetControl("EDT_CHAT")
--			if kEditChat ~= nil and kEditChat:IsNil() == false then
--				if kEditChat:IsFocus() then
--					return false
--				end
--			end
--		end
		return true
	end
	return false
end

function Manufacture_OnCleanUp(actor)
	--액션 복구
	local action = actor:GetAction()
	if action ~= nil and action:IsNil() == false then
		SkillHelpFunc_ShowWeapon(actor, action)						-- 무기가 감춰져있으면 복구하고	(1번만 해야한다)

		if action:GetCurrentSlot() == 0 then
			SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND1")
		elseif action:GetCurrentSlot() == 1 then
			SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND2")
		elseif action:GetCurrentSlot() == 2 then
			SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND3")
		elseif action:GetCurrentSlot() == 3 then
			SkillHelpFunc_StopSound(actor, action, "MANUFACTURE_TOOL_SOUND4")
		end
		actor:DetachFrom(10001, true)
		actor:DetachFrom(10002, true)
		
	end
	
	--UI 지움
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetCameraZoom(action:GetParamFloat(2)) --카메라 줌 복구
		CloseUI("FRM_MANUFACTURE_GAUGE")
	end

	return true
end

function Manufacture_OnNextStep(actor, action, szDefName, szDefTargetNodeName, iEffectSlot)
	local iCurSlot = action:GetCurrentSlot()

	local szCurName = szDefName..(iCurSlot+1)
	local szCurTargetNodeName = szDefTargetNodeName..(iCurSlot+1)
	local kCurID = action:GetScriptParam(szCurName)
	if iCurSlot > 0 then --현재 슬롯이 처음이 아니라면 이전에 붙혔던 파티클들은 지워준다.
		local iPrevSlot = iCurSlot - 1
		if iCurSlot == 3 then
			iPrevSlot = 1
		end

		local szPrevName = szDefName..(iPrevSlot+1)
		local kPrevID = action:GetScriptParam(szPrevName)
		if kPrevID ~= kCurID then
			actor:DetachFrom(iEffectSlot, true)
		end
	end

	if kCurID ~= "" then
		SkillHelpFunc_AttachParticleToNode(iEffectSlot, szCurName, szCurTargetNodeName, 1, actor, action)
	end

end
