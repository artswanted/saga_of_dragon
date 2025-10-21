function Act_FlyToCamera_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	actor:ResetAnimation()
	actor:SetCanHit(false)
	actor:StopJump();
	actor:FreeMove(true);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	
	--	시작 위치 기록
	action:SetParamFloat(0,actor:GetPos():GetX());
	action:SetParamFloat(1,actor:GetPos():GetY());
	action:SetParamFloat(2,actor:GetPos():GetZ());
	
	--	State
	action:SetParamInt(3,0);
	
	--	날기 시작 시간 기록
	action:SetParamFloat(4,g_world:GetAccumTime());
	
	--	날아갈 방향 구하기(패스 노멀의 역방향)
	local	kPathNormal = actor:GetPathNormal();
	kPathNormal:Multiply(-1);
	kPathNormal:Unitize();
	
	kPathNormal:Rotate(Point3(0,0,1),math.random(-20,20)*3.141592/180.0);
	kPathNormal:Unitize();
	
	action:SetParamFloat(5,kPathNormal:GetX());
	action:SetParamFloat(6,kPathNormal:GetY());
	action:SetParamFloat(7,kPathNormal:GetZ());
	
	-- Moving Dir 저장
	action:SetParamFloat(8,actor:GetLookingDir():GetX());
	action:SetParamFloat(9,actor:GetLookingDir():GetY());
	action:SetParamFloat(10,actor:GetLookingDir():GetZ());
	
	-- UpSpeed
	action:SetParamFloat(11,600+math.random(-200,200));
	
	actor:SetThrowStart();
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	
	
	return true
end

function Act_FlyToCamera_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(3);
	
	local	fFrontMoveSpeed = 500;
	local	fFrontMoveAccel = -100;
	
	--ODS("iState : "..iState.."\n");
	
	actor:DoDropItems();
	
	
	if iState == 0 then
	
		
		local	fUpSpeed = action:GetParamFloat(11);
		
		--	경과시간
		local	fElapsedTime = accumTime - action:GetParamFloat(4);
		
		-- 이동
		local	kStartPos = Point3(action:GetParamFloat(0),
									action:GetParamFloat(1),
									action:GetParamFloat(2));
									
		local	fStartZ = action:GetParamFloat(2);
									
		local	kFlyDir = Point3(action:GetParamFloat(5),
									action:GetParamFloat(6),
									action:GetParamFloat(7));
								
		
		
		kFlyDir:Multiply((fFrontMoveSpeed+fFrontMoveAccel*fElapsedTime)*fElapsedTime);
		kStartPos:Add(kFlyDir);
		kStartPos:SetZ(fStartZ+(fUpSpeed+g_fGravity*fElapsedTime)*fElapsedTime);
		
		actor:SetTranslate(kStartPos);
		
		-- 회전
		
		local	fRotateAngle = fElapsedTime * 360*10 * 3.141592/180.0;
	
		local	kMovingDir = Point3(action:GetParamFloat(8),
									action:GetParamFloat(9),
									action:GetParamFloat(10));
									
		--actor:SetRotation(fRotateAngle,Point3(1,0,0));		
		
		
		local	kCollPoint = actor:CheckCollWithCamera();
		if kCollPoint:GetX() ~= -1 or 
			kCollPoint:GetY() ~= -1 then
			
			--	화면과 충돌했음.
			
			AddNewScreenBreak(kCollPoint:GetX(),kCollPoint:GetY());
			
			QuakeCamera(0.5,2,1,0.1,3);
						
			--	스테이트 변경
			action:SetParamInt(3,1);
			action:SetParamFloat(4,accumTime);
			
			--	시작 위치 기록
			action:SetParamFloat(0,actor:GetPos():GetX());
			action:SetParamFloat(1,actor:GetPos():GetY());
			action:SetParamFloat(2,actor:GetPos():GetZ());
			
			actor:EndBodyTrail();
						
			return	true;
			
		end
		
		if fElapsedTime>5 then
		
			ODS("Time Up 01\n");
			return	false;
		
		end
	
	elseif iState == 1 then
	
		--	밑으로 주욱 미끌어져 내려오기
		
		local	fElapsedTime = accumTime - action:GetParamFloat(4);
		ODS("fElapsedTime : "..fElapsedTime.."\n");
	
		local	kStartPos = Point3(action:GetParamFloat(0),
									action:GetParamFloat(1),
									action:GetParamFloat(2));
									


		kStartPos:SetZ(kStartPos:GetZ()+g_fGravity*0.1*fElapsedTime*fElapsedTime);
		actor:SetTranslate(kStartPos);		
		
		
		if fElapsedTime>5 then
		
			ODS("Time Up 02\n");
			return	false;
		
		end									
									
										
	
	end
	
	return true
end

function Act_FlyToCamera_OnCleanUp(actor, action)
end

function Act_FlyToCamera_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
			
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	ODS("Act_FlyToCamera_OnLeave action:"..action:GetID().."\n");
	
	actor:EndBodyTrail();

	-- actor:SetCanHit(true)
	if (string.sub(actor:GetID(), 1, 1) == "m" 
		or string.sub(actor:GetID(), 1, 1) == "o") then
      if actor:GetAbil(AT_MANUAL_DIEMOTION) == 0 then	-- 0은 서버에서 죽임, 101은 죽이지 않음(후처리 필요)
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
         g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
      end
	end

	return true
end

