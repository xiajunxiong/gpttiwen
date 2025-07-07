
TeamPartnerInfoView = TeamPartnerInfoView or DeclareView("TeamPartnerInfoView", "item_info/team_partner_info")

function TeamPartnerInfoView:TeamPartnerInfoView()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team
end

function TeamPartnerInfoView:LoadCallback()
	local partner_list = self.data:GetTeamPartnerList()
	if #partner_list > 3 then
		for i = 1, 2 do
			self.PartnersObj1s[i]:SetActive(true)
		end
		self.Partners1:SetData(partner_list)
	else
		self.PartnersObj2:SetActive(true)
		self.Partners2:SetData(partner_list)
	end
	self.InfoPosition.position = self.data.info_position
end

function TeamPartnerInfoView:OnClickBlank()
    ViewMgr:CloseView(TeamPartnerInfoView)
end

TeamPartnerInfoViewItem = DeclareMono("TeamPartnerInfoViewItem", UIWidgetBaseItem)

function TeamPartnerInfoViewItem:TeamPartnerInfoViewItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamPartnerInfoViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
    local vo = data.vo

	local prof_sp = RoleData.GetProfSp(info.prof)

    self.Name.text = info.name
    self.ProfI.SpriteName = prof_sp

    local level = RoleData.Instance:GetBaseData().level
    self.Level.text = "Lv.".. level
end

function TeamPartnerInfoViewItem:OnClickItem()
	if nil == self.team_data.team_partner_info.info then
		PartnerCtrl.Instance:SendPartnerFight(self.data.id)
	else
		PartnerCtrl.Instance:SendChangeSyncFightOrder(self.data.id, self.team_data.team_partner_info.info.uid)
	end
	ViewMgr:CloseView(TeamPartnerInfoView)
end