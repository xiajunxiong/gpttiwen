FILE_REQ("modules/experience/experience_data")
VIEW_REQ("modules/experience/experience_view")

ExperienceCtrl = ExperienceCtrl or BaseClass(BaseCtrl)

function ExperienceCtrl:__init()
	if ExperienceCtrl.Instance ~= nil then
		Debuger.LogError("[ExperienceCtrl] attempt to create singleton twice!")
		return
	end
	ExperienceCtrl.Instance = self
	self.data = ExperienceData.New()
end

function ExperienceCtrl:__delete()
    ExperienceCtrl.Instance = nil
end