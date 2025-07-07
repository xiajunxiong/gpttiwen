HeavenGiftView = HeavenGiftView or DeclareView("HeavenGiftView","serveractivity/heaven_gift_view",Mod.Splendid.HeavenGift)
VIEW_DECLARE_LEVEL(HeavenGiftView, ViewLevel.L1)
function HeavenGiftView:HeavenGiftView()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_HEAVEN_GIFT))
end

function HeavenGiftView:LoadCallback()
    self.Panels[ServerActivityData.Instance:GetHeavenGiftShow()]:SetActive(true)
end

function HeavenGiftView:OnClickClose()
    ViewMgr:CloseView(HeavenGiftView)
end

function HeavenGiftView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[42].desc)
end


HeavenGiftPanel = HeavenGiftPanel or DeclareMono("HeavenGiftPanel", UIWFlushPanel)
--EnterInter=============InteractorHandle
--Times==================Text
function HeavenGiftPanel:HeavenGiftPanel()
    self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.heaven_gift_data, func = self.OnRecieveCountChange,"receive_count"},
	}
end

function HeavenGiftPanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.data:IsRefreshingBox() then
        self.data.heaven_gift_data.remind_check = true
    end
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_HEAVEN_GIFT, TimeType.Type_Special_4)
    self.RewardList:SetData(self:GetRewardList())
    local config = Config.tianjianghaoli_auto.other[1]
    local s_time, e_time = self:GetTime(config.time_start), self:GetTime(config.time_end)
    local time_tip = Language.HeavenGift.TimeTip
    local color = Language.HeavenGift.TimeTipColor[self.data:GetHeavenGiftShow()]
    UH.SetText(self.OpenTime, Format(time_tip, color,s_time.hour, s_time.min, e_time.hour, e_time.min,color, config.time_the,color, config.map_case_number))
end


function HeavenGiftPanel:OnRecieveCountChange()
    local frequency = Config.tianjianghaoli_auto.other[1].reward_frequency
    local color=Language.HeavenGift.TimesTipColor[self.data:GetHeavenGiftShow()]
    UH.SetText(self.Times,Format(Language.HeavenGift.TimesTip,color,self.data:GetHeavenGiftCount(),frequency))
    self.EnterInter.Interactable = self.data:IsHasHeavenGiftCount() and self.data:IsRefreshingBox()
end

function HeavenGiftPanel:OnClickEnter()
    if not self.EnterInter.Interactable then
        if not self.data:IsHasHeavenGiftCount() then
            PublicPopupCtrl.Instance:Center(Language.HeavenGift.NotTimesTip)
        else
            PublicPopupCtrl.Instance:Center(Language.HeavenGift.NotTimeTip)
        end
        return
    end

    local EnterFunc = function ()
        if self.data:IsHasHeavenGiftCount() then
            SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(),nil,function()
                local treasure_chest_id = Config.tianjianghaoli_auto.case_coordinate[1].treasure_chest_id
                AutoInvokerCtrl.Instance:SetCommands({AutoCommandGather.New(treasure_chest_id)}, 0)
            end,nil,true)
        else
            SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(),nil,nil,nil,true)
        end
        ActivityRandData.Instance:CheckRandChildData()
        ViewMgr:CloseView(HeavenGiftView)
        ViewMgr:CloseView(PlanActivityView)
    end

    if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.HeavenGift.EnterTip,function ()
            LoginCtrl.Instance:SendBackToOriginServer()
            EnterFunc()
        end)
    else
        EnterFunc()
    end
end

function HeavenGiftPanel:GetRewardList()
    local list = {}
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HEAVEN_GIFT, Config.tianjianghaoli_auto.case_exhibition, "seq")
    for i, v in ipairs(cfg) do
        if v.item_show == 1 then
            table.insert(list, Item.Create(v.reward_item))
        end
    end
    return list
end

function HeavenGiftPanel:GetTime(time)
    local hour = math.floor(time / 100)
    local min = time - hour * 100
    return {hour = hour, min = min < 10 and tostring("0" .. min) or min}
end
