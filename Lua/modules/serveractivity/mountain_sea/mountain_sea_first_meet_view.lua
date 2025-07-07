MountainSeaFirstMeetView = MountainSeaFirstMeetView or DeclareView("MountainSeaFirstMeetView", "serveractivity/mountain_sea/mountain_sea_first_meet", Mod.MountainSea.FirstMeet)
function MountainSeaFirstMeetView:MountainSeaFirstMeetView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET,TimeType.Type_Time_8,Language.MountainSeaFirstMeet.ActTime)
end

function MountainSeaFirstMeetView:OnClickClose()
    ViewMgr:CloseView(MountainSeaFirstMeetView)
end
function MountainSeaFirstMeetView:OnClickDesc()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[179].desc})
end
----------------------------MountainSeaFirstMeetPanel----------------------------
MountainSeaFirstMeetPanel = MountainSeaFirstMeetPanel or DeclareMono("MountainSeaFirstMeetPanel", UIWFlushPanel)
function MountainSeaFirstMeetPanel:MountainSeaFirstMeetPanel()
    self.Data = ServerActivityData.Instance
    self.data = self.Data.mountain_sea_first_meet_data
    self.data_cares = {
        {data = self.data, func = self.FlushPanel, init_call = true},
        {data = self.Data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
        {data = ActivityData.Instance.activity_status_event, func = BindTool.Bind(self.FlushTask,self),keys = {"act_type","status"}},
    }
end
function MountainSeaFirstMeetPanel:FlushTask()
    self.Data:SetMountainSeaFirstMeetFulsh()
    self:FlushPanel()
end
function MountainSeaFirstMeetPanel:FlushPanel()
    self.RewardLits:SetData(self.data.reward_info)
    self.TaskList:SetData(self.data.task_list)
    self.BuyObj:SetActive(self.data.is_buy == 0)
    UH.SetText(self.Moneny,string.format(Language.MountainSeaFirstMeet.Moneny,Config.shanhaichuyu_auto.other[1].buy_money / 10))
end

function MountainSeaFirstMeetPanel:OnBuyClick()
    self.price = Config.shanhaichuyu_auto.other[1].buy_money
    local fee = self.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_FIRST_MEET,fee,ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET,1)
end

function MountainSeaFirstMeetPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_FIRST_MEET, self.price)
end


function MountainSeaFirstMeetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

----------------------------MountainSeaFirstMeetTaskItem----------------------------
MountainSeaFirstMeetTaskItem = MountainSeaFirstMeetTaskItem or DeclareMono("MountainSeaFirstMeetTaskItem", UIWidgetBaseItem)
function MountainSeaFirstMeetTaskItem:SetData(data)
    local state = data.times >= data.param_1 and 2 or 1 --2已完成
    UH.SetText(self.Desc,string.format(Language.MountainSeaMeet.Times[state],data.task_desc,data.times >= data.param_1 and data.param_1 or data.times,data.param_1))
    UH.SetText(self.BtnText,Language.MountainSeaFirstMeet.TaskState[state])
    UH.SetText(self.StateTxt,Language.MountainSeaFirstMeet.State[data.state])
    local day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
    day = day + ((data.state == 2 and day < 6) and 1 or 0) 
    UH.SetText(self.Name,data.state == 0 and [[]] or Language.MountainSeaFirstMeet.Days[day])
    UH.SetInter(self.BtnInter,data.state == 1 and state == 1)
    local index = data.state == 2 and 0 or data.state
    for i = 1,self.StateObjs:Length() do
        self.StateObjs[i]:SetActive(i == index + 1)
    end
    UIWidgetBaseItem.SetData(self,data)
end
function MountainSeaFirstMeetTaskItem:OnClickGo()
    if ServerActivityData.Instance.mountain_sea_first_meet_data.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.MountainSeaFirstMeet.Tips)
        return
    end
    ViewMgr:OpenViewByKey(ServerActivityData.Instance:GetMountainSeaFirstMeetActData(self.data.task_type).activity_mod_key)
end
----------------------------MountainSeaFirstMeetGiftItem----------------------------
MountainSeaFirstMeetGiftItem = MountainSeaFirstMeetGiftItem or DeclareMono("MountainSeaFirstMeetGiftItem", UIWidgetBaseItem)
function MountainSeaFirstMeetGiftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.DoubleReward:SetActive(ServerActivityData.Instance.mountain_sea_first_meet_data.is_buy == 1)
    local show_text = Language.MountainSeaFirstMeet.Extra[data.is_extra]
    if ServerActivityData.Instance.mountain_sea_first_meet_data.is_buy == 0 and data.is_extra == 0 then
        show_text = " "
    end
    UH.SetText(self.Name,Language.MountainSeaFirstMeet.Days[data.activity_days])
    local state = data.state
    local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
    local show_list = data.reward_item
    local is_fetch = data.is_finish == 1
    if data.activity_days > cur_open_day then
        state = 2 
        self.Time:StampTime(ServerActivityData.Instance:GetMountainSeaFirstMeetTime(data.activity_days),TimeType.Type_Special_4,Language.MountainSeaFirstMeet.Time)
        for i,v in ipairs(DataHelper.FormatList(show_list)) do
            v.is_open = true
        end
    else 
        self.is_finish = data.is_finish == 1
        if data.is_finish == 0 and data.activity_days < cur_open_day then --未完成任务
            show_list = data.unfinish_reward_item
            is_fetch = true
        end
        --if data.state == 1 then 
        --    is_fetch = false
        --end
        for i,v in ipairs(DataHelper.FormatList(show_list)) do
            v.is_open = false
        end
    end
    UH.SetInter(self.ShowInter,state < 2)
    --[[self.Extra:SetActive(data.is_extra == 1) -->>屏蔽
    self.is_extra = false
    UH.SetInter(self.ShowInter,state < 2)
    --额外奖励标识
    if data.is_extra == 0 and cur_open_day > data.activity_days then 
        is_fetch = true
        show_text = Language.MountainSeaFirstMeet.Extra[1]
        self.Extra:SetActive(true)
        self.is_extra = data.is_finish == 1
        if data.state == 0 or data.is_finish == 1 then --已经领取奖励了   任务完成又可领取
            state = 0
        end
    end
    --当天额外奖励领取
    if data.is_extra == 0 and ServerActivityData.Instance.mountain_sea_first_meet_data.is_buy == 1 and cur_open_day == data.activity_days and data.state == 1 then 
        is_fetch = true
        state = 0
        show_text = Language.MountainSeaFirstMeet.Extra[0]
    end
    if data.is_extra == 1 and cur_open_day == data.activity_days then 
        show_text = Language.MountainSeaFirstMeet.Extra[0]
        self.Extra:SetActive(false)
    end]]
    show_text = " "
    UH.SetText(self.ExtraTxt,show_text)
    UH.SetInter(self.BtnInter,is_fetch)
    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end
    if is_fetch then
        self.effect = self.EffectTool:Play(5161028)
    end
    for i = 1,self.StateObjs:Length() do
        self.StateObjs[i]:SetActive(i == state + 1)
    end
    self.RewardList:SetData(show_list)
end

function MountainSeaFirstMeetGiftItem:OnClickItem()
    local type = 1  -- 通常奖励
    if ServerActivityData.Instance.mountain_sea_first_meet_data.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.MountainSeaFirstMeet.Tips)
        return
    end
    local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
    --[[if  self.is_extra then
        type = 3 --额外奖励
    end
    if self.data.is_extra == 0 and self.is_finish then
        type = 4 --额外奖励 + 通常奖励
    end]]
    if self.is_finish == false and cur_open_day ~= self.data.activity_days then
        type = 2 --未完成奖励
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET,0,self.data.seq,type) 
end