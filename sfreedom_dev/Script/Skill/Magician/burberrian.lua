

function Skill_Burberrian_OnCheckCanEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
end

function Skill_Burberrian_OnCastingCompleted(actor, action)
	Skill_Burberrian_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_Burberrian_Fire(actor,action)
	actor:DetachFrom(126)
	--actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish") -- 밑에 이펙트로 교체
	actor:AttachParticle(127, "char_root", "ef_mag_dash_attk_char_root")	
end

function Skill_Burberrian_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Burberrian" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return false 
	end

	actor:Stop()

	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Burberrian_OnCastingCompleted(actor,action);	
	
	return true
end

function Skill_Burberrian_OnFindTarget(actor,action,kTargets)
	--[[local kParam = FindTargetParam()
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 60
	end	
	kParam:SetParam_1(actor:GetTranslate(), actor:GetLookingDir())
	kParam:SetParam_2(action:GetSkillRange(0,actor), iParam2, 0, 0)
	kParam:SetParam_3(true, FTO_NORMAL)
	
	return	action:FindTargets(TAT_BAR, kParam, kTargets, kTargets);
	]]--

	-- 포인터 버스터류
	return Skill_Magician_Charge_Attack_OnFindTarget(actor,action,kTargets);
end

function Skill_Burberrian_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local bIsAnimDone = actor:IsAnimationDone()
	local nextActionID = action:GetNextActionName()
	local endParam = action:GetParam(911)
	local iSlot = action:GetCurrentSlot()

	--ODS("_______Bur Update_________\n")

	if endParam == "end" then
		return false
	end

	if bIsAnimDone == true then
		if iSlot == 1 and nextActionID == "a_battle_idle" then
			action:SetParam(911, "end")
			nextActionID = "a_battle_idle"
			return false
		end
	end

	return true
end
function Skill_Burberrian_OnCleanUp(actor, action)
end

function Skill_Burberrian_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)
	--ODS("_______Bur Leave_________\n")


	if endParam == "end" then
		return true
	end

	return false
end

function Skill_Burberrian_OnEvent(actor, textKey)
	local action = actor:GetAction();

	if textKey == "hit" or textKey == "fire" then
		action:CreateActionTargetList(actor);
		Skill_Burberrian_DoEffect(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
	end

	return true
end

function Skill_Burberrian_OnTargetListModified(actor,action,bIsBefore)

    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or action:GetParamInt(2) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            Act_Melee_Default_HitOneTime(actor,action);
        
        end        
    
    end

end


function Skill_Burberrian_DoEffect(actor)
	local action = actor:GetAction()
	local kHelperPos = actor:GetNodeWorldPos("p_wp_l_hand")
	local kTargetDir = actor:GetPos()
	local kMovingDir = actor:GetLookingDir()
	local guid = GUID("123")
	
	--ODS("Skill_Burberrian_DoEffect kMovingDir:"..kMovingDir:GetX()..","..kMovingDir:GetY()..","..kMovingDir:GetZ().."\n");

	ODS("________바바리 등장_______\n")

	-- 바바리 아저씨등장, 이펙트 사운드 연출
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 900002, 0)
	if pilot:IsNil() == false then
		local kNewActor = pilot:GetActor()
		kNewActor:FreeMove(true)
		kMovingDir:Multiply(40)
		kTargetDir:Add(kMovingDir)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, kNewActor, kTargetDir, 9)
		
		local pt = kNewActor:GetTranslate()
		pt:SetZ(pt:GetZ() - 10)
		kNewActor:AttachParticleToPoint(192, pt, "ef_bang")
		kNewActor:ReserveTransitAction("a_Burberrian_Freeze")
		kNewActor:ClearActionState()
		kTargetDir:Add(kMovingDir)
		kNewActor:LookAt(kTargetDir,true)
		
		kNewActor:AttachSound(7283,"Burberrian");
		kNewActor:AttachParticle(7212,"char_root","ef_burberryman_01_char_root");		
	end

	action:GetTargetList():ApplyActionEffects()
end

function Skill_Burberrian_DoUpdate(actor, accumTime, frameTime)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local world = g_world

	if actor:IsAnimationDone() == true and
		world:IsNil() == false then
		local pt = actor:GetTranslate()
		pt:SetZ(pt:GetZ() - 30)
		actor:AttachParticleToPoint(192, pt, "ef_bang")
		world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end
end


function Skill_Burberrian_Freeze_OnEnter(actor, action)
	--action:SetDoNotBroadCast(true)
	--ODS("Freeze Enter____________\n")
	action:SetParamInt(4,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(6,g_world:GetAccumTime());
	return true
end

function Skill_Burberrian_Freeze_OnUpdate(actor, accumTime, frameTime)

	local action = actor:GetAction()
	local iState = action:GetParamInt(4);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end	
	local world = g_world;

	--ODS("Freeze Update____________\n")
	if iState == 0 then
		if accumTime - action:GetParamFloat(6) > 1 then
			action:SetParamInt(4,1);
			action:SetParamFloat(6,g_world:GetAccumTime());
		end
	end

	if iState == 1 then
		if accumTime - action:GetParamFloat(6) > 1 then
			action:SetParamFloat(6,g_world:GetAccumTime());
			actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.3);
			action:SetParamInt(4,2);
		end
	elseif iState == 2 then
		if accumTime - action:GetParamFloat(6) > 0.3 then
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
			return false;
		end
	end
	return true
end

function Skill_Burberrian_Freeze_OnCleanUp(actor, action)
end

function Skill_Burberrian_Freeze_OnLeave(actor, action)
	--ODS("Freeze Leave____________\n")
	return true
end

