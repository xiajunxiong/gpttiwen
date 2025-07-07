RewardUpgradeView = RewardUpgradeView or DeclareMono("RewardUpgradeView", UIWFlushPanel)

function RewardUpgradeView:RewardUpgradeView()
	self.data = RewardCtrl.Instance:Data()
	self.flush_events = {EventProtocol.ToEventKey(SCLevelRewardInfo)}
	self.data_cares = {
		{data = self.data.upgrade_change, func = self.onFlush},
		{data = RoleData.Instance:GetBaseData(), func = self.onFlush,init_call = false}
	}
end

function RewardUpgradeView:onFlush()
	self.RewardList:SetData(self.data:GetRewardUpgradeList())
end

--------------------------------RewardUpgradeItem---------------------------------
RewardUpgradeItem = DeclareMono("RewardUpgradeItem", UIWidgetBaseItem)

function RewardUpgradeItem:RewardUpgradeItem()
	self.language = Language.Reward.RewardUpgrade
	self.btn_state = Language.Reward.BtnState
end

function RewardUpgradeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local role_level = RoleData.Instance:GetRoleLevel()
	local condition_desc = self.language.condition_desc
	self.Condition.text = string.format(condition_desc, data.level)
	local prof_type = RoleData.Instance:ProfType()
	prof_type = prof_type > 0 and prof_type or 1
	-- if 10 == data.level then
		for _, item in pairs(data.rewards) do
			if item.old_id then
				local co = Cfg.RewardJobBagByIdType(item.old_id, prof_type)
				item.item_id = co and co.new_id or item.item_id
				item.show_key = "workshop_equip"
			end
		end
	-- end
	self.RewardGrid:SetData(data.rewards)
	local is_get = data.get_flag == 1
	local value = not is_get and data.level <= role_level
	self.BtnGetInter.Interactable = value
	self.BtnGet.text = is_get and self.btn_state.Geted or self.btn_state.Get
	self.Effect:SetActive(value) -- 10010237
end

function RewardUpgradeItem:OnClickGet()
	RewardCtrl.Instance:SendUpgradeRewardReq(self.data.seq)
	AudioMgr:PlayEffect(AudioType.UI, "item_get")
end