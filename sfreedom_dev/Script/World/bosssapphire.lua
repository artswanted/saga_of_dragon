--SetCameraSetting(Point3(0, 470, 125), Point3(24, 0, 112))
--SetCameraSpeed(13.0, 2.0, 10.0, 4.5)

puppets = {}
aaaa = false

function Net_InsertPuppets(world)
	if aaaa == true then
		return true
	end
	aaaa = true

	guid=GUID("1")

	i = 0
	while i < 36 do
		guid:Generate()

		
		if i % 6 == 0 then
			puppetName = "p_5"
		else
			puppetName = "p_4"
		end

		puppets[i] = world:AddPuppet(guid, puppetName, Point3(0, 0, 0), Quaternion(i * math.pi / 18 + math.pi * 0.5, Point3(0, 0, 1)))
		i = i + 1
	end
end

explosion_start = false

function Net_ExplosePuppets(world)
	explosion_start = true
end

function World_Update(world, accumTime)
	if explosion_start == true then
		if explosion_time == nil then
			explosion_time = accumTime
			explosion_cur = 0
		end
	
		if accumTime - explosion_time > 0.10 then
			explosion_time = accumTime
			puppets[explosion_cur]:TransitAction("rotate_s")
			explosion_cur = explosion_cur + 1

			if explosion_cur - 1 >= 0 then
				puppets[explosion_cur - 1]:TransitAction("rotate_e")
			end
		end

		if explosion_cur >= 36 then
			explosion_start = false
			explosion_time = nil
			explosion_cur = 0
		end
	end
end
