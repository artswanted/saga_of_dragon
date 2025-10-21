-- slot 1 : ani 시작 시간
-- slot 2 : total ani time
-- slot 3 : CleanUp flag
-- TODO : blink가 점점 빨라지게 하자.
-- 죽는 ani/type을 좀 더 다양하게 만들자. (knockback이 된다던지.. 뒤로 쭉 밀린다던지..)

-- 죽었을 때 날아가는 시간
g_dyingThrowTime = 5
g_dyingTime = 2.5
g_dyingAlphaDelayTime = 1
function GetDyingTime(actor,action)

	if nil~=action then

		if action:GetID() == "a_voluntarily_die" then
			-- 자의에 의한 사망시 알파값 빠지는 시간
			return 1.0
		end

		local Time = actor:GetAnimationInfoFromAniName("DYING_TIME", action:GetCurrentSlotAnimName())
		if nil~=Time and false==Time:IsNil() then
			return tonumber(Time:GetStr()) --유지
		end

	end

	return g_dyingTime
end

function ChangeDieAnimation(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local aniTime = actor:GetAnimationLength(action:GetCurrentSlotAnimName())
	if 0 == aniTime then
		if actor:GetUseDieAnimation() then
			actor:SetUseSubAnimation(false)
		else
			if actor:GetExistDieAnimation() == true then
				actor:SetUseSubAnimation(true)
			end
		end
	end
end

function Act_Die_OnCheckCanEnter(actor,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:GetAction():GetID() == "a_telejump" then	--	텔레점프 중에 죽었을 경우, 텔레점프가 끝난 후에 죽도록 한다.
	
		if actor:GetAction():GetParam(10) =="Telejump_Die_OK" then
			return	true
		end
		actor:GetAction():SetParam(10,"Next_Die")
		return	false;
	elseif actor:GetAction():GetID() == "a_die" then
		return false
	elseif actor:GetAction():GetID() == "a_voluntarily_die" then
		return false
	end
	
	return	true;

end

function Act_Die_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("Act_Die_OnEnter actor : "..actor:GetID().." GUID :"..actor:GetPilotGuid():GetString().."\n");
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:SetAbil(517, 1);
	
	if( actor:IsUnitType(UT_SUB_PLAYER) ) then
		action:SetParamInt(20110623, 1) -- 격투가 Sub캐릭터 alpha 안먹임 
	else
		local kSubActor = actor:GetSubPlayer()
		if( false == kSubActor:IsNil() ) then	
			local kSubActorAction = kSubActor:GetAction()
			if( false == kSubActorAction:IsNil() ) then
				if(kSubActorAction:GetID() ~= "a_die") then
					TryMustChangeActorAction(kSubActor, "a_die")
				end
			end
		end
	end
	
	local iManualAction = actor:GetAbil(AT_MANUAL_DIEMOTION)
	if iManualAction > 0 then	
		if actor:GetAction():GetID() == "a_die" then	--또 죽는 액션이 오는것을 방지하자
			return false
		end
		ODS("Manual Die Motion ( " .. iManualAction .. " ) is Reserved!!\n")
		actor:ReserveTransitActionByActionNo(actor:GetAbil(AT_MANUAL_DIEMOTION));
		if 11002==actor:GetAbil(AT_MANUAL_DIEMOTION) then
			BreakObject(actor)
		end
		return false 
	end

	action:SetDoNotBroadCast(true)
	actor:Stop()
	actor:SetCanHit(false)
	
	if action:GetParam(1) == "DIE_IDLE" then
		action:SetSlot(1)
		ChangeDieAnimation(actor, action)
		return	true
	end
	
	--ODS("SkillNo : "..actor:GetAbil(AT_SKILL_SUMMONED_PARENT_SKILL).."\n", false, 3851);
	if nil~=g_world and false==g_world:IsNil() then
		local iOrderSkill = actor:GetAbil(AT_SKILL_SUMMONED_PARENT_SKILL)
		local heartPos = actor:GetNodeTranslate("p_ef_heart")
		if 300101001 == iOrderSkill then	--리게인
			local iDieEffectNo = g_world:AttachParticle("eff_sum_skill_smn_regain_01", heartPos)
			actor:SetParam("SUMMON_DIE_EFFECT", iDieEffectNo)
		elseif 300301101 == iOrderSkill then	--임팩트 언서먼
			local iDieEffectNo = g_world:AttachParticle("eff_sum_skill_mon_impect_unsummon_00", heartPos)
			actor:SetParam("SUMMON_DIE_EFFECT", iDieEffectNo)
		end
	end
	
	local iDieSlot = 0
	if action:GetID() == "a_voluntarily_die" then	-- 자의에 의한 사망인데 죽는 애니가 없는 경우 일반 다이로 애니재생
		local aniTime = actor:GetAnimationLength(action:GetSlotAnimName(0))
		if  aniTime == 0 then		--	죽기 애니가 없다면
			iDieSlot = 2
		end
	end
	action:SetSlot(iDieSlot)
	ChangeDieAnimation(actor, action)
	action:SetParamInt(4,0)	

	local actorID = actor:GetID()	
	if actorID ~= nil then
		-- 몬스터 이면..
		if string.sub(actorID, 1, 1) == "m" then
		
			-- 옵션이 켜져있으면  20%의 확률로 카메라 쪽으로 날아간다.
			local FlyToCameraOptionValue = Config_GetValue(HEADKEY_GAME, SUBKEY_FLY_TO_CAMERA)
			if  false == actor:IsUnitType(UT_SUMMONED) and
				1 == FlyToCameraOptionValue and actor:IsMeetFloor() and 0==actor:GetAbil(AT_SKILL_ON_DIE) and 
				0==actor:GetAbil(AT_NOT_FLY_TO_CAMERA) and
				actor:GetAbil(AT_GRADE) ~= 3 and
				actor:GetAbil(AT_GRADE) ~= 4 then 
				
				if math.random(1000)<200 then
				
					action:SetNextActionName("a_fly_to_camera");	--	카메라쪽으로 날려보내자
					action:ChangeToNextActionOnNextUpdate(true, true)
					return	true;
				
				end
			end

			-- if actor:GetExistDieAnimation() == true then
				-- actor:SetUseSubAnimation(true)
				-- actor:AttachSound(2000,"mon_die");
			-- end
		
			local heartPos = actor:GetNodeTranslate("p_ef_heart")
			local DieParticleNode = actor:GetDieParticleNode()
			if nil~=DieParticleNode and 0<string.len(DieParticleNode) then
				heartPos = actor:GetNodeTranslate(DieParticleNode)
			end

			
			local	die_particle_id = actor:GetDieParticleID();
			if die_particle_id == "" then
				die_particle_id = "e_transition_01"
			end

			actor:SetHideShadow(true)
			
			action:SetParamInt(0, 1)
			
			local aniTime = actor:GetAnimationLength(action:GetSlotAnimName(iDieSlot))
			if  aniTime == 0 then		--	죽기 애니가 없다면
				action:SetParamInt(4,2)
				actor:HideNode("Scene Root",true);	--	모습을 감추자
				actor:ReleaseAllParticles();

				local kDieSound = actor:GetDieSoundID()
				if nil==kDieSound or ""==kDieSound then
					kDieSound = "die_poof"
				end

				actor:AttachSound(2784,kDieSound)	-- 펑소리				
			end
			action:SetParamFloat(5,aniTime)
			actor:AttachParticleToPointS(436, heartPos, die_particle_id, actor:GetDieParticleScale())
		end
	end
		
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamFloat(1, g_world:GetAccumTime())
	action:SetParamInt(3,0)
	action:SetParamFloat(2, GetDyingTime(actor,action))	--	알파를 빼는 시간
	action:SetParamFloat(0,g_dyingAlphaDelayTime);	--	죽기 애니가 끝난 후, 알파를 빼기전까지의 시간
	Act_Die_InitAlpha(action)

	if true == IsSlowMotionDie( actor ) then
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
												, g_fChargeFocusFilterValue[2]
												, g_fChargeFocusFilterValue[3]
												, 0.2
												,true
												,true)
		g_world:SetUpdateSpeed(0.0008)
		action:SetParamInt(8,1)
		action:SetParamFloat(9,GetAccumTime())
		
		PlaySoundByID( "PVP_Battle_Hit" )
	end
	
	if action:GetParamInt(4) == 0 then
	
		if actor:IsMeetFloor() == false then	--	공중에서 죽었다면, 일단 땅으로 떨어뜨리자
		
			action:SetParamInt(4,3)
			actor:FreeMove(true)
		
		end
	
	end
	return true
end

function Act_Die_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local action = actor:GetAction()
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local startTime = action:GetParamFloat(1)
	local totalalphafadeoutTime = action:GetParamFloat(2)	
	local elapsedTime = accumTime - startTime
	local	iDieState = action:GetParamInt(4)
	local	alphadelaytime = action:GetParamFloat(0);
	local	aniTime = action:GetParamFloat(5)
	
	if action:GetCurrentSlot() == 1 then
		return	true
	end
	
	actor:DoDropItems();

	if Act_Die_IsAlpha(action) then
		if( 1 == action:GetParamInt(20110623) ) then 
			return true
		end
		if actor:IsMyActor() == false then
			Act_Die_DoAlpha(actor,action)
		end
		return	true;
	end
		
	if iDieState == 0 then	--	죽기 모션이 끝나기를 기다린다		
		if actor:GetPilot():IsPlayerPilot() == false then
			if actor:IsAnimationDone() == true or elapsedTime > aniTime * 2 then	
				action:SetParamFloat(1,accumTime);
				Act_Die_StartAlpha(actor,action)
			end
			return	true;
		end
		
	elseif iDieState == 2 then	--	알파가 다 빠지기를 기다린다

		if elapsedTime > totalalphafadeoutTime + alphadelaytime + aniTime then
			if actor:GetPilot():IsPlayerPilot() == false then
				if actor:GetAbil(AT_MANUAL_DIEMOTION) == 0 then	-- 0은 서버에서 죽임, 101은 죽이지 않음(후처리 필요)
				
					g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
					action:SetParamInt(3,1);
					
				end
			end		
			return false
		end
	
	elseif iDieState == 3 then	-- 땅으로 떨어지기
	
		if actor:IsMeetFloor() then
			actor:FreeMove(false);
			action:SetParamInt(4,0)
			return	true
		end
		
		local	fZ_Velocity_Init = -0;
		local	fTotalElapsedTime = accumTime - action:GetActionEnterTime();
		
		local	fSpeed =g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
	
		actor:SetMovingDelta(Point3(0,0,fSpeed));	
	
	end
	
	if 1 == action:GetParamInt(8) then
		local	fElapsedTime = GetAccumTime() - action:GetParamFloat(9);
		local	fMaxSpeedTime = 4.0
		local	fRate = fElapsedTime/fMaxSpeedTime;
		if fRate> 1 then
			fRate = 1
		end
		
		local fSpeed = fRate;
		
		if fRate<3.5 then
			fSpeed = fSpeed*0.3
		end
		
		if fRate == 1.0 then
			g_world:SetUpdateSpeed(1)
			action:SetParamInt(8,0)
			
			if true == IsSlowMotionDie( actor ) then
				g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
														, g_world:GetWorldFocusFilterAlpha()
														, 0
														, 0.2
														,false
														,true)
			end
		else 
			g_world:SetUpdateSpeed(fSpeed*0.400)
		end
	end
	
	if( actor:IsMyActor() == true 
		and actor:GetAbil(AT_HP) > 0
	) then -- 간혹 HP 차있는데 죽는 현상이 있어서 우회 수정
		TryMustChangeActorAction(actor, "a_idle");
		actor:AttachParticle(20120612,"char_root", "ef_unrivaled_char_root");		
		CloseUI("FRM_DEAD_MISSION");
		CloseUI("FRM_BS_BATTLE_REBIRTHTIME");
		CloseUI("DeadDlg_NoRevive");
		CloseUI("FRM_DEAD_SG");
		CloseUI("DeadDlg");
		return false;
	end
	
	return true
end

function Act_Die_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	g_world:SetUpdateSpeed(1)
	
	if actor:GetAbil(AT_GRADE) == EMGRADE_BOSS then
		g_world:OnBossDie()
	end
	
	local iDieEffectNo = tonumber(actor:GetParam("SUMMON_DIE_EFFECT"))
	if nil~= iDieEffectNo and 0 < iDieEffectNo then
		g_world:DetachParticle(iDieEffectNo)
	end
		
	if true == IsSlowMotionDie( actor ) then
		g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
														, g_world:GetWorldFocusFilterAlpha()
														, 0
														, 0.2
														,false
														,true)
	end
	actor:FreeMove(false);
end

function Act_Die_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if action:GetID() == "a_fly_to_camera" then
		return	true;
	end
	-- 몬스터라면 제거한다.
	actor:DetachFrom(436);

	local kCuAction = actor:GetAction()
	if( CheckNil(kCuAction == nil) ) then return false end
	if( CheckNil(kCuAction:IsNil()) ) then return false end	
	
	if actor:GetPilot():IsPlayerPilot() == false and 
		kCuAction:GetParamInt(3) == 0
	then
		if actor:GetAbil(AT_MANUAL_DIEMOTION) == 0  -- 0은 서버에서 죽임, 101은 죽이지 않음(후처리 필요)
			and 0 == kCuAction:GetParamInt(20110623) 
		then
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		end
	end
	
	if action:GetID() == "a_revive" or action:GetID() == "a_Resurrection_01" then
		return	true;
	end
	
	return false
end

function Act_Die_OnEvent(actor,textKey)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	ODS("Act_Die_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
		
	local iOrderSkill = actor:GetAbil(AT_SKILL_SUMMONED_PARENT_SKILL)
	
	if textKey == "alpha_end" then
		Act_Die_StartAlpha(actor, actor:GetAction())
	elseif 300101001 == iOrderSkill and textKey == "regain_end" then	--리게인
		local iDieEffectNo = tonumber(actor:GetParam("SUMMON_DIE_EFFECT"))
		if nil~= iDieEffectNo and 0 < iDieEffectNo then
			g_world:DetachParticle(iDieEffectNo)
		end
	elseif 300301101 == iOrderSkill and textKey == "impact_eff00" then	--임팩트 언서먼
		local iDieEffectNo = tonumber(actor:GetParam("SUMMON_DIE_EFFECT"))
		if nil~= iDieEffectNo and 0 < iDieEffectNo then
			g_world:DetachParticle(iDieEffectNo)
		end
		local heartPos = actor:GetNodeTranslate("p_ef_heart")
		g_world:AttachParticle( "eff_sum_skill_mon_impect_unsummon_01", heartPos)
		actor:AttachSound(38513, "sum_impact_unsummon")
	elseif textKey == "hit" or textKey == "fire" then
		if not actor:IsUnitType(UT_PLAYER) then
			Act_Mon_Melee_DoDamage(actor,actor:GetAction())
		end
		
		SkillFunc_QuakeCamera(actor:GetAction(), 6)		 --Hit 메세지때 quake camera를 쓰는가
	end

	return true;
end

function Act_Die_InitAlpha(action)
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	action:SetParamInt(67,0)
end

function Act_Die_StartAlpha(actor,action)
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local iUseAlpha = action:GetParamInt(67)
	if 0~=iUseAlpha then
		return
	end

	action:SetParamFloat(1,g_world:GetAccumTime());
	action:SetParamInt(67,1)
	
	local Dont = actor:GetAnimationInfoFromAniName("DONT_RELEASE_PARTICLE", action:GetCurrentSlotAnimName())
	if Dont:GetStr()=="" or Dont:GetStr()=="0" then
		actor:ReleaseAllParticles();
	end
end

function Act_Die_IsAlpha(action)
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	return 0 ~= action:GetParamInt(67)
end

function Act_Die_DoAlpha(actor,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	action:SetParamInt(67,0)
	actor:SetTargetAlpha(actor:GetAlpha(), 0.0, action:GetParamFloat(2));
	action:SetParamInt(4,2);
end

function Act_Die_OnTargetListModified(actor,action,bIsBefore)
	
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
