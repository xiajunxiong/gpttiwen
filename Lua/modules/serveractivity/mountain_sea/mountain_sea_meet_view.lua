MountainSeaMeetView = MountainSeaMeetView or DeclareView("MountainSeaMeetView", "serveractivity/mountain_sea/mountain_sea_meet", Mod.MountainSea.Meet)
function MountainSeaMeetView:MountainSeaMeetView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET,TimeType.Type_Special_4,Language.MountainSeaFirstMeet.ActTime)
end

function MountainSeaMeetView:OnClickClose()
    ViewMgr:CloseView(MountainSeaMeetView)
end
function MountainSeaMeetView:OnClickDesc()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[175].desc})
end

----------------------------MountainSeaFirstMeetPanel----------------------------
MountainSeaMeetPanel = MountainSeaMeetPanel or DeclareMono("MountainSeaMeetPanel", UIWFlushPanel)
function MountainSeaMeetPanel:MountainSeaMeetPanel()
    self.Data = ServerActivityData.Instance
    self.data = self.Data.mountain_sea_meet_data
    self.language = Language.MountainSeaFirstMeet
    self.data_cares = {
        {data = self.data, func = self.FlushPanel, init_call = true},
        {data = self.Data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushByItem, init_call = true},
    }
end
function MountainSeaMeetPanel:FlushByItem()
    self.other = Config.shanhaixiangfeng_auto.other[1]
    UH.SetIcon(self.Icon,self.other.consume_item,ICON_TYPE.ITEM)
    local has_num = Item.GetNum(self.other.consume_item)
    UH.SetText(self.Num,has_num)
end
function MountainSeaMeetPanel:OnClickItem()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.other.consume_item})})
    --CellClicks.BagGridNoButton({GetData = function()
    --    return Item.New({item_id = Config.shanhaixiangfeng_auto.other[1].consume_item})
    --end})
end

function MountainSeaMeetPanel:FlushPanel()
    self.RewardLits:SetData(self.data.reward_data[1].show_item)
    self.GroupList:SetData(self.data.reward_group)
    self.StartList:SetData(self.data.task_start_info)
    self.EndList:SetData(self.data.task_end_info)
    self.StartObj:SetActive(not (#self.data.task_start_info > 0))
    self.EndObj:SetActive(not (#self.data.task_end_info > 0))
    self.BuyObj:SetActive(self.data.is_buy == 0)
    self.Herald:SetActive(self.data.show_tip ~= nil)
    UH.SetText(self.HeraldText,string.format(Language.MountainSeaMeet.ActTips,self.data.show_tip or " "))
    UH.SetText(self.Moneny,string.format(Language.MountainSeaMeet.Moneny,Config.shanhaixiangfeng_auto.other[1].buy_money / 10))
end

function MountainSeaMeetPanel:OnBuyClick()
    self.price = Config.shanhaixiangfeng_auto.other[1].buy_money
    local fee = self.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_MEET, fee, ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET, 3)
end

function MountainSeaMeetPanel:OnClickTip()
    local tb = TimeHelper.FormatUnixTime2Date(self.data.open_time)
    PublicPopupCtrl.Instance:Center(string.format(Language.MountainSeaMeet.OpenTip,tb.month,tb.day))
end

function MountainSeaMeetPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_MEET, self.price)
end


function MountainSeaMeetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

----------------------------MountainSeaMeetTaskItem----------------------------
MountainSeaMeetTaskItem = MountainSeaMeetTaskItem or DeclareMono("MountainSeaMeetTaskItem", UIWidgetBaseItem)
function MountainSeaMeetTaskItem:SetData(data)
    local state = data.times == data.param_1 and 2 or 1 --2已完成
    UH.SetText(self.Desc,string.format(Language.MountainSeaMeet.Times[state],data.task_desc,data.times,data.param_1))
    self.RewardList:SetData((data.is_open or state == 2) and data.reward_item or data.unfinish_reward_item) --1领取  0前往 2已领取
    for i = 1,self.StateObjs:Length() do
        self.StateObjs[i]:SetActive(i == data.state + 1)
    end
    
    UIWidgetBaseItem.SetData(self,data)
end
function MountainSeaMeetTaskItem:OnClickGo()
    if ServerActivityData.Instance.mountain_sea_meet_data.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.MountainSeaMeet.GoToView)
        return
    end
    ViewMgr:OpenViewByKey(ServerActivityData.Instance:GetMountainSeaMeetViewModKey(self.data.task_type))
end
function MountainSeaMeetTaskItem:OnClickItem()
    if ServerActivityData.Instance.mountain_sea_meet_data.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.MountainSeaMeet.GoToView)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET,1,self.data.seq)
end

----------------------------MountainSeaFirstMeetGiftItem----------------------------
MountainSeaMeetRewardItem = MountainSeaMeetRewardItem or DeclareMono("MountainSeaMeetRewardItem", UIWidgetBaseItem)
function MountainSeaMeetRewardItem:SetData(data)
    if data.item ~= 0 then
        local item = Item.New(data.item.reward_item)
        self.Cell:SetData(item)
        UH.SetText(self.Name,item:Name())
    end
    self.Hide:SetActive(data.is_show == 0)
    UIWidgetBaseItem.SetData(self,data)
end
function MountainSeaMeetRewardItem:OnClickItem()
    if ServerActivityData.Instance.mountain_sea_meet_data.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.MountainSeaMeet.Tips)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET,2,self.data.index - 1)
end

----------------------------MountainSeaMeetGiftItem----------------------------
MountainSeaMeetGiftItem = MountainSeaMeetGiftItem or DeclareMono("MountainSeaMeetGiftItem", UIWidgetBaseItem)
function MountainSeaMeetGiftItem:SetData(data)
   self.Cell:SetData(Item.New(data))
   if data.is_open then
     self.Cell:MakeGrey(data.is_open)
   end
   UIWidgetBaseItem.SetData(self,data)
end

--山海活动提醒监听
--================================MainViewMountainSeaView================================
MainViewMountainSeaView = MainViewMountainSeaView or DeclareMono("MainViewMountainSeaView", UIWFlushPanel)

function MainViewMountainSeaView:MainViewMountainSeaView()
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,keys = {"act_type", "status"}},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = ActivityData.Instance.acts_tips_data,func = self.FlushTipsView,init_call = false},
    }

    self.Timer:SetShowCallBack(function (t)
        local t_str = ""

        if t.day ~= nil and tonumber(t.day) > 0 then 
            t_str = string.format(Language.UiTimeMeter.TimeStr3, t.day, t.hour ,t.min)
        elseif (t.day == nil or tonumber(t.day) == 0) and tonumber(t.hour) > 0 then 
            t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
        elseif (t.day == nil or tonumber(t.day) == 0) and (t.hour == nil or tonumber(t.hour) == 0) and tonumber(t.min) > 0 then 
            t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
        end 
        return t_str
    end)
end

function MainViewMountainSeaView:OnFlush()
    self:FlushTipsView()
end

function MainViewMountainSeaView:FlushTipsView()
    PlanActivityData.Instance:GetRandPlanList()
    self.Tips:SetActive(ServerActivityData.Instance:GetMountainSeaTip())
	local begin_time,end_time = PlanActivityData.Instance:GetActMSBeginAndEndAndDayTime()
	self.Timer:StampTime(end_time,TimeType.Type_Time_3, "")
end
