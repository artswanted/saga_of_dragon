-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

-- Melee
function Air_Sprash_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end

function Air_Sprash_OnCastingCompleted(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Air_Sprash_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Air_Sprash_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
		
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	local iFireCount = action:GetAbil(AT_COUNT);
	--ODS("iFireCount : "..iFireCount.."\n", false, 3851)
	action:SetParamInt(100,iFireCount)			--발사해야 할  횟수
	action:SetParamInt(101,1)					--현재 발사 한 횟수 (초기화)
		
	Air_Sprash_OnCastingCompleted(actor,action);
	
	return true
end

function Air_Sprash_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
	
		local curActionParam = action:GetParam(0)
		local iFireCount = action:GetParamInt(100)
		local iCurFireCount = action:GetParamInt(101)
		
		if iCurFireCount < iFireCount then
			actor:ResetAnimation()
			actor:PlayCurrentSlot(true);
			action:SetParamInt(101, iCurFireCount+1)
--			ODS("iCurFireCount : "..iCurFireCount.."\n", false, 3851)
		elseif curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			action:SetParamInt(101,1)
			
			action:SetNextActionName(nextActionName)
			return false;
		else
		
			action:SetParam(1, "end")
			return false
		end
	end

	return true
end

function Air_Sprash_OnCleanUp(actor)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("RestoreItemModel Air_Sprash_ReallyLeave\n");
	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	actor:EndWeaponTrail();
	return true;
end

function Air_Sprash_OnLeave(actor, action)

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

function Air_Sprash_OnFindTarget(actor,action,kTargets)
--ODS("Air_Sprash_OnFindTarget\n", false, 3851)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	local	iAttackRange = action:GetSkillRange(0,actor)
	
	return	Air_Sprash_OnFindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
end

function Air_Sprash_OnFindTargetEx(actor,action,iActionID,iAttackRange,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	arrow_worldpos = actor:GetPos();
	
-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir();
	kRotateAxis:Cross(Point3(0,0,1));
	kRotateAxis:Unitize();
	local	fRotateAngle = 60.0*3.141592/180.0;

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end

	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);		

	local	kFireTargetDir = actor:GetLookingDir();
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle);
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end

function Air_Sprash_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
    if bIsBefore == false then
		--  hit 키 이후라면
		--  바로 대미지 적용시켜준다.
		SkillHelpFunc_DefaultHitOneTime(actor,action);
   end

end
function Air_Sprash_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local action = actor:GetAction();
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		actor:AttachParticleS(3851, "p_ef_fire", "eff_sum_skill_smn_muzz02", 1.0)
			
		if actor:IsUnderMyControl() then
		
			local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");
		
			if bUseTLM then
				--ODS("CreateActionTargetList\n", false, 3851)
				action:CreateActionTargetList(actor);
				
				if IsSingleMode() then		        
					return;
				else
					local	kTargetList = action:GetTargetList();
					local	iTargetCount = kTargetList:size();
					--ODS("iTargetCount : "..iTargetCount.."\n", false, 3851)
					action:BroadCastTargetListModify(actor:GetPilot());
				end	
			end	
		end 
	end

	return	true;
end
