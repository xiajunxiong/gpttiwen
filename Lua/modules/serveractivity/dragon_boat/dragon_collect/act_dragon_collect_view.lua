-- 苍龙赐福
ActDragonCollectView = ActDragonCollectView or DeclareView("ActDragonCollectView", "serveractivity/dragon_boat/dragon_collect/act_dragon_collect_view", Mod.DragonBoat.DragonCollect)
function ActDragonCollectView:ActDragonCollectView()
    
end

function ActDragonCollectView:OnCloseClick()
    ViewMgr:CloseView(ActDragonCollectView)
end

ActDragonCollectPanel = ActDragonCollectPanel or DeclareMono("ActDragonCollectPanel", UIWFlushPanel)
function ActDragonCollectPanel:ActDragonCollectPanel()
    self.data = ActDragonCollectData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
end

function ActDragonCollectPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.DRAGON_COLLECT,TimeType.Type_Time_8,Language.LuckBox.Text5)
    local item_info = Config.canglongcifu_auto.other[1].all_light_item
    self.Cell:SetData(Item.Create(item_info))
    local reward_cfg = self.data:GetShowReward()
    local reward_list = DataHelper.FormatItemList(reward_cfg.show_reward_item)
    self.RewardList:SetData(reward_list)
    if not self.data.first_remind then
        self.data.first_remind = true
        self.data:RemindNotice()
    end
    self:FlushPanel()
end

function ActDragonCollectPanel:FlushPanel()
    local list_data = self.data:GetListData()
    self.List:SetData(list_data)
    local flag = self.data:GetEndFlag()
    if self.last_flag ~= nil and self.last_flag ~= flag and flag == 1 then
        self.SuccEffect:SetActive(true)
    end
    self.last_flag = flag
    self.EndFlag:SetActive(flag == -1)
    self.GetBtn:SetActive(flag == 1)
    self.CanEffect:SetActive(flag == 1)
    self:FlushStuff()
end

function ActDragonCollectPanel:FlushStuff()
    local item_id = self.data:ItemId()
    UH.SetIcon(self.Icon, Item.GetIconId(item_id))
    UH.SetText(self.Num, "x" .. Item.GetNum(item_id))
end

function ActDragonCollectPanel:OnGetClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.DRAGON_COLLECT,ActDragonCollectCtrl.ReqType.Get)
end

function ActDragonCollectPanel:OnPreviewClick()
    self.Preview:SetActive(true)
end

function ActDragonCollectPanel:OnClosePreviewClick()
    self.Preview:SetActive(false)
end

function ActDragonCollectPanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[330].desc)
end

function ActDragonCollectPanel:OnJumpClick()
    local cfg = self:GetJumpCfg()
    if not cfg then
        return
    end
    local mod_key = cfg.jump_mod
    if mod_key and mod_key ~= 0 then
        ViewMgr:OpenViewByKey(mod_key)
        ViewMgr:CloseView(ActDragonCollectView)
        ViewMgr:CloseView(ActDragonBoatMainView)
    end
end

function ActDragonCollectPanel:GetJumpCfg()
    local open_day = ActDragonBoatData.Instance:OpenDay()
    for i,v in pairs(Config.canglongcifu_auto.activity_jump) do
        if open_day >= v.start_time and open_day <= v.end_time then
            return v
        end
    end
end

function ActDragonCollectPanel:OnIconClick()
    local item_id = self.data:ItemId()
    local item = Item.Create({item_id = item_id})
    local info = {
        item_list = {item}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end


ActDragonCollectItem = ActDragonCollectItem or DeclareMono("ActDragonCollectItem", UIWidgetBaseItem)
function ActDragonCollectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local flag = ActDragonCollectData.Instance:GetFlag(data)
    self.Mask:SetActive(flag == 0)
    self.SelImg:SetActive(flag == 1)
    local icon_id = Item.GetIconId(data.light_item.item_id)
    local num = data.light_item.num
    UH.SetIcon(self.Icon1,icon_id)
    UH.SetIcon(self.Icon2,icon_id)
    UH.SetText(self.Num1,num)
    UH.SetText(self.Num2,num)
end

function ActDragonCollectItem:OnLigthClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.DRAGON_COLLECT,ActDragonCollectCtrl.ReqType.Open,self.data.seq)
end

function ActDragonCollectItem:OnMaskClick()
    local item_id = ActDragonCollectData.Instance:ItemId()
    MainOtherCtrl.Instance:GetWayViewById(item_id)
end