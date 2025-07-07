
local Input = UnityEngine.Input

AnimalRaceShots = AnimalRaceShots or DeclareMono("AnimalRaceShots")

function AnimalRaceShots:AnimalRaceShots()
	self.touch_helper = Game.SceneTouchHelper
	self.input = UnityEngine.Input
	Runner.Instance:AddRunObj(self, 3)
	self.VCamDolly = self.VCam:GetCinemachineComponent(0)
	self.speed = 0.2

	self.animal_ar_info_sm_care = AnimalRaceData.Instance.ar_info:Care(BindTool.Bind(self.AutoRaceCamera, self), "info_type")
end

function AnimalRaceShots:OnDestroy()
	Runner.Instance:RemoveRunObj(self)
	AnimalRaceData.Instance.ar_info:Uncare(self.animal_ar_info_sm_care)
end

function AnimalRaceShots:Update()
	if Input.GetMouseButton(0) then
		self.auto_cam = false
		local mX = Input.GetAxis("Mouse X")
		local value = self.VCamDolly.m_PathPosition + mX * Time.deltaTime * self.speed
		value = math.min(1,value)
		value = math.max(0,value)
		self.VCamDolly.m_PathPosition = value
	end
	if self.auto_cam then
		local pass_time = math.floor(AnimalRaceConfig.RaceAllTime - (AnimalRaceData.Instance.ar_info.timestamp - TimeHelper.GetServerTime()))
		local percent = pass_time > 0 and AnimalRaceData.Instance:GetAnimalPosPercent(1, pass_time) or 0
		self.VCamDolly.m_PathPosition = 1 - percent
	end
end

function AnimalRaceShots:AutoRaceCamera()
	self.auto_cam = AnimalRaceConfig.InfoType.RoleDoing == AnimalRaceData.Instance.ar_info.info_type
end