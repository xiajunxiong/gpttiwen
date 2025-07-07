RoleInfoAccrssoryView = RoleInfoAccrssoryView or DeclareView("RoleInfoAccrssoryView", "role_info/role_info_accrssory")
function RoleInfoAccrssoryView:OnCloseClick()
    ViewMgr:CloseView(RoleInfoAccrssoryView)
end

--------------RoleInfoAccrssoryPanel--------------
RoleInfoAccrssoryPanel = RoleInfoAccrssoryPanel or DeclareMono("RoleInfoAccrssoryPanel", UIWFlushPanel)
function RoleInfoAccrssoryPanel:RoleInfoAccrssoryPanel()
    self.Data = AccrssoryData.Instance
    self.data = RoleInfoData.Instance:GetRoleAccrssory()
    self.data_cares = {
        {data = self.data, func = self.FlushPanel, init_call = false},
    }
end

function RoleInfoAccrssoryPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function RoleInfoAccrssoryPanel:FlushPanel()
    for i = 1, self.LeftAccrssoryItem:Length() do
        self.LeftAccrssoryItem[i]:SetData(self.data.info[i] or {})
    end
    for i = 1, self.AccrssorySkillItem:Length() do
        self.AccrssorySkillItem[i]:SetData(self.data.skill[i] or {})
    end
end

function RoleInfoAccrssoryPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end
