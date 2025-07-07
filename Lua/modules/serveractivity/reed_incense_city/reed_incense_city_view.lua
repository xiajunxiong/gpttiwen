ReedIncenseCityView = ReedIncenseCityView or DeclareView("ReedIncenseCityView", "serveractivity/reed_incense_city/reed_incense_city", Mod.DragonBoat.ReedIncenseCity)
VIEW_DECLARE_LEVEL(ReedIncenseCityView, ViewLevel.L1)
VIEW_DECLARE_MASK(ReedIncenseCityView, ViewMask.BgBlock)
function ReedIncenseCityView:ReedIncenseCityView()
    
end

function ReedIncenseCityView:LoadCallback(param_t)
    
end

function ReedIncenseCityView:CloseCallback()

end

function ReedIncenseCityView:OnCloseClick()
    ViewMgr:CloseView(ReedIncenseCityView)
end

----------------------------ReedIncenseCityPanel----------------------------
ReedIncenseCityPanel = ReedIncenseCityPanel or DeclareMono("ReedIncenseCityPanel", UIWFlushPanel)
function ReedIncenseCityPanel:ReedIncenseCityPanel()
    self.Data = ReedIncenseCityData.Instance
    self.language = Language.ReedIncenseCity
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = true},
        {data = ActivityData.Instance.active_data, func = self.FlushAll, keys = {"active_num"}, init_call = false},
    }
end

function ReedIncenseCityPanel:Awake()
    UIWFlushPanel.Awake(self)
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY) - 1)
        UH.SetText(self.TxtStartMonth, open_t.month)
        UH.SetText(self.TxtStartDay, open_t.day)
        UH.SetText(self.TxtEndMonth, end_t.month)
        UH.SetText(self.TxtEndDay, end_t.day)
    end
    UH.SetText(self.TxtDesc, Format(self.language.Desc, self.Data:GetNeedActive()))
end

function ReedIncenseCityPanel:FlushAll()
    for i = 1, self.Reminds:Length() do
        local num = self.Data:SingleRemind(i - 1)
        self.Reminds[i]:SetNum(num)
        self.Effects[i]:SetActive(num > 0)
    end
end

function ReedIncenseCityPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- seq从0~7
function ReedIncenseCityPanel:OnBtnClick(seq)
    local num, p2, p3 = self.Data:SingleRemind(seq)
    if num == 1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        ReedIncenseCityCtrl.Instance:SendReq(seq)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        if p3 then
            PublicPopupCtrl.Instance:Center(Format(self.language.Tip[p2], p3))
        else
            PublicPopupCtrl.Instance:Center(self.language.Tip[p2])
        end
    end
end

-- index为1或者2
function ReedIncenseCityPanel:OnRewardClick(index)
    local cfg = self.Data:RewardShowCfg()
    local t = {}
    t.item_list = DataHelper.FormatItemList(cfg["reward_item_"..index])
    ViewMgr:OpenView(ReedIncenseCityShowItemView, t)
end

----------------------------ReedIncenseCityShowItemView----------------------------
ReedIncenseCityShowItemView = ReedIncenseCityShowItemView or DeclareView("ReedIncenseCityShowItemView", "serveractivity/reed_incense_city/reed_incense_city_show_item_view")
VIEW_DECLARE_MASK(ReedIncenseCityShowItemView, ViewMask.BlockClose)
function ReedIncenseCityShowItemView:LoadCallback(param_t)
    self.ShowList:SetData(param_t.item_list)
end