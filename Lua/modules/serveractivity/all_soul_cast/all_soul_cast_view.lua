AllSoulCastView = AllSoulCastView or DeclareView("AllSoulCastView", "serveractivity/all_soul_cast/all_soul_cast", Mod.MayDay.AllSoulCast)
function AllSoulCastView:OnClickClose()
    ViewMgr:CloseView(AllSoulCastView)
end

AllSoulCastPanel = AllSoulCastPanel or DeclareMono("AllSoulCastPanel", UIWFlushPanel)
function AllSoulCastPanel:AllSoulCastPanel()
    self.data = AllSoulCastData.Instance
    self.data_cares = {
        {data = AllSoulCastData.Instance.act_info, func = self.FlushView},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }
end

function AllSoulCastPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:ShowActTime()
    UH.SetText(self.BtnBuyText, RechargeData.Instance:GoldToMoney(self.data:GetBuyPrice()))
    self.FreeItemRed:SetNum(1)
    UH.SetText(self.TipsText, self.data:OtherConfig().tips)

    local function func_anim()
        self.RewardAnim:Play("ItemAnim")
    end
    self.anim_timer = Invoke(func_anim, 0.3)

    local origin = 1425 / 750
    local current = UnityEngine.Screen.width / UnityEngine.Screen.height
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        current = (UnityEngine.Screen.width - 120) / UnityEngine.Screen.height
        UH.LocalPosG(self.BirdObj, Vector3.New(95, 80, 0))
    end
    if current > origin then
        UH.LocalScaleG(self.BirdObj, Vector3.New(current / origin, 1, 1))
    else
        UH.LocalScaleG(self.BirdObj, Vector3.New(1, origin / current, 1))
    end
end

function AllSoulCastPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.anim_timer)
    self.anim_timer = nil
    self.BirdObj:SetActive(false)
end

function AllSoulCastPanel:FlushView()
    local show_list = self.data:GetShowList()
    self.last_reward = table.remove(show_list, #show_list)
    for i = 1, 6 do
        self.RewardList[i]:SetData(show_list[i])
    end
    local yet_get = AllSoulCastData.Instance:IsRewardFetched(self.last_reward.seq)
    local enough_day = AllSoulCastData.Instance:LoginDay() >= self.last_reward.need_day
    self.LastItemCell:SetData(Item.Create(self.last_reward.reward_item[0]))
    self.LastCanGet:SetActive(not yet_get and enough_day)
    self.LastYetGet:SetActive(yet_get)

    local is_free_fetched = self.data:IsFreeFetched()
    self.FreeItem:SetActive(not is_free_fetched)
    self.BtnBuy:SetActive(not self.data:IsRewardActived())
end

function AllSoulCastPanel:ShowActTime()
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_ALL_SOUL_COST)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_ALL_SOUL_COST) - 1)
        UH.SetText(self.ActTime, Format(Language.AllSoulCast.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.ActTime, Format(Language.AllSoulCast.ActTime, 1, 1, 1, 1))
    end
end

function AllSoulCastPanel:OnClickBuy()
    AllSoulCastCtrl.Instance:SendBuy()
end

function AllSoulCastPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ALL_SOUL_CAST, self.data:GetBuyPrice())
end

function AllSoulCastPanel:OnClickFreeFetch()
    AllSoulCastCtrl.Instance:SendFreeFetch()
end

function AllSoulCastPanel:OnClickGetLast()
    if self.last_reward ~= nil then
        local is_actived = AllSoulCastData.Instance:IsRewardActived()
        if not is_actived then
            PublicPopupCtrl.Instance:Center(Language.AllSoulCast.FetchError1)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            return
        end
        AllSoulCastCtrl.Instance:SendFetch(self.last_reward.seq, 1) 
    end
end

function AllSoulCastPanel:OnClickOpenTips()
    self.TipsView:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
end

function AllSoulCastPanel:OnClickCloseTips()
    self.TipsView:SetActive(false)
end


AllSoulCastItem = AllSoulCastItem or DeclareMono("AllSoulCastItem", UIWidgetBaseItem)
function AllSoulCastItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.DayText, Format(Language.AllSoulCast.DayStr, DataHelper.GetDaXie(data.need_day)))

    local yet_get = AllSoulCastData.Instance:IsRewardFetched(data.seq)
    self.YetGet:SetActive(yet_get)
    local enough_day = AllSoulCastData.Instance:LoginDay() >= data.need_day
    local can_get = enough_day and not yet_get
    self.CanGet:SetActive(can_get)
    local item_list = DataHelper.FormatItemList(data.reward_item)
    self.ItemList:SetData(item_list)
    for i = 1, 3 do
        self.ItemEffectList[i]:SetActive(can_get and i <= #item_list)
    end
end

function AllSoulCastItem:OnClickFetch()
    local is_actived = AllSoulCastData.Instance:IsRewardActived()
    if not is_actived then
        PublicPopupCtrl.Instance:Center(Language.AllSoulCast.FetchError1)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    AllSoulCastCtrl.Instance:SendFetch(self.data.seq)
end

AllSoulCastCellItem = AllSoulCastCellItem or DeclareMono("AllSoulCastCellItem", UIWidgetBaseItem)
function AllSoulCastCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
end
