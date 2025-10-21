
function Boss_Camera_In_OnEnter()
	SetCameraSetting(Point3(400, 360, 200), Point3(0, 0, 0))
end
function Boss_Camera_In_OnUpdate()
end
function Boss_Camera_In_OnLeave()
end

function Boss_Camera_Out_OnEnter()
	SetCameraSetting(Point3(200, 450, 200), Point3(0, 0, 0))
end
function Boss_Camera_Out_OnUpdate()
end
function Boss_Camera_Out_OnLeave()
end