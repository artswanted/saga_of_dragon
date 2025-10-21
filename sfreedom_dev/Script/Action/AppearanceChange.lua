function Act_AppearanceChange_OnEnter(actor, action)
	if( CheckNil(actor==nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(action==nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:ResetAnimation();
	return true
end

function Act_AppearanceChange_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(actor==nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	local	bAnimDone = actor:IsAnimationDone()
	
	if bAnimDone then
	
		return	false;
	
	end

	return true;
end

function Act_AppearanceChange_OnCleanUp(actor, action)
	if( CheckNil(actor==nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	-- if( CheckNil(action==nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	local	kCurrentAction = actor:GetAction();
	if( CheckNil(kCurrentAction==nil) ) then return false end
	if( CheckNil(kCurrentAction:IsNil()) ) then return false end
	if kCurrentAction == nil or kCurrentAction:IsNil() then
		return true
	end
	
	local	kKFMPath = kCurrentAction:GetParam(0);
	actor:ChangeKFM(kKFMPath,true);
	
	local iAttachType = kCurrentAction:GetParamInt(1);
	if(0 ~= iAttachType) then		
		--ODS("KFM변화후 붙여야할 이펙트가 있다면\n",false, 912)
		local EffectID = kCurrentAction:GetParam(2);
		if("" ~= EffectID) then		
			local fScale = 1			
			if(1 == iAttachType) then
				local TargetNode = kCurrentAction:GetParam(3);
				actor:AttachParticleS(20100401, TargetNode, EffectID, fScale)
				--ODS("노드에 붙음\n",false, 912)
			elseif(2 == iAttachType) then
				local kPos = kCurrentAction:GetParamAsPoint(4);
				kPos:Add(actor:GetPos())
				actor:AttachParticleToPointS(20100401, kPos, EffectID, fScale)
				--ODS("위치에 붙음\n",false, 912)
			end
		end
	end
	
	return true
end

function Act_AppearanceChange_OnLeave(actor, action)
	return true
end
