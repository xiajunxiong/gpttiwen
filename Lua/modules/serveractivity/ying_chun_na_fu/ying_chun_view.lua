YingChunView = YingChunView or DeclareView("YingChunView", 'serveractivity/ying_chun_na_fu/ying_chun_view', Mod.RabbitYear.YingChunNaFu)
function YingChunView:YingChunView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.YxDm.Time, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU, TimeType.Type_Time_3)
end

function YingChunView:LoadCallback(param_t)
    AppearanceCtrl.Instance.is_shield_appear_show = true  --屏蔽皮肤展示
end

YingChunPanel = YingChunPanel or DeclareMono("YingChunPanel", UIWFlushPanel)

function YingChunPanel:YingChunPanel()
    self.data = YingChunData.Instance
    self.data_cares = {
        -- {data = self.data.pray_data, func = self.FlushCurrNum, init_call = false, keys = {"curr_num"}},
        {data = self.data.ying_chun_data, func = self.ShowDrawResult, keys = {"flush_notice"}, init_call = false},
        {data = self.data.ying_chun_data, func = self.FlushItemNum, keys = {"flush_curr"}, init_call = false},
    }
    self.is_drawing = false -- 是否正在抽奖
    self.data:ClearFirstRemind()
end


function YingChunPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ToggleSkipAni.isOn = self.data.isSkipAni
    self:ShowReward()
    self:FlushItemNum()
end

function  YingChunPanel:FlushItemNum()
    local num=Item.GetNum(self.data:GetConsunmeItem())
    UH.SetText(self.CurrNum,num)
    self.RedOne:SetNum(num>=1 and 1 or 0)
    self.RedTen:SetNum(num>=10 and 1 or 0)
end

function YingChunPanel:ShowReward()
    local cfgs = self.data:GetShowCfg()
    self.rewards = {}
    local index = 1
    for _, v in pairs(cfgs) do
        if v.is_award == 1 then
            self.rewards[5] = v.reward_item
            local path = IllusionData.GetLiHuiByItemId(v.reward_item.item_id)
            if path then
                self.Icons[5]:SetByPath(path)
            end
            local name = Item.GetName(v.reward_item.item_id)
            UH.SetText(self.Name,name)
            UH.SetText(self.Nums[5],v.reward_item.num)
            self:ShowModel()
        else
            self.rewards[index] = v.reward_item
            UH.SetIcon(self.Icons[index], Item.GetIconId(v.reward_item.item_id), ICON_TYPE.ITEM)
            UH.SetText(self.Nums[index], v.reward_item.num)
            index = index + 1
        end
    end
end

function YingChunPanel:ShowModel()
    local res_path = DrawerHelper.GetDataByItem(Item.Create(self.rewards[5]).vo)
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData({ui_obj = uo, view = YingChunView, load_callback = nil})
    else
        uo:SetData(res_path)
    end
end

function YingChunPanel:ShowRewardDetail(index)
    if self.rewards ~= nil and self.rewards[index] ~= nil  then
        CellClicks.BagGridNoButton({
            GetData = function()
                return Item.Create(self.rewards[index]) 
            end,
        })
    end
end

function YingChunPanel:ShowRecord()
    ViewMgr:OpenView(YingChunRecordView)
end

function YingChunPanel:ShowProbability()
    ViewMgr:OpenView(YingChunProBabilityView)
end

function YingChunPanel:Lotery(num)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    if self.is_drawing then
        return
    end
    local has_num = Item.GetNum(self.data:GetConsunmeItem())
    if has_num >= num then
        self.is_drawing = true
        local type = num == 1 and YingChunCtrl.ReqType.Lotery or YingChunCtrl.ReqType.Lotery10
        PublicPopupCtrl.Instance:SetShield() -- 屏蔽激活飘字
        AchieveCtrl.Instance:DeLayCourseRemind()  --延迟成就展示
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU, type)
    else
        MainOtherCtrl.Instance:GetWayView({item = Item.New({item_id = self.data:GetConsunmeItem()})})
        -- ItemInfoCtrl.Instance:ItemInfoView({item_list = {Item.Init(self.data:GetConsunmeItem(), 1, 0)}})
        PublicPopupCtrl.Instance:Center(Language.Bag.NotEnoughItem)
    end
end

function YingChunPanel:OnClickCur()
    CellClicks.BagGridNoButton({
        GetData = function()
            return Item.Create(Item.Create({item_id = self.data:GetConsunmeItem()}))
        end,
    })
end

function YingChunPanel:OnClickSkip(toggle)
    self.data.isSkipAni = toggle
end

function YingChunPanel:ShowDrawResult()
    if self.data.ying_chun_notice ~= nil and #self.data.ying_chun_notice.info.list > 0 then
        if self.data.isSkipAni == true then
            ViewMgr:OpenView(YingChunReward)
            self.is_drawing = false
        else
            self.DrawAni:Stop()
            self.DrawAni:Play()
            UH.PlayEffect(self.EffectTool, 6164002)
            TimeHelper:CancelTimer(self.ShowTimer)
            self.ShowTimer = TimeHelper:AddDelayTimer(function()
                ViewMgr:OpenView(YingChunReward)
                self.is_drawing = false
            end, 3.2)
        end
    end
end



function YingChunPanel:OnCloseClick(data)
    if not self.is_drawing then
        AppearanceCtrl.Instance.is_shield_appear_show = false -- 放开屏蔽皮肤展示
        ViewMgr:CloseView(YingChunView)
    end
end





