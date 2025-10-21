LavPuppets = {}

function Net_InsertLavPuppets(world)
	guid=GUID("1")
	guid:Generate()
	LavPuppets[0] =  world:AddPuppet(guid, "p_front", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	guid:Generate()
	LavPuppets[1] =  world:AddPuppet(guid, "p_left", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	guid:Generate()
	LavPuppets[2] =  world:AddPuppet(guid, "p_right", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	
	ODS("____________________Add Puppets!!!!!!!__________________\n")

	LavPuppets[0]:TransitAction("idle")
	LavPuppets[1]:TransitAction("idle")
	LavPuppets[2]:TransitAction("idle")
end

function World_LavUpdate(world, accumTime)
end

function testtttttttttttttttt()
	LavPuppets[0]:TransitAction("dmg")
	LavPuppets[0]:ReloadNif()
end
