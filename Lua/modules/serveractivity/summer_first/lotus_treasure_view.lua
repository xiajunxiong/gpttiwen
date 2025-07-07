LotusGuideView = LotusGuideView or DeclareView("LotusGuideView", "serveractivity/summer_first/lotus_guide_view")
function LotusGuideView:LotusGuideView()
    self.data = ServerActivityData.Instance
    self.act = ACTIVITY_TYPE.RAND_LOTUS
    self.cfg = self.data:GetLotusCfg()
    local end_reward_data = self.cfg[#self.cfg]
    self.reward_id = end_reward_data.item_id
end

function LotusGuideView:LoadCallback()
    UH.SetText(self.TimeText, self.data:GetLotusActTimeDesc(self.act))
    UH.SetIcon(self.BubbleIcon, Item.GetIconId(self.reward_id))
    UH.SetText(self.BubbleText, Item.GetName(self.reward_id))
end

function LotusGuideView:OnCloseClick()
    ViewMgr:CloseView(LotusGuideView)
end

function LotusGuideView:OnGoToClick()
    ViewMgr:CloseView(LotusGuideView)
    ViewMgr:OpenView(LotusTreasureView)
end

function LotusGuideView:OnHelpClick()
    local cfg = Config.language_cfg_auto.textdesc[135] or {}
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = cfg.desc or ""})
end

function LotusGuideView:OnBubbleIconClick()
    local info = {
        item_list = {Item.Create({item_id = self.reward_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
    local pos = UH.GetRootPosition(self.BubbleIcon.gameObject)
    ItemInfoData.Instance:SetClickDataPos(pos)
end



LotusTreasureView = LotusTreasureView or DeclareView("LotusTreasureView", "serveractivity/summer_first/lotus_treasure_view", Mod.SummerFirst.Lotus)
LotusTreasureView.Oper = {
    Info = 0,           -- 请求信息下发
    ActiveGrid = 1,     -- 请求激活格子  p1:row[0,3]  p2:col[0,3]
    GetEndReward = 2,   -- 请求领取最后一个奖励
}
VIEW_DECLARE_LEVEL(LotusTreasureView, ViewLevel.L0)
function LotusTreasureView:LotusTreasureView()
    
end

function LotusTreasureView:OnCloseClick()
    ViewMgr:CloseView(LotusTreasureView)
end

function LotusTreasureView:OnHelpClick()
    local cfg = Config.language_cfg_auto.textdesc[135] or {}
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = cfg.desc or ""})
end

function LotusTreasureView:OnJumpClick()
    -- if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_HELP_OTHERS) then
	-- 	PublicPopupCtrl.Instance:Error(Language.Activity.NotOpenTip)
    --     return
	-- end
    -- if ActivityRandData.Instance:OnClickHandle(ACTIVITY_TYPE.RAND_HELP_OTHERS) then
    --     ViewMgr:CloseView(SummerFirstView)
    --     ViewMgr:CloseView(LotusTreasureView)
    -- end

    ViewMgr:OpenViewByKey(Mod.Activity.Daily)
    ViewMgr:CloseView(LotusTreasureView)
end

LotusTreasurePanel = LotusTreasurePanel or DeclareMono("LotusTreasurePanel", UIWFlushPanel)
function LotusTreasurePanel:LotusTreasurePanel()
    self.data = ServerActivityData.Instance
    self.act_data = self.data.lotus_data
    self.data_cares = {
        {data = self.act_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushGridList, init_call = false},
    }
    self.act_type = ACTIVITY_TYPE.RAND_LOTUS
end

function LotusTreasurePanel:Awake()
    UIWFlushPanel.Awake(self)
    ServerActivityCtrl.Instance:SendActivityReq(self.act_type, LotusTreasureView.Oper.Info)
    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    ActivityRandData.SetRemind(self.act_type, server_open_day)

    self.cfg = self.data:GetLotusCfg()
    UH.SetText(self.ActTimeText, self.data:GetLotusActTimeDesc(self.act_type))
    self:FlushPanel()
    self:FlushRewardList()
end

function LotusTreasurePanel:FlushPanel()
    self:FlushGridList()
    self:FlushEndReward() 
end

function LotusTreasurePanel:FlushGridList()
    local grid_list_data,stuff_list_data = self.data:GetLotusGridListData()
    self.GridList:SetData(grid_list_data)
    self.StuffInfoList:SetData(stuff_list_data)
end

function LotusTreasurePanel:FlushRewardList()
    local col_list_data = self.data:GetLotusColListData()
    local row_list_data = self.data:GetLotusRowListData()
    for i = 1, self.RowRewards:Length() do
        self.RowRewards[i]:SetData(row_list_data[i])
    end
    for i = 1, self.ColRewards:Length() do
        self.ColRewards[i]:SetData(col_list_data[i])
    end
end

function LotusTreasurePanel:FlushEndReward()
    local end_reward_data = self.cfg[#self.cfg]
    local flag = self.data:IsLotusEndRewardFlag()
    self.EndReward.IsShowTip = flag >= 0
    self.EndReawrdBtnObj:SetActive(flag == 1 or flag == -1)
    self.EndEffectObj:SetActive(flag == 1)
    self.EndReward:SetData(end_reward_data)
end

function LotusTreasurePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function LotusTreasurePanel:OnGetEndRewardClick()
    local flag = self.data:IsLotusEndRewardFlag()
    if flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.ActLotus.Error1)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.ActLotus.Error2)
    else
        ServerActivityCtrl.Instance:SendActivityReq(self.act_type, LotusTreasureView.Oper.GetEndReward)
    end
end

LotusTreasureGridItem = LotusTreasureGridItem or DeclareMono("LotusTreasureGridItem", UIWidgetBaseItem)

function LotusTreasureGridItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local is_lighten = ServerActivityData.Instance:IsLotusGridLighten(data)
    self.PutBtnObj:SetActive(not is_lighten)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id))
    local is_can_put = ServerActivityData.Instance:IsLotusGridPut(data)
    self.TipObj:SetActive(is_can_put and not is_lighten)
end

function LotusTreasureGridItem:OnClick()
    if ServerActivityData.Instance:IsLotusGridPut(self.data) then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LOTUS, LotusTreasureView.Oper.ActiveGrid, self.data.line_num - 1, self.data.seq)
    else
        local item = Item.Create({item_id = self.data.activation_item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        PublicPopupCtrl.Instance:Error(string.format(Language.ActLotus.Error3, self.data.activation_item_num, Item.GetName(self.data.activation_item_id)))
    end
end

LotusTreasureRewardItem = LotusTreasureRewardItem or DeclareMono("LotusTreasureRewardItem", UIWidgetBaseItem)

function LotusTreasureRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    UH.SetIcon(self.Icon, Item.GetIconId(item_id))
    UH.SetText(self.Num, data.activation_item_num)
    self.TipObj:SetActive(self.IsShowTip == true)
end

function LotusTreasureRewardItem:OnIconClick()
    local info = {
        item_list = {Item.Create({item_id = self.data.item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
    local pos = UH.GetRootPosition(self.Icon.gameObject)
    ItemInfoData.Instance:SetClickDataPos(pos)
end

LotusTreasureStuffItem = LotusTreasureStuffItem or DeclareMono("LotusTreasureStuffItem", UIWidgetBaseItem)
function LotusTreasureStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id))
    UH.SetText(self.Num, Item.GetNum(data.item_id))
end

function LotusTreasureStuffItem:OnIconClick()
    local info = {
        item_list = {Item.Create({item_id = self.data.item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
    local pos = UH.GetRootPosition(self.Icon.gameObject)
    pos.y = pos.y - 200
    ItemInfoData.Instance:SetClickDataPos(pos)
end

SummerRewardDialog = SummerRewardDialog or DeclareView("SummerRewardDialog", "serveractivity/summer_first/summer_reward_dialog")

-- @ reward_item_list : item_obj[]
function SummerRewardDialog:LoadCallback(reward_item_list)
    reward_item_list = reward_item_list or {}
    local length = 0
    for i,v in pairs(reward_item_list) do
        length = length + 1
    end
    self.ListScroll.enabled = length > 3
    self.List:SetData(reward_item_list)
end

function SummerRewardDialog:OnCloseClick()
    ViewMgr:CloseView(SummerRewardDialog)
end