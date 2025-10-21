-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function Skill_DeathStinger_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_DeathStinger_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_DeathStinger_Fire(actor,action);
	action:SetSlot(0);
	actor:PlayCurrentSlot();
	actor:AttachParticle(7212,"char_root","ef_skill_death_stinger_02_char_root"); 
end
function Skill_DeathStinger_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
--	action:CreateActionTargetList(actor);
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	action:SetParamFloat(2,actor:GetPos():GetX());	--	시작 위치 기록
	action:SetParamFloat(3,actor:GetPos():GetY());	--	시작 위치 기록
	action:SetParamFloat(4,actor:GetPos():GetZ());	--	시작 위치 기록
	action:SetParamFloat(5,g_world:GetAccumTime());	--	시작 시간 기록
	action:SetParamInt(6,0)	--	Step	
		
	actor:FreeMove(true);
	actor:StopJump();
	actor:SetMovingDelta(Point3(0,0,0));
end

function Skill_DeathStinger_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID()
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_DeathStinger_OnCastingCompleted(actor,action);

	-- 시작 위치 저장
	action:SetParamAsPoint(7, actor:GetPos());

	actor:StopJump()

	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000653, 0) -- sa_iceshower
	if false == pilot:IsNil() then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		local kBird = pilot:GetActor()
		if false == kBird:IsNil() then
			local kTrans = actor:GetTranslate()
			kBird:FreeMove(true)
			kTrans:SetZ(kTrans:GetZ()+5)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AddActor(guid, kBird, kTrans, 9);
			pilot:GetUnit():SetCaller(actor:GetPilotGuid())
			local	kLookAt = actor:GetLookingDir()
			kBird:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
			kBird:ReserveTransitAction("a_DeathStingerSA")
			pilot:GetActor():ClearActionState();
			if actor:IsMyActor() then
	        	pilot:GetActor():SetUnderMyControl(true);
		    end

			local fScale = (action:GetSkillLevel()%10)*0.05 + 1.0	--스킬레벨에 따라 커지게
			kBird:SetTargetScale(fScale, 0.56)
			action:SetParam(123,guid:GetString())	--GUID 저장
			g_pilotMan:InsertPilot(guid, pilot)
		end
	end

	action:SetParamInt(122,0)
	action:SetParamInt(121,0)

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end
	
	return true
end

function Skill_DeathStinger_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local	iStep = action:GetParamInt(6);

	local curAnimSlot = action:GetCurrentSlot();

	if curAnimSlot == 0 then
		if animDone == true then
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
			local	iBaseClassID = actor:GetPilot():GetBaseClassID()
			if iBaseClassID == CT_FIGHTER then
				actor:AttachParticle(493,"char_root","ef_fig_tackle_char_root");
			elseif iBaseClassID == CT_MAGICIAN then
				actor:AttachParticle(494,"char_root","ef_Lightningamor_01_char_root");
			end

			local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
			if kSoundID~="" then
				actor:AttachSound(2783,kSoundID);
			end	
		end	
	else
		if action:GetActionParam() == AP_FIRE then
			local	kStartPos = Point3(action:GetParamFloat(2),action:GetParamFloat(3),action:GetParamFloat(4));
			local	fMoveSpeed = 600;
			   
			local kNewPos = actor:GetPos();
			local kOldPos = action:GetParamAsPoint(7);
			kOldPos:SetZ(0)
			kNewPos:SetZ(0); --Z를 무시하고 계산한다. 떨어지는 것은 중력에 의한 것

			local fDistance = kNewPos:Distance(kOldPos);
			local fMaxDistance = action:GetAbil(AT_CASTER_MOVERANGE)
			if 0==fMaxDistance then
				fMaxDistance=144
			end

			--이동한 거리가 있을 경우
			local AttackCount = action:GetParamInt(121)
			if 0.0 <= fDistance then
				--남은 이동 거리
				local fDistance2 = fMaxDistance - fDistance;
				if 1>=fDistance2 then
					local guid = GUID(action:GetParam(123))
					local Death = g_pilotMan:FindPilot(guid)
					if false==Death:IsNil() and false==Death:GetActor():IsNil() then
						Death:GetActor():EndBodyTrail()
					end
				end

				--두 벡터를 이용하여 방향 벡터를 구하고
				--방향 벡터를 이용하여 실제로 이동하는 곳의 위치를 구한다.
				local kDirection = kNewPos:_Subtract(kOldPos);
				kDirection:Unitize();
				kDirection:Multiply(fMoveSpeed * frameTime);
				kDirection:Add(kNewPos);

				--이번 프레임에 이동 하는 거리
				local fDistance3 = kDirection:Distance(kOldPos);

				--이미 최대 대쉬 이동거리 거리를 벗어난 경우
				if fMaxDistance - fDistance3 < 0 then
					--fMoveSpeed값을 다시 세팅해야 한다.				
					fMoveSpeed = fDistance2 / frameTime;
				end
				local iRate = math.floor((fDistance/fMaxDistance)*100/20)
				if 5>AttackCount then
					if iRate ~= action:GetParamInt(122) or iRate >= AttackCount then
					    action:CreateActionTargetList(actor);
						if action:GetTargetCount()>0 then
						    action:BroadCastTargetListModify(actor:GetPilot());
							action:SetParamInt(121,action:GetParamInt(121)+1)
						end
						action:ClearTargetList()
						action:SetParamInt(122,iRate)
					end
				end
			end
		
			local kMovingDir = actor:GetLookingDir()
			kMovingDir:Multiply(fMoveSpeed);
			
			actor:SetMovingDelta(kMovingDir);
			
			local	fCurrentDistance = kStartPos:Distance(actor:GetPos());
			local	fElapsedTime = accumTime - action:GetParamFloat(5);

			ODS("fCurrentDistance : " .. fCurrentDistance .. " fMaxDistance : " .. fMaxDistance .. " fElapsedTime : " .. fElapsedTime);
			if fCurrentDistance >= fMaxDistance or fElapsedTime > 1 then

				action:SetParamInt(6,1);
				action:SetParamFloat(5,accumTime);
--				Skill_DeathStinger_DoDamageToTargets(actor);
				
				actor:DetachFrom(493);
				
				local curActionParam = action:GetParam(0)
				if curActionParam == "GoNext" then
					if 2==curAnimSlot then
						return false==animDone	--아직 애니 안끝났으면
					elseif 1==curAnimSlot then
						actor:PlayNext()
						local guid = GUID(action:GetParam(123))
						local Death = g_pilotMan:FindPilot(guid)
						if false==Death:IsNil() and false==Death:GetActor():IsNil() then
							Death:GetActor():PlayNext()
						end

						action:SetParam(0, "null")
						action:SetParam(1, "end")
						return true
					end
					nextActionName = action:GetParam(1);

					action:SetParam(0, "null")
					action:SetParam(1, "end")
					
					action:SetNextActionName(nextActionName)
					return false;
				else
					if true==animDone then
					end
					if 2==curAnimSlot then
						if false==animDone then

							action:SetParam(0, "null")
							action:SetParam(1, "end")
							return true	--아직 애니 안끝났으면
						end
					elseif 1==curAnimSlot then
						actor:PlayNext()
						actor:PlayCurrentSlot()
						local guid = GUID(action:GetParam(123))
						local Death = g_pilotMan:FindPilot(guid)
						if false==Death:IsNil() and false==Death:GetActor():IsNil() then
							Death:GetActor():PlayNext()
						end
						action:SetParam(0, "null")
						action:SetParam(1, "end")
						return true
					end
					action:SetParam(1, "end")
					return false
				end						
			
			end
		end
	end

	return true
end

function Skill_DeathStinger_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	actor:DetachFrom(493);
	actor:DetachFrom(494);
	actor:DetachFrom(7212);	
	actor:EndBodyTrail();
	actor:FreeMove(false);

	Skill_DeathStinger_RemoveEntity(actor:GetAction())
	
	return true;
end

function Skill_DeathStinger_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()

	ODS("curParam : " .. curParam .. "\n", false, 1509)
	
	if actor:IsMyActor() == false then
		return true;
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
		return false;
	end

	return false 
end

function Skill_DeathStinger_RemoveEntity(curAction)
	local guid = GUID(curAction:GetParam(123))
	local Death = g_pilotMan:FindPilot(guid)	--키반응으로 액션이 끊겼을 경우 엔티티는 끝나는 액션
	if false==Death:IsNil() and false==Death:GetActor():IsNil() then
		Death:GetActor():GetAction():SetSlot(2)
		Death:GetActor():PlayCurrentSlot()
	end
end

function Skill_DeathStinger_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
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

	local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
	local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
	
	if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
		local nUnitSize = actorTarget:GetAbil(AT_UNIT_SIZE);
		local nEffectScale = 1.0;
		if 1 == nUnitSize then --1:Small
			nEffectScale = 1.0;
			ODS("Small Monster\n");
		elseif 2 == nUnitSize then --2:Middle
			nEffectScale = 1.5;
			ODS("Middle Monster\n");
		elseif 3 == nUnitSize then --3:Large
			nEffectScale = 2.0;
			ODS("Large Monster\n");
		end
		
		actorTarget:AttachParticleS(12,kHitTargetEffectTargetNodeID,kHitTargetEffectID, nEffectScale);
	else
		if kActionResult:GetCritical() then
			actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
		else
			actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
		end	
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
function Skill_DeathStinger_DoDamageToTargets(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	kAction = actor:GetAction();

	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());


	local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end

	local iTargetCount = kAction:GetTargetCount();
	if iTargetCount>0 then

		local i = 0;
		while i<iTargetCount do

			local kActionResult = kAction:GetTargetActionResult(i);
			if kActionResult:IsNil() == false then

				local kTargetGUID = kAction:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and kActionResult:IsMissed() == false then

					local actorTarget = kTargetPilot:GetActor();
					Skill_DeathStinger_DoDamage(actor,actorTarget,kActionResult,kAction:GetTargetInfo(i));

				end
			end

			i=i+1;

		end

	end

	if kAction:GetTargetList():IsNil() == false then
		kAction:GetTargetList():ApplyActionEffects();
	end
	kAction:ClearTargetList();	

end


function Skill_DeathStinger_OnEvent(actor,textKey)

	return	true;
end

function Skill_DeathStinger_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if bIsBefore == false then
		--  때린다.
		Skill_DeathStinger_DoDamageToTargets(actor)   --  타격 효과
		action:GetTargetList():ApplyActionEffects();    
	end
end
