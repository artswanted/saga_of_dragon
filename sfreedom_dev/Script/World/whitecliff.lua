-------------------------- 이동형 카메라 --------------------------
-- 기본형 카메라
function underCamera_01_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_01_OnUpdate(trigger, actor)
	return true
end

function underCamera_01_OnLeave(trigger, actor)
	return true
end

-- 가로형 카메라
function underCamera_02_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_02_OnUpdate(trigger, actor)
	return true
end

function underCamera_02_OnLeave(trigger, actor)
	return true
end

-- 세로형 카메라
function underCamera_03_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_03_OnUpdate(trigger, actor)
	return true
end

function underCamera_03_OnLeave(trigger, actor)
	return true
end



-------------------------- 고정형 카메라 세트 --------------------------

-- 1. 고정형 카메라
function setCamera_wc01_01_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(-807, -1811, 223), Quaternion4(0.244734, 0.663404, 0.571289, 0.416687))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_01_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_01_OnLeave(trigger, actor)
	return true
end

-- 2. 고정형 카메라
function setCamera_wc01_02_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(-808, -1811, 659), Quaternion4(0.190871, 0.680859, 0.536127, 0.461050))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_02_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_02_OnLeave(trigger, actor)
	return true
end

-- 3. 고정형 카메라
function setCamera_wc01_03_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(766, -1699, 317), Quaternion4(0.572829, 0.414568, 0.085447, 0.701925))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_03_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_03_OnLeave(trigger, actor)
	return true
end

-- 4. 고정형 카메라
function setCamera_wc01_04_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(769, -1703, 687), Quaternion4(0.577346, 0.408255, 0.104910, 0.699281))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_04_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_04_OnLeave(trigger, actor)
	return true
end

-- 5. 고정형 카메라
function setCamera_wc01_05_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(769, -1703, 1107), Quaternion4(0.577346, 0.408255, 0.104910, 0.699281))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_05_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_05_OnLeave(trigger, actor)
	return true
end

-- 6. 고정형 카메라
function setCamera_wc01_06_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(-637, -891, 265), Quaternion4(-0.023170, 0.706727, 0.618140, 0.343369))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_06_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_06_OnLeave(trigger, actor)
	return true
end


-- 7. 고정형 카메라
function setCamera_wc01_07_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(628, -809, 251), Quaternion4(0.656573, 0.262510, -0.134291, 0.694238))
	g_world:SetCameraMode(0, actor)
	return true
end

function setCamera_wc01_07_OnUpdate(trigger, actor)
	return true
end

function setCamera_wc01_07_OnLeave(trigger, actor)
	return true
end
