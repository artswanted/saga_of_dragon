-- [DIRECTION] : 이동 할 방향  "LEFT" or "RIGHT"
function Skill_BackStep_Shot_OnCastingCompleted(actor, action)
end

function Skill_BackStep_Shot_OnCheckCanEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kCurAction = actor:GetAction();
	if( false == kCurAction:IsNil() ) then
		local kActionName = kCurAction:GetID();	
		if kActionName ~= "a_back_step" then
			return false;
		end
	end
	
    return  true;
    
end

function Skill_BackStep_Shot_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

    ODS("Skill_BackStep_Shot_OnEnter\n");

	if actor:IsMyActor() == false then
		actor:ClearReservedAction()
	end
	
	action:SetParam(2,"");
	
	actor:SetComboCount(0);
	actor:StopJump();
	
	if actor:IsMyActor() == false then
		actor:SetTranslate(action:GetActionStartPos());
	end
	
	actor:SetMovingDelta(Point3(0,0,0));
			
	return true
end

function Skill_BackStep_Shot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local fAccel = -100 * frameTime
	local fVelocity = action:GetParamFloat(1)
	
	local kMovingDir = Point3(action:GetParamFloat(6),
	                            action:GetParamFloat(7),
	                            action:GetParamFloat(8));
	                            
	if fVelocity<0 then
		fVelocity = 0
	end
		                            
	kMovingDir:Multiply(-fVelocity)
	actor:SetMovingDelta(kMovingDir);
	
	ODS("BackStep_Shot fVelocity :"..fVelocity.."\n");

	fVelocity = fVelocity + fAccel
	action:SetParamFloat(1, fVelocity)

	local fElapsedTime = accumTime - action:GetParamFloat(0);
	if fElapsedTime > 0.5 or fVelocity <= 0 then
		if action:GetParam(3) == "FIRE" then
			action:SetParam(2,"end");
			return  false;
	    end
	    
	    if fVelocity< 0 then
			fVelocity = 0
			action:SetParam(2,"end");
			return  false;
		end
	end
	
	return	true;
end

function Skill_BackStep_Shot_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:EndBodyTrail();
    actor:FreeMove(false);

	return true
end

function Skill_BackStep_Shot_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());	
	
	local newActionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return  true;
	end

	if action:GetActionType()=="EFFECT"  then
		return true;
	end		
		
	if kCurAction:GetParam(2) == "end" then
		return	true;
	elseif 	newActionID == "a_lock_move" then
		return true;
	end

	return false
end

function Skill_BackStep_Shot_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	ODS("Skill_BackStep_Shot_OnEvent : "..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then

    	local kAction = actor:GetAction();
		
		CheckNil(nil==kAction);
		CheckNil(kAction:IsNil());
	
    	kAction:SetParam(3,"FIRE");

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Projectile_Default_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);	
			
			kArrow:Fire();
		else
			
			ODS("Arrow is nil\n");
		
		end
		
		actor:ClearIgnoreEffectList();
					
	end
end