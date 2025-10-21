a = 1
b = 1
c = 1
d = 1
e = 1
lordlair_scene_01_play = false
lordlair_actor = nil

-- 포탈 카메라
function camera_ll_01_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
--	g_world:SetCameraMode(1, actor)
--	SetCameraSetting(Point3(0, 296, 122), Point3(0, 0, 64))
    lordlair_actor = actor
    g_world:SetCameraModeMovie(2, 0, "return_fllow()")
	return true
end

function return_fllow()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
--	g_world:SetCameraMode(1, lordlair_actor)
    g_world:SetCameraPose(Point3(500, -754, 353), Quaternion(0.660199, 0.253253, -0.180084, 0.683791))
  	g_world:SetCameraMode(0, lordlair_actor)
end

function camera_ll_01_OnUpdate(trigger, actor)
	return true
end

function camera_ll_01_OnLeave(trigger, actor)
	return true
end

-- 고정 카메라 (1)
function camera_ll_02_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(-110, -54, 526), Quaternion(0.686030, 0.171356, -0.257132, 0.658698))
	g_world:SetCameraMode(0, actor)
	return true

	-- if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) == true then
	-- 	SetCameraSetting(Point3(-40, 316, 254), Point3(0, 0, 76))
	-- end
	-- return true
end

function camera_ll_02_OnUpdate(trigger, actor)
	return ture
end

function camera_ll_02_OnLeave(trigger, actor)
	return true
end

-- 고정 카메라 (2)
function camera_ll_03_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(500, -754, 353), Quaternion(0.660199, 0.253253, -0.180084, 0.683791))
	g_world:SetCameraMode(0, actor)
	return true

	-- if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) == true then
	--	SetCameraSetting(Point3(-40, 316, 254), Point3(0, 0, 76))
	--	g_world:SetCameraMode(1, actor)
	-- end
	-- return true
end

function camera_ll_03_OnUpdate(trigger, actor)
	return ture
end

function camera_ll_03_OnLeave(trigger, actor)
	return true
end

-- 고정 카메라 (3)
function camera_ll_04_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(1377, -1260, 499), Quaternion(0.301372, 0.639660, 0.242608, 0.664185))
	g_world:SetCameraMode(0, actor)
	return true

	--SetCameraSetting(Point3(-40, 316, 254), Point3(0, 0, 76))
	--end
	--return true
end

function camera_ll_04_OnUpdate(trigger, actor)
	return ture
end

function camera_ll_04_OnLeave(trigger, actor)
	return true
end

-- 고정 카메라 (4)
function camera_ll_05_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(1694, -811, 391), Quaternion(0.447379, 0.547588, 0.197611, 0.678933))
	g_world:SetCameraMode(0, actor)
	return true

	--SetCameraSetting(Point3(-40, 316, 254), Point3(0, 0, 76))
	--end
	--return true
end

function camera_ll_05_OnUpdate(trigger, actor)
	return ture
end

function camera_ll_05_OnLeave(trigger, actor)
	return true
end

-- 전투 카메라
function camera_ll_06_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(1, actor)
	SetCameraSetting(Point3(-40, 476, 182), Point3(0, 0, 120))
	return true
end

function camera_ll_06_OnUpdate(trigger, actor)
	return true
end

function camera_ll_06_OnLeave(trigger, actor)
	return true
end
