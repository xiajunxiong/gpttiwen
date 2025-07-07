MountainSeaWindMoonView = MountainSeaWindMoonView or DeclareView("MountainSeaWindMoonView", "serveractivity/mountain_sea/mountain_sea_wind_moon", Mod.MountainSea.WindMoon)
VIEW_DECLARE_LEVEL(MountainSeaWindMoonView, ViewLevel.L1)
VIEW_DECLARE_MASK(MountainSeaWindMoonView, ViewMask.BgBlock)

function MountainSeaWindMoonView:MountainSeaWindMoonView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_WIND_MOON, TimeType.Type_Special_4)
end

function MountainSeaWindMoonView:OnClickClose()
    ViewMgr:CloseView(MountainSeaWindMoonView)
end

function MountainSeaWindMoonView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[170].desc)
end

----------------------------MountainSeaWindMoonPanel----------------------------
MountainSeaWindMoonPanel = MountainSeaWindMoonPanel or DeclareMono("MountainSeaWindMoonPanel", UIWFlushPanel)
function MountainSeaWindMoonPanel:MountainSeaWindMoonPanel()
    self.Data = ServerActivityData.Instance
    self.reward_data = self.Data.mountain_sea_wind_moon_reward_data
    self.state_data = self.Data.mountain_sea_wind_moon_state_data
    self.lottery_data = self.Data.mountain_sea_wind_moon_lottery_data
    self.language = Language.MountainSeaWindMoon
    self.data_cares = {
        {data = self.reward_data, func = self.FlushGift, init_call = false},
        {data = self.state_data, func = self.FlushGift, init_call = false},
        {data = self.lottery_data, func = self.StartLottery, init_call = false},
    }
    self:FlushPanel()
end

function MountainSeaWindMoonPanel:FlushPanel()
    UH.SetText(self.TxtMin, Config.integral_point_choujiang_auto.other[1].continuou_times)
    for i = 1, self.RollerList:Length() do
        -- 转动结束后才刷新左侧礼包数量
        self.RollerList[i].Roller:SetData({end_func = BindTool.Bind(self.RollerEnd, self)})
    end
    self:FlushGift()
end

function MountainSeaWindMoonPanel:RollerEnd()
    if self:JudgeIsPlaying() then
        return
    else
        self.ObjBlock:SetActive(false)
        self:FlushGift()
    end
end

function MountainSeaWindMoonPanel:FlushGift()
    if self:JudgeIsPlaying() or GetActive(self.ObjBlock) then
        return
    end
    self.config = self.Data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_WIND_MOON, Config.integral_point_choujiang_auto.basis_configuration, "color_num")
    for i = 1, self.GiftItemList:Length() do
        self.GiftItemList[i]:SetData(self.config[i])
    end
    self.RedPoint:SetNum(self.Data:MountainSeaWindMoonBtnRemind())
    UH.SetText(self.TxtTimes, Format(self.language.Times, self.reward_data.times))
    self.ObjBlock:SetActive(false)
end

function MountainSeaWindMoonPanel:StartLottery()
    for i = 1, self.RollerList:Length() do
        self.RollerList[i]:SetData({seq = self.lottery_data.info[i]})
    end
end

function MountainSeaWindMoonPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function MountainSeaWindMoonPanel:OnStartClick()
    if self:JudgeIsPlaying() then
        return
    end
    if self.state_data.is_can_lottery then
        if self.reward_data.times > 0 then
            -- 防止连点
            self.ObjBlock:SetActive(true)
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_WIND_MOON, 1)
        else
            PublicPopupCtrl.Instance:Center(self.language.TimesNotEnought)
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.TimeNotStart)
    end
end

function MountainSeaWindMoonPanel:JudgeIsPlaying()
    for i = 1, self.RollerList:Length() do
        if self.RollerList[i].Roller:GetIsPlaying() then
            return true
        end
    end
end

----------------------------MountainSeaWindMonnGiftItem----------------------------
MountainSeaWindMonnGiftItem = MountainSeaWindMonnGiftItem or DeclareMono("MountainSeaWindMonnGiftItem", UIWidgetBaseItem)
function MountainSeaWindMonnGiftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, Language.MountainSeaWindMoon.GiftName[data.color_num])
    UH.SetText(self.TxtSameColor, Format(Language.MountainSeaWindMoon.SameColor, data.color_num))
    self.can_num = ServerActivityData.Instance:GetMountainSeaWindMoonCanGetNum(data.color_num)
    self.ObjNum:SetActive(self.can_num > 0)
    UH.SetText(self.TxtNum, self.can_num)
end

function MountainSeaWindMonnGiftItem:OnGiftClick()
    if self.can_num > 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_WIND_MOON, 2, self.data.seq)
    else
        local tab = {
            pos = UH.GetRootPosition(self.ObjShowPos),
            item_list = DataHelper.FormatItemList(self.data.reward_item),
            pivot = 0,
        }
        ViewMgr:OpenView(ShowItemView, tab)
    end
end

----------------------------MountainSeaWindMonnRollerItem----------------------------
MountainSeaWindMonnRollerItem = MountainSeaWindMonnRollerItem or DeclareMono("MountainSeaWindMonnRollerItem", UIWidgetBaseItem)
function MountainSeaWindMonnRollerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Roller:PlayRoller(data.seq)
end