-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)


function Skill_Transformation_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;	
end

function Skill_Transformation_OnCastingCompleted(actor, action)
	Skill_Transformation_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end
function Skill_Transformation_Fire(actor,action)
	actor:DetachFrom(126)
	--actor:AttachParticle(127, "p_wp_l_hand", "ef_mag_spell_finish") -- 밑에 이펙트로 교체
	actor:AttachParticle(127, "char_root", "ef_mag_dash_attk_char_root")

	local iSkillRange = action:GetSkillRange(0,actor);
	local kDir = actor:GetLookingDir();
    kDir:Multiply(iSkillRange);
    kDir:Add(actor:GetPos());
    kDir:SetZ(kDir:GetZ()-20);

	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	
end

function Skill_Transformation_OnEnter(actor, action)
	local prevActionID = actor:GetAction():GetID()

	if prevActionID == "a_Transformation" then
		action:SetParam(911, "end")
		action:SetDoNotBroadCast(true)
		return true 
	end

	actor:Stop()


	local iSkillRange = action:GetSkillRange(0,actor);
	local kDir = actor:GetLookingDir();
    kDir:Multiply(iSkillRange);
    kDir:Add(actor:GetPos());
    kDir:SetZ(kDir:GetZ()-20);

	actor:AttachParticleToPointS(7212,kDir, "ef_Curseofwitch_01_char_root", 1.2);

	actor:AttachParticle(126, "p_wp_l_hand", "ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand")

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Transformation_OnCastingCompleted(actor,action);	
	return true
end
function Skill_Transformation_OnFindTarget(actor,action,kTargets)

-- 포인터 버스터류
	return Skill_Magician_Charge_Attack_OnFindTarget(actor,action,kTargets);
end

function Skill_Transformation_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local bIsAnimDone = actor:IsAnimationDone()
	local nextActionID = action:GetNextActionName()
	local endParam = action:GetParam(911)
	local iSlot = action:GetCurrentSlot()

	if endParam == "end" then
		return false
	end

	if bIsAnimDone == true then
		if iSlot == 1 and nextActionID == "a_idle" then
			action:SetParam(911, "end")
			nextActionID = "a_battle_idle"
			return false
		end
	end

	return true
end

function Skill_Transformation_OnCleanUp(actor, action)
	actor:DetachFrom(126,true)
	actor:DetachFrom(127,true)
end

function Skill_Transformation_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local nextActionID = action:GetID()
	local endParam = curAction:GetParam(911)

	if endParam == "end" then
		return true
	end

	return false
end
function Skill_Transformation_OnTargetListModified(actor,action,bIsBefore)

    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
            else
                Skill_Transformation_DoEffect(actor);
            end
        
        end        
    
    end

end
function Skill_Transformation_OnEvent(actor, textKey)

	if textKey == "hit" or textKey == "fire" then
	
		actor:AttachSound(7283,"Transformation");
		
	
		if actor:IsUnderMyControl() then
		
			local	kAction = actor:GetAction();
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Transformation_OnTargetListModified(actor,actor:GetAction(),false)
				else
					actor:GetAction():BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end
		
		end
		actor:GetAction():SetParam(2,"HIT");
			
		Skill_Transformation_DoEffect(actor)
	end

	return true
end

function Skill_Transformation_DoEffect(actor)
	local action = actor:GetAction()
	action:GetTargetList():ApplyActionEffects()
end

