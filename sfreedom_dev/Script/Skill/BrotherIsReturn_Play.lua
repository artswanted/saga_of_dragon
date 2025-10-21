-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID

function Skill_BrotherIsReturn_Play_OnEnter(actor, action)

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
	
	--	State
	action:SetParamInt(4,0)
	
	--	Hit Count
	action:SetParamInt(5,0)
	
	--	Last Hit Time
	action:SetParamFloat(6,0);

	action:SetParamFloat(7,0);

	action:SetParamInt(8,0);
	
	local kPos = actor:GetPos();
	kPos:SetZ(kPos:GetZ()-30);
	actor:AttachParticleToPoint(1,kPos,"ef_bang");
	
	actor:SetMovingDelta(Point3(0,0,0));
	actor:StopJump();
	actor:FreeMove(true);

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
	
	return true
end

function Skill_BrotherIsReturn_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local actionID = action:GetID();
	local iSlotNum = action:GetCurrentSlot();
	local fMoveSpeed  = 2000;
	local fMaxMoveDistance = action:GetParamInt(10)-40;
	local	iState = action:GetParamInt(4);
	
	if iSlotNum == 0 then
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		if fElapsedTime>=0.6 then
		
			actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
			
			action:SetParamFloat(0,accumTime);	--	이동 시작 시간 기록
			
			local	kPos = actor:GetPos();
			-- 현재 위치 기록
			action:SetParamFloat(1,kPos:GetX());
			action:SetParamFloat(2,kPos:GetY());
			action:SetParamFloat(3,kPos:GetZ());
		
			actor:PlayNext();
		end
	
	end
	
	if iSlotNum == 1 then
		local	fElapsedTime = accumTime - action:GetParamFloat(0);
		local	fMoveDistance = fElapsedTime * fMoveSpeed;

		if fMoveDistance>=fMaxMoveDistance then
			if iState ~= 2 then
				if action:GetParamInt(8) == 0 then
					if actor:IsUnderMyControl() then
						kAction:CreateActionTargetList(actor);
						kAction:BroadCastTargetListModify(actor:GetPilot());
					end
				
					Skill_BrotherIsReturn_Play_HitOneTime(actor,action)
					Skill_BrotherIsReturn_Play_HitOneTime(actor,action)
					Skill_BrotherIsReturn_Play_HitOneTime(actor,action)
				end

				if action:GetParamFloat(7) < fMaxMoveDistance then
					fMoveDistance = fMaxMoveDistance;
					local kMovingDir = Point3(action:GetParamFloat(11),action:GetParamFloat(12),action:GetParamFloat(13));
					kMovingDir:Multiply(fMoveDistance);
					action:SetParamFloat(7,fMaxMoveDistance);
					local kNextPos = Point3(action:GetParamFloat(1),action:GetParamFloat(2),action:GetParamFloat(3));
					kNextPos:Add(kMovingDir);
					actor:SetTranslate(kNextPos);
				end

				action:SetParamInt(4,2);
			end
			
			actor:EndBodyTrail();

			actor:PlayNext();
		else
			local kMovingDir = Point3(action:GetParamFloat(11),action:GetParamFloat(12),action:GetParamFloat(13));
			
			--ODS("Brother is Return Moving Dir : "..kMovingDir:GetX()..","..kMovingDir:GetY()..","..kMovingDir:GetZ().."\n");
			
			kMovingDir:Multiply(fMoveDistance);
			action:SetParamFloat(7,action:GetParamFloat(7)  + fMoveDistance);
			
			local kNextPos = Point3(action:GetParamFloat(1),action:GetParamFloat(2),action:GetParamFloat(3));
			kNextPos:Add(kMovingDir);
			
			actor:SetTranslate(kNextPos);
		
		end
	
	end
	
	if iState == 1 then
		local	fElapsedTime = accumTime - action:GetParamFloat(6);
		if fElapsedTime >= 0.1 then
			action:SetParamInt(8,1);
			if actor:IsUnderMyControl() then
				kAction:CreateActionTargetList(actor);
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end
		
			if Skill_BrotherIsReturn_Play_HitOneTime(actor,action) == false then
				return	false;
			end
		end
	
	elseif iState == 2 then
		if animDone and accumTime - action:GetParamFloat(6)>1.0 then
			actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.7);
			action:SetParamInt(4,3);
		end
	elseif iState == 3 then
		if accumTime - action:GetParamFloat(6)>2.0 then
			return false;
			
		end
	end
	

	return true
end


function Skill_BrotherIsReturn_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:EndBodyTrail();	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_BrotherIsReturn_Play_OnLeave(actor, action)
	return true;
end

function Skill_BrotherIsReturn_Play_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo, iHitCount)

	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	if actor == nil then
		return;
	end

	if actorTarget == nil then
		return;
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
	
	if kActionResult:GetCritical() then
		actorTarget:AttachParticleToPointS(12 + iHitCount, pt, "e_dmg_cri", 0.8)
	else
		actorTarget:AttachParticleToPointS(12 + iHitCount, pt, "e_dmg", 0.8)
	end	
	
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actorTarget:SetShakeInPeriod(5, iShakeTime/2);		
	
end

function Skill_BrotherIsReturn_Play_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(6,g_world:GetAccumTime());
	
	local	iHitCount = action:GetParamInt(5);
	
	ODS("Skill_BrotherIsReturn_Play_HitOneTime iHitCount:"..iHitCount.."\n");


	local kOriginalActorGUID = action:GetGUIDInContainer(0);
	local kOriginalActor = nil;
	if kOriginalActorGUID:IsNil() == false then

		local	kPilot = g_pilotMan:FindPilot(kOriginalActorGUID);
		if kPilot:IsNil() == false then
			kOriginalActor = kPilot:GetActor();
		end
	
	end	

	--local	iSkillLevel = action:GetParamInt(16);
	local	iTotalHit = 3;
		
	if iHitCount == iTotalHit-1 then
	
		local iTargetCount = action:GetTargetCount();
		local i =0;
		if iTargetCount>0 then
			
			while i<iTargetCount do
			
				local actionResult = action:GetTargetActionResult(i);
				if actionResult:IsNil() == false then
				
					action:SetParamInt(15,actionResult:GetValue()/(iTotalHit));
					local	iOneDmg = action:GetParamInt(15);
				
					actionResult:SetValue(actionResult:GetValue() - iOneDmg*(iTotalHit-1));
						
					local kTargetGUID = action:GetTargetGUID(i);
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
						Skill_BrotherIsReturn_Play_DoDamage(kOriginalActor,actorTarget,actionResult,action:GetTargetInfo(i), iHitCount);
					end
					
				end
				
				i=i+1;
			
			end
		end
				
		action:GetTargetList():ApplyActionEffects();
	
	else
	
		action:GetTargetList():ApplyOnlyDamage(iTotalHit,false, 6);
		
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
						
						Skill_BrotherIsReturn_Play_DoDamage(kOriginalActor,actorTarget,actionResult,action:GetTargetInfo(i), 1);
					end
				end
				
				i=i+1;
			
			end
		end
		
	end
	
	
	iHitCount = iHitCount + 1;
	
	if iHitCount == iTotalHit then
		action:SetParamInt(4,2)	--	
		return	true
	end
	
	action:SetParamInt(5,iHitCount);
	
	return	true;

end

function Skill_BrotherIsReturn_Play_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iSlotNum = action:GetCurrentSlot();

	ODS("textkey : ".. textKey .. " ".. iSlotNum.."\n");

	if action:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" then
		action:SetParamInt(4,1)	--	Start Hit

		Skill_BrotherIsReturn_Play_HitOneTime(actor,action);			
	end

	return	true;
end

function Skill_BrotherIsReturn_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

    if bIsBefore == false then

        if kAction:GetParamInt(3) == 1 then -- My actor			
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		local	iTotalHit = action:GetParamInt(16);
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or action:GetParamInt(2) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            Skill_BrotherIsReturn_Play_HitOneTime(actor,action);        
        end    
    end
end

function Skill_BrotherIsReturn_Play_OnCastingCompleted(actor, action)
end