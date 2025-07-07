FILE_REQ("modules/divine_costume/divine_costume_data")
VIEW_REQ("modules/divine_costume/divine_costume_view")
VIEW_REQ("modules/divine_costume/divine_costume_pop_view")
VIEW_REQ("modules/divine_costume/resonance_show_view")
VIEW_REQ("modules/divine_costume/resonance_success_view")
VIEW_REQ("modules/divine_costume/divine_costume_result_view")
DivineCostumeCtrl = DivineCostumeCtrl or BaseClass(BaseCtrl)

function DivineCostumeCtrl:__init()
	if DivineCostumeCtrl.Instance ~= nil then
		Debuger.LogError("[DivineCostumeCtrl] attempt to create singleton twice!")
		return
	end	
	DivineCostumeCtrl.Instance = self
	self.data = DivineCostumeData.New()
end

function DivineCostumeCtrl:__delete()
	self.data = nil
	DivineCostumeCtrl.Instance = nil
end

function DivineCostumeCtrl:GuideStart(is_force)
	is_force = is_force or false
    if UnityPlayerPrefs.GetInt(PrefKeys.DivineCostumeGuide()) == 0 then
        UnityPlayerPrefs.SetInt(PrefKeys.DivineCostumeGuide(), 1)
    elseif not is_force then
        return
    end
    GuideManager.Instance:Start(136)
end

--没激活的，第一次打开，才会触发这个指引
function DivineCostumeCtrl:CheckGuideStart()
	local show_list = DivineCostumeData.Instance:GetEquipmentShowList()
	if show_list == nil or show_list[1] == nil then
		return
	end
	local cell_data = DivineCostumeData.Instance:GetEquipmentData(show_list[1].id)
	if cell_data == nil then
		self:GuideStart()
	else
        UnityPlayerPrefs.SetInt(PrefKeys.DivineCostumeGuide(), 1)
	end
end
