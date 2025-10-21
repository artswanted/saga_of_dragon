-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID

function Skill_BrotherIsGone_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	
	local kPos = actor:GetPos();
	kPos:SetZ(kPos:GetZ()-30);
	actor:AttachParticleToPoint(1,kPos,"ef_bang");
	
	action:SetParamInt(2,0);	--	발차기 횟수	
	action:SetParamInt(4,0);
	action:SetParamInt(5,0);

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
	
	actor:FreeMove(true)
	return true
end

function Skill_BrotherIsGone_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local actionID = action:GetID();
	local iSlotNum = action:GetCurrentSlot();
	local iState = action:GetParamInt(4);
	
	if iSlotNum == 0 then
	
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		if fElapsedTime>=0.5 then
			actor:PlayNext();
			actor:AttachParticle(127, "char_root", "ef_skill_brother_is_gone")
		end
	
	end
	
	if iSlotNum == 1 then	
		if animDone == true and iState == 0 then	
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end	
			action:SetParamInt(4,1);
			action:SetParamFloat(6,g_world:GetAccumTime());
		end
	end

	if iState == 1 then
		if accumTime - action:GetParamFloat(6) > 0.4 then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			action:SetParamFloat(6,g_world:GetAccumTime());
			actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.7);
			action:SetParamInt(4,2);
		end
	elseif iState == 2 then
		if accumTime - action:GetParamFloat(6) > 1.0 then
			return false;
		end
	end

	return true;
end

function Skill_BrotherIsGone_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_BrotherIsGone_Play_OnLeave(actor, action)
	return true;
end

function Skill_BrotherIsGone_Play_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)

	if actor == nil or actorTarget == nil then
		return;
	end
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	if kActionResult:IsMissed() == true then
		return
	end

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
--	local actionID = kAction:GetID();	

	local	iSphereIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iSphereIndex);
	pt:SetX(pt:GetX() + math.random(-5,5))
	pt:SetY(pt:GetY() + math.random(-5,5))
	pt:SetZ(pt:GetZ() + math.random(-5,5))
	
	if kActionResult:GetCritical()  then
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
	else
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
	end	
	
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actorTarget:SetShakeInPeriod(5, iShakeTime/2);		
	
end
function Skill_BrotherIsGone_Play_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetActionParam() == AP_CASTING then
		return true;
	end
	
	local	iTotalHit = 15
	local	iHitCount = action:GetParamInt(5);
	local	iRealDamage = 0;
	local	iTotalRealHit = 8;
	local	iRealHit = action:GetParamInt(2);

	iRealDamage = iHitCount % 2;
	ODS("iRealDamage : ".. iRealDamage.."    HitCount : "..iHitCount .."\n")
	
	if textKey == "hit" then
		--hit 키가 15개로 되어있다 현재. 실제로는 8발만 때려야 하므로 아래와 같이 처리한다.
		if iHitCount == 0 or iRealDamage == 1 then
			if iRealHit == iTotalRealHit - 1 then

				local iTargetCount = action:GetTargetCount();
				local i =0;
				if iTargetCount>0 then
					
					while i<iTargetCount do
					
						local actionResult = action:GetTargetActionResult(i);
						if actionResult:IsNil() == false then
						
							action:SetParamInt(15,actionResult:GetValue()/(iTotalRealHit));
							local	iOneDmg = action:GetParamInt(15);
						
							actionResult:SetValue(
								actionResult:GetValue() - iOneDmg*(iTotalRealHit-1));
								
							local kTargetGUID = action:GetTargetGUID(i);
							local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
							if kTargetPilot:IsNil() == false then
								
								local actorTarget = kTargetPilot:GetActor();
								
								Skill_BrotherIsGone_Play_DoDamage(kOriginalActor,actorTarget,actionResult,action:GetTargetInfo(i));
							end
							
						end
						
						i=i+1;
					
					end
				end
						
				action:GetTargetList():ApplyActionEffects();
		
			else

				action:GetTargetList():ApplyOnlyDamage(iTotalRealHit);
				
				local iTargetCount = action:GetTargetCount();
				local i =0;
				if iTargetCount>0 then
					
					while i<iTargetCount do
					
						local actionResult = action:GetTargetActionResult(i);
						if actionResult:IsNil() == false then
						
							local kTargetGUID = action:GetTargetGUID(i);
							local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
							if kTargetPilot:IsNil() == false then
								
								local actorTarget = kTargetPilot:GetActor();
								
								Skill_BrotherIsGone_Play_DoDamage(kOriginalActor,actorTarget,actionResult,action:GetTargetInfo(i));
							end
						end
						
						i=i+1;
					
					end
				end
			end	

			action:SetParamInt(2, iRealHit + 1);
		end	

		iHitCount = iHitCount + 1;		
		action:SetParamInt(5,iHitCount);
	end

	return	true;
end

function Skill_BrotherIsGone_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	--형 왔다 와 형 간다는 같은 패턴이므로 형간다의 함수를 호출해서 사용
	Skill_BrotherIsReturn_Play_OnTargetListModified(actor,action,bIsBefore)
end


function Skill_BrotherIsGone_Play_OnCastingCompleted(actor, action)
end