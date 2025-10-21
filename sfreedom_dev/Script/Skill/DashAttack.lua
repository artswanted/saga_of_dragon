-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Skill_Dash_Attack_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	--ODS("actor:GetAction():GetID():"..actor:GetAction():GetID().."\n",false, 3851)
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		if kCurAction:GetID() ~= "a_dash" then
			return false;
		end
	end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then
			return	false;
	end

	return true;
	
end

function Skill_Dash_Attack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    --ODS("Skill_Dash_Attack_OnEnter\n",false, 3851)
    
	action:SetParam(2,"");
	
	actor:FindPathNormal()		
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end
	
	local	kActorID = actor:GetID();
	if kActorID == "c_magician_male" or kActorID == "c_magician_female" then
		actor:AttachParticle(10,"char_root","ef_mag_dash_attk_char_root");	
	end
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	return	true;
end
function Skill_Dash_Attack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	--ODS("Skill_Dash_Attack_OnFindTarget iAttackRange:"..iAttackRange.."\n",false, 3851)

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end
	
	local	kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_Dash_Attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
    --ODS("Skill_Dash_Attack_OnUpdate\n");
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	actor:FindPathNormal()		
	
	local fAccel = -2000 * frameTime;
	local fVelocity = action:GetParamFloat(1);

	local kNewPos = actor:GetPos();
	local kOldPos = action:GetParamAsPoint(7);
	kOldPos:SetZ(0)
	kNewPos:SetZ(0); --Z를 무시하고 계산한다. 떨어지는 것은 중력에 의한 것

	local fDistance = kNewPos:Distance(kOldPos);
	local fMaxDistance = action:GetParamFloat(8);

	--이동한 거리가 있을 경우
	if 0.0 < fDistance then
		--남은 이동 거리
		local fDistance2 = fMaxDistance - fDistance;

		--두 벡터를 이용하여 방향 벡터를 구하고
		--방향 벡터를 이용하여 실제로 이동하는 곳의 위치를 구한다.
		local kDirection = kNewPos:_Subtract(kOldPos);
		kDirection:Unitize();
		kDirection:Multiply(fVelocity * frameTime);
		kDirection:Add(kNewPos);

		--이번 프레임에 이동 하는 거리
		local fDistance3 = kDirection:Distance(kOldPos);

		--이미 최대 대쉬 이동거리 거리를 벗어난 경우
		if fMaxDistance - fDistance3 < 0 then
			--fVelocity 다시 세팅해야 한다.				
			fVelocity = fDistance2 / frameTime;
		end
	end
	
--	ODS("__________Dash Attack Velocity " .. fVelocity .. "\n")

	if fVelocity>0 then
	
		local kMovingDir = actor:GetLookingDir()
--		ODS("__________Dash Attack Looking Dir : " .. kMovingDir:GetX() .. ", " .. kMovingDir:GetY() .. ", " .. kMovingDir:GetZ() .. "\n")
		kMovingDir:Multiply(fVelocity);
		
		actor:SetMovingDelta(kMovingDir);
		
		fVelocity = fVelocity+fAccel;
		action:SetParamFloat(1,fVelocity);
		
	else 
		
		if actor:IsAnimationDone() == true then
			action:SetParam(2,"end");
			return	false;					
		end
	
	end
	

	return	true;
end
function Skill_Dash_Attack_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:EndBodyTrail();
	return	true;
end

function Skill_Dash_Attack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	
	if actor:IsMyActor() == false then
		return	true;
	end

	if kCurAction:GetParam(2) == "end" then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	

	return	false;
end
function Skill_Dash_Attack_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

        if action:GetParam(3) == "HIT"  then
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
           SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        end        
    
    end

end

function Skill_Dash_Attack_OnEvent(actor,textKey,seqID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if textKey == "hit" then

		if actor:GetPilot():GetBaseClassID() == CT_FIGHTER then			
			actor:AttachParticle(31,"char_root","ef_fig_dash_attk_01_char_root");
		elseif  actor:GetPilot():GetBaseClassID() == CT_THIEF and false == actor:CheckStatusEffectExist("se_transform_to_metamorphosis") and false == actor:CheckStatusEffectExist("se_transform_to_Dancer") then
			actor:AttachParticle(31,"char_root","ef_thi_dash_attk_01_char_root");
		elseif actor:GetPilot():GetBaseClassID() == CT_ARCHER then
			actor:AttachParticle(31,"char_root","ef_arc_dash_attk_01_char_root");	
		elseif actor:GetPilot():GetBaseClassID() == CT_SHAMAN then
			actor:AttachParticle(31,"char_root","ef_fig_dash_attk_01_char_root");			
		end		
	
		local	kAction = actor:GetAction();
		
		CheckNil(nil==kAction);
		CheckNil(kAction:IsNil());
	
		if actor:IsUnderMyControl() then
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
		        
					Actor_PC_Base_DoDefaultMeleeAttack(actor,actor:GetAction(),g_fDashAttackFreezeTime);
					Skill_Dash_Attack_OnTargetListModified(actor,kAction,false)
		            
					return;
				else
		        	kAction:BroadCastTargetListModify(actor:GetPilot());
		            
				end			
			
			end	
		end
		
		kAction:SetParam(3,"HIT");
	end

	return	true;
end

function Skill_Dash_Attack_OnCastingCompleted(actor,action)
end