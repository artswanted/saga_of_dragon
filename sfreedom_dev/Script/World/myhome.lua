
function TestHomeBuild()
	CreateHome()
	AddWall(5)
	SetEditMode(true)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraZoom(0.4)
end

function ReleaseHome()
	if g_world == nil or g_world:IsNil() == true then
		return
	end
	
	g_world:ReleaseHome()
end

function CreateHome()
	if g_world == nil or g_world:IsNil() == true then
		return
	end
	
	-- if g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) == false then
		-- return
	-- end
	
	local kHome = g_world:GetHome()
	if kHome:IsNil() == true then
		g_world:CreateHome()
	end
	
	kHome = g_world:GetHome()
	kHome:SetRoomSize(200,200,160)
end

function InitializeHome()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == true then
		CreateHome()
	end
	
	--AddWall(5)
	--g_world:SetCameraZoom(0.4)

	
	-- AddFurniture(70000010)
	-- AddFurniture(70000040)
	-- AddFurniture(70000190)
	--SetEditMode(true)
	--g_world:GetHome():AddFurniture(70000010, Point3(0,-150,100),0)
	--g_world:SetPhysXDebug(true)
end

function ReadyHome()
--	g_world:SetMainCamera("minimap_camera")

end


function InitializeHomeTest()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == true then
		CreateHome()
	end
	
	g_world:SetCameraZoom(1.0)
	--AddFurniture(70000010)
	-- AddFurniture(70000040)
	-- SetEditMode(true)
end

function InitializeHomeUI()


end

function AddWall(iRotate)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		local pos = kHome:GetCenterPos()
		kHome:AddWall(pos, iRotate)
	end
end

function RemoveWall(iRotate)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		kHome:RemoveWall()
	end
end

function AddFurniture(itemNo, iRotate)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local pos = GetPlayer():GetTranslate()
	pos:SetZ(pos:GetZ() + 1)
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		kHome:AddFurniture(itemNo, pos, iRotate)
	end
end

function RemoveAllFurniture()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		kHome:RemoveAllFurniture()
	end	
end

function SetEditMode(bEditMode)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local home = g_world:GetHome()
	if home:IsNil() then
		return false
	end

	home:SetArrangeMode(bEditMode)
end

function Enter_Home(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ClearReservedPilot()
	local siStreetNo = trigger:GetParam()
	local siHouseNo = trigger:GetParam2()
	
	EnterHome(siStreetNo, siHouseNo)
end

function EnterBuilding(trigger, actor)
	local kNum = trigger:GetParam()
	OnSendMyhomeEnter(kNum)
end

function EnterHome(siStreet, siHouse)
	local kPacket = NewPacket(17203) --PT_C_M_REQ_MYHOME_ENTER
	kPacket:PushShort(siStreet)
	kPacket:PushInt(siHouse)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function ExitHome()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if IsUseCashShop() == true then
		return
	end

	if kHome:IsNil() == false then
		local kGuid = kHome:GetHomeGuid()
		if nil ~= kGuid and not kGuid:IsNil() then
			g_world:ClearReservedPilot()
			SetEditMode(false)
			local kPacket = NewPacket(17207) --PT_C_M_REQ_MYHOME_EXIT
			kPacket:PushGuid(kGuid)
			Net_Send(kPacket)
			DeletePacket(kPacket)
		end
	end	
end

function EnterHometown(iGroundNo, iPortalNo)
	local kPacket = NewPacket(17223) --PT_C_M_REQ_HOMETOWN_ENTER
	kPacket:PushInt(iGroundNo)
	kPacket:PushInt(iPortalNo)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function ExitHometown()
	local kPacket = NewPacket(17225) --PT_C_M_REQ_HOMETOWN_EXIT
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Exit_Home_Town(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ClearReservedPilot()
	ExitHometown()
end

function SetMovingFurniture()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		kHome:SetMovingFurniture()
	else
		OnClickMoveFurniture()
	end		
end

function RemoveFurniture()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kHome = g_world:GetHome()
	if kHome:IsNil() == false then
		kHome:RemoveFurniture()
	else
		OnClickRemoveFurniture()
	end		
end

function Enter_Field(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ClearReservedPilot()
	CallTeleCardUI(0)
	RegistUIAction(GetUIWnd("SFRM_TELEPORT"), "CloseUI")
end

function PickNamePlate(who)

	local kGuid = OnClickNamePlate(who)
	local	kPilot = g_pilotMan:FindPilot(kGuid)
	if kPilot:IsNil() then
		return
	end

	local kPopupItem = {}
	local iIndex = 1
	kPopupItem[iIndex] = "MYHOME_HOUSE_INFO"

	local kCursorPos = GetCursorPos()

	CallCommonPopup(kPopupItem, kGuid, nil, kCursorPos)
end
