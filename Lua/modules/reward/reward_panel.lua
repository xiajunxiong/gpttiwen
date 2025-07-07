RewardPanel = RewardPanel or DeclareMono("RewardPanel", UIWFlushPanel)

function RewardPanel:RewardPanel()
    self.data = RewardData.Instance
    self.data_cares = {
        {data = self.data.reward_list_data,func = self.FlushRewardPanel,init_call = false},
        {data = self.data.open_reward_data,func = self.OpenRightPanel,init_call = false},
    }
end

function RewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().show_month_and_fortunebag then
        self:InitRewardPanel(3)
    else
        self:InitRewardPanel(self.data:GetOpenPanelType())
    end
    self:FlushListView()
    self.data:OpenPanelData()
    self.BtnRecharge:SetActive(not GLOBAL_CONFIG:Flags().block_recharge)
    -- 该遮罩用于转盘抽奖
    self.ObjBlock:SetActive(false)
    self.data:RewardBlock(self.ObjBlock)
end

function RewardPanel:OpenRightPanel()
    self:InitRewardPanel(self.data:GetOpenPanelType())
    self:FlushListView()
end

function RewardPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Reward, self.handle)
    self.data:RewardBlock()
end

function RewardPanel:InitRewardPanel(type)
    self.btn_list = self.data:GetRewardShowList()
    local data_list = self.btn_list[type]
    type = #data_list == 0 and 1 or type
    self.Toggle[type].isOn = true
    self.select_type = type
    self:FlushRemindView()
end

function RewardPanel:FlushRewardPanel()
    self.btn_list = self.data:GetRewardShowList()
    self:FlushListView()
    self:FlushRemindView()
end

function RewardPanel:FlushListView()
    local list = self:GetModuleView(self.btn_list[self.select_type] or {})
    self.List:SetDataList(list,self.data:GetOpenPanelData(list))
end

function RewardPanel:OnClickToggle(type)
    self.select_type = type
    self:FlushListView()
end

function RewardPanel:OnClickRecharge()
    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end

----------------------------模块处理----------------------------
function RewardPanel:GetModuleView(list)
    for k,v in pairs(list) do
        list[k].sub_view = self[v.view_name]
    end
    return list
end

function RewardPanel:FlushRemindView()
	RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Reward, self.handle)
    self.handle = Remind.Instance:RegisterGroup(Mod.Reward,function()
        for k,v in pairs(self.btn_list) do
            self.Toggle[k]:SetObjActive(#v ~= 0)
            self.RedPoint[k]:SetNum(self:GetRemindNum(v))
        end
    end,true)
end

function RewardPanel:GetRemindNum(data)
    for k,v in pairs(data) do
        if Remind.Instance:GetRemindNum(v.open) > 0 then
            return 1
        end
    end
    return 0
end

function RewardPanel:OnRewardPanelBtnClick(value, data)
    -- 特殊处理背景框
    if data.open == Mod.Reward.GodBeastGrace or data.open == Mod.Reward.GodToken then
        UH.LocalPosG(self.ObjBg, Vector3.New(-400, 315, 0))
        UH.SizeDelta(self.ObjBg, Vector2.New(1088, 609))
    else
        UH.LocalPosG(self.ObjBg, Vector3.New(-400, 315, 0))
        UH.SizeDelta(self.ObjBg, Vector2.New(996, 609))
    end
end

----------------------------RewardPanelBtnItem----------------------------
RewardPanelBtnItem = DeclareMono("RewardPanelBtnItem", UIWidgetBaseItem)

function RewardPanelBtnItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SpriteName(self.IconOn,data.view_name .. [[_on]])
    UH.SpriteName(self.IconOff,data.view_name .. [[_off]])
    if self.data then
        Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
        if self.data.sub_view then
            self.data.sub_view:SetActive(false)
        end
        self.TabActivator.Actives:Clear()
    end
    self.TabActivator.Actives:Add(data.sub_view)
    self.handle = Remind.Instance:RegisterGroup(data.mod,function ()
        self.RedPos:SetNum(Remind.Instance:GetGroupNum(data.mod))
    end, true)
    UIWidgetBaseItem.SetData(self, data)
end

function RewardPanelBtnItem:OnRelease()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
    UIWidgetBaseItem.OnRelease(self)
end

function RewardPanelBtnItem:Click()
    self.ToggleClick:Trigger()
end