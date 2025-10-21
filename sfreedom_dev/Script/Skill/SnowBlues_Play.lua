
function Skill_SnowBlues_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	SkillFunc_InitUseDmgEffect(action);
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());	
	action:SetParamFloat(2,g_world:GetAccumTime());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );

	action:SetSlot(0);
	actor:PlayCurrentSlot();
	action:SetParamInt(3, 0);
	action:SetParamFloat(8, 90);
	action:SetParamFloat(10,g_world:GetAccumTime());	--데미지 시간

	--actor:AttachParticleToPoint(7212,actor:GetPos(),"ef_SnowBlues_01");
	actor:AttachParticle(7212,"char_root","ef_SnowBlues_01");
	
	return true
end

function Skill_SnowBlues_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_SnowBlues_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        --Skill_SnowBlues_DoDamage(actor,action,0,false);
		--action:GetTargetList():ApplyActionEffects();    
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_SnowBlues_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iCurrentSlot = action:GetCurrentSlot();
	local iState = action:GetParamInt(3);
	
	local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	if fMoveSpeed == 0 then
	    fMoveSpeed = action:GetParamFloat(8);
	end
	
	if iState == 0 then
		kMoveDir:Multiply(fMoveSpeed);	
		actor:SetMovingDelta(kMoveDir);

		local   fElapsedTIme = accumTime - action:GetParamFloat(2);
		if fElapsedTIme>0.1 then
			action:GetParamFloat(2, accumTime);
			action:SetParamFloat(8, action:GetParamFloat(8) - 0.5);
			
			if 0 > action:GetParamInt(8) then
				action:SetParamInt(8, 0);
			end
		end
	

		if iCurrentSlot == 0 then --걷는 상태
			local   fElapsedTIme = accumTime - action:GetParamFloat(1);
			if fElapsedTIme>0.6 then	
				action:SetParamFloat(1,accumTime);
				action:SetSlot(1);
				actor:PlayCurrentSlot();
			end
		elseif  iCurrentSlot == 1 then --넘어지는 상태
			if animDone == true then
				action:SetParamFloat(1,accumTime);
				action:SetSlot(2);
				actor:PlayCurrentSlot();
			end
		end
			local   fElapsedTIme10 = accumTime - action:GetParamFloat(10)
			if fElapsedTIme10>0.3 then
		
				action:SetParamFloat(10,accumTime)
				action:CreateActionTargetList(actor);
			    
				if IsSingleMode() or action:GetTargetCount() == 0 then
			    
					Skill_SnowBlues_Play_OnTargetListModified(actor,action,false);
			    
				else
			    
					action:BroadCastTargetListModify(actor:GetPilot());
					action:ClearTargetList();
			    
				end
			end

		
	elseif iState == 1 then
		local   fElapsedTIme = accumTime - action:GetParamFloat(1);
		if fElapsedTIme>0.3 then
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.5);
			action:SetParamInt(3, 2);
		end
	end

	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 2.0;
    end	

    local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);

	if iState == 0 and iCurrentSlot == 2 then --마지막에 자연스럽게 알파로 사라지도록 처리
		if fLifeTime - fTotalElapsedTime < 3.6 then			
			action:SetParamInt(3, 1);
			actor:SetHideShadow(true);
		end
	end
    

    if fTotalElapsedTime > fLifeTime then
        return  false
    end	

	return true
end

function Skill_SnowBlues_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end

	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_SnowBlues_Play_OnLeave(actor, action)
	return true;
end
--[[
function Skill_SnowBlues_DoDamage(actor, action,fAttackHoldingTime,bNoHold)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	local	iTargetCount = action:GetTargetCount();

	if 	iTargetCount == 0 then
		return true;
	end

	local	i = 0;
	
	if bNoHold == nil then
	    bNoHold = false
	end
		
	if fAttackHoldingTime == nil or fAttackHoldingTime == 0 then
		fAttackHoldingTime = g_fAttackHitHoldingTime;
	end
	
	local bWeaponSoundPlayed = false;
	
	while i<iTargetCount do
		
		local	kTargetGUID = action:GetTargetGUID(i);
		local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			
			local	actorTarget = kTargetPilot:GetActor();
			if actorTarget:IsNil() == false and actorTarget:GetPilotGuid() ~= actor:GetPilotGuid() then
			
				local kActionResult = action:GetTargetActionResult(i);
				
				if kActionResult:IsMissed() == false then
	
	                if bWeaponSoundPlayed == false then
	                    bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID()
						if kActionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
						actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                    end	
                    				
--					local	kActionTargetInfo = action:GetTargetInfo(i);	
					
					local	iSphereIndex = action:GetTargetABVShapeIndex(i);
					local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);		

					local bCri = kActionResult:GetCritical()
					local DamageEff = actor:GetDamageEffectID(bCri)
					if nil==DamageEff or 0==string.len(DamageEff) then
						if bCri then
							DamageEff = "e_dmg_cri"
						else
							DamageEff = "e_dmg"
						end
					end
					actorTarget:AttachParticleToPoint(12, pt, DamageEff)
					--actorTarget:AttachParticleToPointS(13, actorTarget:GetNodeWorldPos("char_root"), "ef_Blizzard_00002",2.0)
					actorTarget:AttachParticleToPointWithRotate(13, actorTarget:GetNodeWorldPos("char_root"),"ef_Blizzard_00002", Quaternion(math.random(0,360), Point3(0,0,1)), 2);

--					local	atAction = actorTarget:GetAction()
					
--					local	guidPilot = actorTarget:GetPilotGuid()
--					local	pilot = g_pilotMan:FindPilot(guidPilot)
					
					local iHoldingTime = fAttackHoldingTime * 1000;
					local	iShakeTime = g_fAttackHitHoldingTime * 1000;
					
					-- 충격 효과 적용
					if bNoHold == false then
					    actor:SetAnimSpeedInPeriod(0.00001,iHoldingTime);
					end
					
					actor:SetShakeInPeriod(5,iShakeTime/2);
					actorTarget:SetShakeInPeriod(5,iShakeTime);		
					
				end

			end
		end
		
		i = i + 1;
		
	end
	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects();
	end
	action:ClearTargetList();
end
]]
function Skill_SnowBlues_Play_OnCastingCompleted(actor, action)
end