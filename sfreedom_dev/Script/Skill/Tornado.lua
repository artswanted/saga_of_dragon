-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

-- Melee
function Skill_Tornado_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Tornado_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Tornado_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Tornado_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_Tornado_Fire\n");
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
			
	actor:StartWeaponTrail();
end
function Skill_Tornado_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100 
	end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	--local	fBackDistance = 10;
	kDir:Multiply(iAttackRange);
	kPos:Add(kDir);
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local   kGroundPos = g_world:ThrowRay(Point3(kPos:GetX(),kPos:GetY(),kPos:GetZ()+50.0),Point3(0,0,-1),100);
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 80
	end
    
	local kParam = FindTargetParam();
	
	kParam:SetParam_1(kGroundPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,30,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iTargets = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
	return iTargets;
	
end

function Skill_Tornado_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Tornado_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Tornado_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_Tornado_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
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

function Skill_Tornado_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	ODS("RestoreItemModel Skill_Tornado_ReallyLeave\n");
	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	actor:EndWeaponTrail();
	return true;
end

function Skill_Tornado_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
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
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Tornado_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
	if kActionResult:GetCritical() then
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
	else
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
	end	
	
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	actor:SetShakeInPeriod(5, iShakeTime/2);
	actorTarget:SetShakeInPeriod(5, iShakeTime);		
	
	-- 피격 소리 재생
	local actionName = kActionID
	if kActionResult:GetCritical() then
		actionName = "Critical_dmg"
	end
	actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, kActionTargetInfo);

end

function Skill_Tornado_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_Tornado_HitOneTime\n");
    
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	kDir:Multiply(iAttackRange);
	kPos:Add(kDir);
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local   kGroundPos = g_world:ThrowRay(Point3(kPos:GetX(),kPos:GetY(),kPos:GetZ()+50.0),Point3(0,0,-1),100);
	if kGroundPos:GetX() == -1 then
	    kGroundPos = kPos;
    end
    
    actor:AttachParticleToPoint(8111,kGroundPos,"ef_Tornado_char_root");
	    
	

	local	iHitCount = action:GetParamInt(2);

	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount == iTotalHit then
		return
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
		
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end
function Skill_Tornado_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParamInt(2) > 0 then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            SkillHelpFunc_DefaultHitOneTime(actor,action, false);
        
        end        
    
    end

end
function Skill_Tornado_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
		
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if IsSingleMode() then
					Skill_Tornado_OnTargetListModified(actor,kAction,false)
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end			
			
			end	
			
		end

        Skill_Tornado_HitOneTime(actor,kAction)
	end

	return	true;
end
