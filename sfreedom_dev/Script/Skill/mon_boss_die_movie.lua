
function Act_Boss_Die_Movie_OnCastingCompleted(actor, action)
end

function Act_Boss_Die_Movie_OnEnter(actor, action)
	local ifirst = action:GetParamInt(20)
	if ifirst == 1 then
		return true
	end
	
	local kEnding = actor:GetParam("ending")
	if kEnding == "start" then
		return true
	end

	action:SetParamInt(20,1)

	return true
end

function Act_Boss_Die_Movie_Start(actor, action)
	local movie = actor:GetAnimationInfo("DIE_MOVIE")
	if nil~=movie then
		if ""~=movie then
			g_MovieMgr:Play(movie)
			actor:SetParam("ending", "start")
			return true
		end
	end
	
	return false
end
function Act_Boss_Die_Movie_Ready()
	LockPlayerInput(8)
end

function Act_Boss_Die_Movie_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	local action = actor:GetAction()

	local kEnding = actor:GetParam("ending")
	if kEnding == "start" then	--시작 된 상태에서 
		if g_MovieMgr:IsPlay() == false then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
			return false
		end
	end

	if actor:IsAnimationDone() == true then
		Act_Boss_Die_Movie_Ready()
		actor:SetHide(true);
		actor:SetHideShadow(true);
		Act_Boss_Die_Movie_Start(actor, action)
		action:SetSlot(1)
		actor:PlayCurrentSlot()
	end

	return true
end



function Act_Boss_Die_Movie_OnCleanUp(actor, action)
	UnLockPlayerInput(8)
	actor:SetParam("ending", "")
end

function Act_Boss_Die_Movie_OnEvent(actor,textKey)
	return true
end

function Act_Boss_Die_Movie_OnLeave(actor, action)
	if actor == nil then
		return true
	end
	
	if g_MovieMgr:IsPlay() == false then
		return true
	end

	return false
end
