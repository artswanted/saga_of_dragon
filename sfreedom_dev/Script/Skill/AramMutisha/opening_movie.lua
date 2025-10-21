function Act_Opening_Movie_OnEnter(actor, action)
   	--g_world:SetCameraModeMovie2(actor, 2, 1000081, "nop()")
	--g_world:SetCameraMovieTranslate(Point3(0,0,-1500))
	--ODS("Act_Opening_Movie_OnEnter",false,987)

	action:SetSlot(0)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(1,0)
	return true
end

function Act_Opening_Movie_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end
	-- local action = actor:GetAction()
	-- if 0==action:GetParamInt(1) then
		-- g_world:SetCameraModeMovie(2, 1000081, "nop()")
		-- g_world:SetCameraMovieTranslate(Point3(0,0,-1500))
		-- action:SetParamInt(1,1)
	-- end
	action = actor:GetAction()		-- 지금 엑션 객체를 반환
	curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if curSlot == 0 and actor:IsAnimationDone() then	-- 처음 에니이고 끝났다면
		return false
	end

	return true
end

function Act_Opening_Movie_OnCleanUp(actor, action)
end

function Act_Opening_Movie_OnLeave(actor, action)
   ODS("Act_Opening_Movie_OnLeave\n")
	return true
end


function Act_Opening_Movie_OnEvent(actor,textKey)
	--ODS("Act_Opening_Movie_OnEvent",false,987)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if textKey == "fade" then	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end	
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 3.0, true )
	end

	return	true;
end