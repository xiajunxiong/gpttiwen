
TaskWXLFBMenuView = TaskWXLFBMenuView or DeclareMono("TaskWXLFBMenuView", UIWFlushPanel)

function TaskWXLFBMenuView:TaskWXLFBMenuView()
	self.data = WenXiangLouCtrl.Instance:Data()
	self.language = Language.Task.WXLMenu
	self.data_cares = {
		{data = self.data.detail_info, func = self.FlushMenu, keys = {"load_status"}},
		{data = self.data.detail_info, func = self.FlushKey, keys = {"key_seq"}},
	}
end

function TaskWXLFBMenuView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	if nil == self.role_pos_event then
		self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
	end
end

function TaskWXLFBMenuView:OnDisable()
	if nil ~= self.role_pos_event then
		EventSys:UnBind(self.role_pos_event)
		self.role_pos_event = nil
	end
end

function TaskWXLFBMenuView:FlushMenu()
	local ls = self.data.detail_info.load_status
	self.ShowDesc.text = string.format(self.language.format, WenXiangLouConfig.LoadType.type0 == ls and self.language.start or self.language.ing, self.language[ls])
	self.ButtonsObj:SetActive(ls > WenXiangLouConfig.LoadType.type1)
end

function TaskWXLFBMenuView:FlushKey()
	self.KeyObj:SetActive(self.data.detail_info.key_seq > -1)
end

function TaskWXLFBMenuView:OnMainRolePosChange()
	self.data:CheckKey(MainPlayer:GetPos())
end

function TaskWXLFBMenuView:OnClickKeys()
	ViewMgr:OpenView(WenXiangLouKeysView)
end

function TaskWXLFBMenuView:OnClickInfo()
	self.data:SetDetailInfoInfoType(WenXiangLouConfig.InfoType.check)
	ViewMgr:OpenView(WenXiangLouInfoView)
end

function TaskWXLFBMenuView:OnClickKeyClose()
	self.KeyObj:SetActive(false)
end

function TaskWXLFBMenuView:OnClickKeyCheck()
	if self.data.detail_info.key_seq > -1 then
		ViewMgr:OpenView(WenXiangLouKeyView, {seq = self.data.detail_info.key_seq})
	end
end