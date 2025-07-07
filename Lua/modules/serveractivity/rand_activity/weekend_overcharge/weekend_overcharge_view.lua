WeekendOverChargeView = WeekendOverChargeView or DeclareView("WeekendOverChargeView", "serveractivity/weekend_overcharge/weekend_overcharge", Mod.RandActivity.WeekendOverCharge)
function WeekendOverChargeView:WeekendOverChargeView()
    WeekendOverChargeCtrl.Instance:SendInfo()
end

function WeekendOverChargeView:OnClickClose()
    ViewMgr:CloseView(WeekendOverChargeView)
end

-- WeekendOverChargePanel
WeekendOverChargePanel = WeekendOverChargePanel or DeclareMono("WeekendOverChargePanel", UIWFlushPanel)
function WeekendOverChargePanel:WeekendOverChargePanel()
    self.data = WeekendOverChargeData.Instance

    self.data_cares = {
        {data = self.data.WeekendInfo, func = self.FlushShow},
    }

    self.ActTimer:CreateActTime(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI,TimeType.Type_Time_8,Language.ClothAroma.ActTime)
end 

function WeekendOverChargePanel:Awake()
    UIWFlushPanel.Awake(self)
end

function WeekendOverChargePanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[406].desc})
end

function WeekendOverChargePanel:FlushShow()
    local money = self.data:GetTotalChargeMoney()
    UH.SetText(self.TodayCharge,string.format(Language.WeekendOverCharge.TotalMoney,money))
    local list = self.data:GetOperShowList()
    for i = 1,3 do 
        self.OperShows[i].gameObject:SetActive(list[i] ~= nil)
        if list[i] ~= nil then 
            self.OperShows[i]:SetData(list[i])
        end 
    end 

    local day_index = self.data:GetTodayIndex()
    for i = 1,3 do 
        self.RunningShow[i]:SetActive(i == day_index)
    end
    
end

-- WeekendOCOperShow
WeekendOCOperShow = WeekendOCOperShow or DeclareMono("WeekendOCOperShow", UIWidgetBaseItem)
function WeekendOCOperShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Condition,data.condition_str)
    self.ShowDay1:SetData(data.day_1_show)
    self.ShowDay2:SetData(data.day_2_show)
    self.ShowDay3:SetData(data.day_3_show)
    self.CountineShow:SetData(data.day_countine)
end

-- WeekendOCDayItemShow
WeekendOCDayItemShow = WeekendOCDayItemShow or DeclareMono("WeekendOCDayItemShow", UIWidgetBaseItem)
function WeekendOCDayItemShow:WeekendOCDayItemShow()
    self.OperData = WeekendOverChargeData.Instance
end
function WeekendOCDayItemShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.ItemShow:SetData(data.item_show)
    self.ItemEff:SetActive(data.show_eff) -- 可领取特效
    self.Got:SetActive(data.show_get)     -- 已获得标记
    self.Lock:SetActive(data.show_lock)   
    self.Supply:SetActive(data.show_supply)     -- 可补领标记（补领+充值完毕
    UH.SetText(self.SupplyText,data.supply_str) -- 等待补领标记（补领+未充值完毕
    self.ClickBlock:SetActive(data.can_oper)
    self.BlockArea:SetActive(false)
    self.RedPos:SetNum(data.show_eff and 1 or 0)
end

function WeekendOCDayItemShow:OnClickOper()
    if not self.data.send_data.is_extra and 
        self.OperData:DayShowDone(
            self.data.send_data.index,
            self.data.send_data.seq ) then 
                PublicPopupCtrl.Instance:Center(Language.WeekendOverCharge.DayFetched)
        return 
    end 

    if self.data.send_data.is_extra and self.OperData:CountineShowDone(self.data.send_data.seq) then 
        PublicPopupCtrl.Instance:Center(Language.WeekendOverCharge.DayFetched)
        return 
    end

    if self.data.send_data.is_extra then 
        WeekendOverChargeCtrl.Instance:SendExtraReward(self.data.send_data)
    else
        WeekendOverChargeCtrl.Instance:SendReward(self.data.send_data)
    end 

end