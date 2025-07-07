AllSoulsFullView = AllSoulsFullView or DeclareView("AllSoulsFullView", "serveractivity/all_souls_full", Mod.PlanActivity.AllSoulsFull)
VIEW_DECLARE_MASK(AllSoulsFullView, ViewMask.BgBlockClose)
function AllSoulsFullView:LoadCallback()
    if ServerActivityData.Instance:GetAllSoulsFullTodayRemind() == 1 then
        ServerActivityData.Instance:SaveAllSoulsFullTodayRemind()
        ServerActivityData.Instance:CheckAllSoulsFullRed()
    end
    ServerActivityData.Instance:ClearAllSoulsFullNewSign()
end

function AllSoulsFullView:OnClickClose()
    ViewMgr:CloseView(AllSoulsFullView)
end

function AllSoulsFullView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(168)
end

AllSoulsFullPanel = AllSoulsFullPanel or DeclareMono("AllSoulsFullPanel", UIWFlushPanel)
function AllSoulsFullPanel:AllSoulsFullPanel()
    self.server_data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.server_data.all_souls_full_info, func = self.FlushView},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }
end

function AllSoulsFullPanel:Awake()
    UIWFlushPanel.Awake(self)
    local act_name = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING)
    UH.SetText(self.Title, act_name)
    UH.SetText(self.DialogText, act_name)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING, TimeType.Type_Special_4)

    local other_cfg = self.server_data:GetAllSoulsFullOtherCfg()
    UH.SetText(self.PriceText, RechargeData.Instance:GoldToMoney(other_cfg.buy_money))
end

function AllSoulsFullPanel:FlushView()
    self.BtnBuy:SetActive(not ServerActivityData.Instance:GetAllSoulsFullIsActive())
    
    local open_list, standy_list, over_list = self.server_data:AllSoulsFullFilter()
    self.OpenItem.gameObject:SetActive(#open_list > 0)
    if #open_list > 0 then
        self.OpenItem:SetData(open_list[1])
    end
    self.StandyList:SetData(standy_list)
    self.NoStandy:SetActive(#standy_list == 0)

    local is_show = #over_list > 0
    local function func_sort(data)
        local task_info = ServerActivityData.Instance:GetAllSoulsFullTaskInfo(data.task_id)
        if task_info ~= nil then
            -- 可领取 -> 已领取 -> 已结束
            if task_info.task_flag == 2 then
                return 1000
            -- elseif task_info.param1 > 0 then
            --     return 0
            end
            return 0
        end
        return 2000
    end
    table.sort(over_list, DataHelper.WeightSort("task_id", func_sort))
    -- for i, v in pairs(over_list) do
    --     local task_info = ServerActivityData.Instance:GetAllSoulsFullTaskInfo(v.task_id)
    --     if task_info ~= nil and task_info.task_flag ~= 2 and task_info.param1 > 0 then
    --         is_show = true
    --         break
    --     end
    -- end
    self.OverList.gameObject:SetActive(is_show)
    self.NoOver:SetActive(not is_show)
    if is_show then
        self.OverList:SetData(over_list)
    end
end

function AllSoulsFullPanel:OnClickActive()
    local fee = self.server_data:GetAllSoulsFullOtherCfg().buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_WAN_LING_GONG_YING, fee, ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING, 2, 0)
end

function AllSoulsFullPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_WAN_LING_GONG_YING, self.server_data:GetAllSoulsFullOtherCfg().buy_money)
end

AllSoulsFullActItem = AllSoulsFullActItem or DeclareMono("AllSoulsFullActItem", UIWidgetBaseItem)
function AllSoulsFullActItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TitleText, ActivityRandData.GetName(data.act_type))
    UH.SetText(self.TimeText, self:GetTimeStr())
end

function AllSoulsFullActItem:GetTimeStr()
    local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(self.data.act_type)
    if open_time ~= nil then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", end_time - 1) --比如配置的23号结束，实际是22号24点结束的
        return Format(Language.AllSoulsFull.ActTime, open_t.month, open_t.day, end_t.month, end_t.day)
    end
    return ""
end

function AllSoulsFullActItem:GetTaskInfo()
    return ServerActivityData.Instance:GetAllSoulsFullTaskInfo(self.data.task_id) or {}
end

--进行中
AllSoulsFullOpenItem = AllSoulsFullOpenItem or DeclareMono("AllSoulsFullOpenItem", AllSoulsFullActItem)
function AllSoulsFullOpenItem:SetData(data)
    AllSoulsFullActItem.SetData(self, data)
    self.ItemCell:SetData(Item.New(data.reward_item[0]))

    local task_info = self:GetTaskInfo()
    local task_prog = task_info.param1 or 0
    self.BtnGet.gameObject:SetActive(task_info.task_flag == 1)
    self.ImgGet.gameObject:SetActive(task_info.task_flag == 2)
    self.BtnGo.gameObject:SetActive(task_info.task_flag ~= 1 and task_info.task_flag ~= 2)
    UH.SetText(self.DescText, data.task_desc .. ColorStr(Format("(%d/%d)", task_prog, data.param_1), COLORSTR.Green9))
end

function AllSoulsFullOpenItem:OnClickGet()
    if not ServerActivityData.Instance:GetAllSoulsFullIsActive() then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsFull.Error1)
        return
    end
    ServerActivityCtrl.Instance:SendAllSoulsFullReq(1, self.data.task_id)
end

function AllSoulsFullOpenItem:OnClickGo()
    if not ServerActivityData.Instance:GetAllSoulsFullIsActive() then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsFull.Error1)
        return
    end
    if ActivityData.IsOpen(self.data.act_type) then
        if ActivityRandData.Instance:OnClickHandle(self.data.act_type) then
        else
            ActivityRandData.SwitchView(self.data.act_type,true)
        end
        ViewMgr:CloseView(AllSoulsFullView)
    else
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
    end
end

--即将开始
AllSoulsFullStandyItem = AllSoulsFullStandyItem or DeclareMono("AllSoulsFullStandyItem", AllSoulsFullActItem)
function AllSoulsFullStandyItem:SetData(data)
    AllSoulsFullActItem.SetData(self, data)
    self.ItemCell:SetData(Item.New(data.reward_item[0]))
    UH.SetText(self.DescText, data.task_desc .. ColorStr(Format("(%d/%d)", 0, data.param_1), COLORSTR.Green9))
end

--已结束
AllSoulsFullOverItem = AllSoulsFullOverItem or DeclareMono("AllSoulsFullOverItem", AllSoulsFullActItem)
function AllSoulsFullOverItem:SetData(data)
    AllSoulsFullActItem.SetData(self, data)

    local task_info = self:GetTaskInfo()
    local task_prog = task_info.param1 or 0
    UH.SetText(self.DescText, data.task_desc .. ColorStr(Format("(%d/%d)", task_prog, data.param_1), COLORSTR.Green9))

    self.ImgGet.gameObject:SetActive(task_info.task_flag == 2)

    if task_prog == data.param_1 then
        self.ItemCell:SetData(Item.New(data.reward_item[0]))
    else
        self.ItemCell:SetData(Item.New(data.unfinish_reward_item[0]))
    end
    self.ItemCell:MakeGrey(true)
    if task_info.task_flag == 2 then --已领取
        self.BtnGetInter.gameObject:SetActive(false)
    -- elseif task_prog == 0 then
    --     self.BtnGetInter.gameObject:SetActive(true)
    --     self.BtnGetInter.Interactable = false
    --     UH.SetText(self.BtnGetText, Language.AllSoulsFull.BtnState[4])
    --     self.BtnEffect:SetActive(false)
    else
        self.BtnGetInter.Interactable = true
        self.BtnGetInter.gameObject:SetActive(true)
        UH.SetText(self.BtnGetText, Language.AllSoulsFull.BtnState[1])
        self.BtnEffect:SetActive(true)
    end
end

function AllSoulsFullOverItem:OnClickGet()
    if not ServerActivityData.Instance:GetAllSoulsFullIsActive() then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsFull.Error1)
        return
    end
    ServerActivityCtrl.Instance:SendAllSoulsFullReq(1, self.data.task_id)
end
