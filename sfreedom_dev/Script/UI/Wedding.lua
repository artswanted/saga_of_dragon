function WeddingFanfare()
	local kActor = GetMyActor()
	if(nil == kActor:IsNil()) then return end
	kActor:AttachParticleToCameraFront(10001, Point3(0,300,-60), "ef_fanfare_02", 0.6)
	PlaySoundByID("Couple_Marriage01")
end

function WeddingBalloon()	
	AttatchParticleToActorPos("c_Boris", 10002, Point3(150,-700,-30), "ef_fanfare_03", 1.5)
	AttatchParticleToActorPos("c_Boris", 10003, Point3(-150,-700,-30), "ef_fanfare_03", 1.5)
	AttatchParticleToActorPos("c_Boris", 10004, Point3(300,-550,-30), "ef_fanfare_03", 1.2)
	AttatchParticleToActorPos("c_Boris", 10005, Point3(-300,-550,-30), "ef_fanfare_03", 1.2)
	PlaySoundByID("Couple_Marriage02")
end