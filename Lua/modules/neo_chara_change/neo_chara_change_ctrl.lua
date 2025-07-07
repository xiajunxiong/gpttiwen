FILE_REQ("modules/neo_chara_change/neo_chara_change_data")
VIEW_REQ("modules/neo_chara_change/neo_prof_change_view")
VIEW_REQ("modules/neo_chara_change/neo_color_change_panel")
VIEW_REQ("modules/neo_chara_change/neo_color_change_succ_view")

NeoCharaChangeCtrl = NeoCharaChangeCtrl or BaseClass(BaseCtrl)

function NeoCharaChangeCtrl:__init()
	if NeoCharaChangeCtrl.Instance ~= nil then
		Debuger.LogError("[NeoCharaChangeCtrl] attempt to create singleton twice!")
		return
	end
	NeoCharaChangeCtrl.Instance = self
	self.data = NeoCharaChangeData.New()
	self:RegisterProtocol(CSRoleChangeColorReq)

end

function NeoCharaChangeCtrl:__delete()
	self:UnRegisterProtocol(CSRoleChangeColorReq)

	NeoCharaChangeCtrl.Instance = nil
end

function NeoCharaChangeCtrl:RequestChangeColor(color)
	local proc = GetProtocol(CSRoleChangeColorReq)
	proc.to_color = color
	--LogError("SEND====",proc)
	SendProtocol(proc)
end

