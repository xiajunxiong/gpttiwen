-- ========================== 辞牛迎新虎 ======================
TigerNewCowOldView = TigerNewCowOldView or DeclareView("TigerNewCowOldView", "serveractivity/tiger_year/tiger_new_cow_old_view", Mod.TigerYear.TigerNewCowOld)
VIEW_DECLARE_LEVEL(TigerNewCowOldView, ViewLevel.L1)
function TigerNewCowOldView:TigerNewCowOldView()
    self.title_data = {
        [0] = "_LocChangShouHu",
        [1] = "_LocFuGuiHu",
        [2] = "_LocKangNingHu",
        [3] = "_LocCiXiaoHu",
        [4] = "_LocHaoDeHu",
    }
end

function TigerNewCowOldView:LoadCallback()
    self:ShowMian()
end

function TigerNewCowOldView:ShowMian()
    self.MainPanel:SetObjActive(true)
    self.DetailPanel:SetObjActive(false)
    UH.SpriteName(self.TitleImg, "_LocCiNiuYingXinHu")
end

function TigerNewCowOldView:OnCloseClick()
    if self.DetailPanel.gameObject.activeSelf then
        self:ShowMian()
        return
    end
    ViewMgr:CloseView(TigerNewCowOldView)
end

function TigerNewCowOldView:OnSelPanelClick(data)
    if TableIsEmpty(data) then
        return
    end
    local state_data = TigerNewCowOldData.Instance:GetTigerStateData(data.tiger_type)
    if state_data.flag == -1 then
        local t_info = TimeManager.FormatDHMS(state_data.time - TimeCtrl.Instance:GetServerTime())
        if t_info.day > 0 then
            PublicPopupCtrl.Instance:Error(string.format(Language.TigerNew.Tip1, t_info.day, t_info.hour))
        else
            PublicPopupCtrl.Instance:Error(string.format(Language.TigerNew.Tip7, t_info.hour, t_info.min))
        end
        return
    end
    self.MainPanel:SetObjActive(false)
    self.DetailPanel:SetObjActive(true)
    self.DetailPanel:SetTigerType(data.tiger_type)
    UH.SpriteName(self.TitleImg, self.title_data[data.tiger_type])
end

function TigerNewCowOldView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[222].desc)
end

TigerNewCowOldPanel = TigerNewCowOldPanel or DeclareMono("TigerNewCowOldPanel", UIWFlushPanel)
function TigerNewCowOldPanel:TigerNewCowOldPanel()
    self.data = TigerNewCowOldData.Instance
    self.act_type = ACTIVITY_TYPE.RABD_TIGER_NEW_COW_OLD
    self.act_info = self.data.act_info
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
    }
end

function TigerNewCowOldPanel:Awake()
    UIWFlushPanel.Awake(self)
    local list_data = self.data:GetGiftGroup()
    self.List:SetData(list_data)
    self.Time:CreateActTime(self.act_type,TimeType.Type_Special_4,Language.TigerNew.ActTime)
    self:FlushPanel()
end

function TigerNewCowOldPanel:FlushPanel()
    UH.SetText(self.BtnText, Language.TigerNew.Text3)
    for i = 1, self.Clips:Length() do
        local mono = self.Clips[i]
        mono:SetData({tiger_type = i - 1})
    end
    self.RedPoint:SetNum(self.act_info.awaken_reward_time > 0 and 1 or 0)
    self.BtnEffect:SetActive(self.act_info.awaken_reward_time > 0)
end

function TigerNewCowOldPanel:OnGetRewardClick()
    if self.act_info.awaken_reward_time <= 0 then
        PublicPopupCtrl.Instance:Error(Language.TigerNew.Tip2)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TigerNewCowOldCtrl.ReqType.GetAwakenReward)
end

TigerNewCowOldCellItem = TigerNewCowOldCellItem or DeclareMono("TigerNewCowOldCellItem", UIWidgetBaseItem)
function TigerNewCowOldCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.Name, string.format(Language.TigerNew.Text4,data.show_rate * 100))
end

TigerNewCowOldClipItem = TigerNewCowOldClipItem or DeclareMono("TigerNewCowOldClipItem", UIWidgetBaseItem)
function TigerNewCowOldClipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local tiger_type = data.tiger_type
    UH.SetText(self.Name, Language.TigerNew.Tiger2[tiger_type])
    local state_data = TigerNewCowOldData.Instance:GetTigerStateData(tiger_type)
    local flag = state_data.flag
    self.Mask:SetActive(flag == -1)
    self.Effect:SetActive(flag == 1)
    self.States[1].gameObject.transform.parent.gameObject:SetActive(flag == 1)
    self.States[2].gameObject.transform.parent.gameObject:SetActive(flag == 0)
    self.States[3].gameObject.transform.parent.gameObject:SetActive(flag == -1)
    self.States[3]:CloseCountDownTime()
    local remind_num = 0
    if flag == 1 then
        UH.SetText(self.States[1], Language.TigerNew.Text5)
        remind_num = TigerNewCowOldData.Instance:GetTigerRemind(tiger_type)
    elseif flag == 0 then
        local str = string.format(Language.TigerNew.Text6,state_data.progress_num,state_data.progress_max_num)
        UH.SetText(self.States[2], str)
        remind_num = TigerNewCowOldData.Instance:GetTigerRemind(tiger_type)
        if state_data.progress_max_num == state_data.progress_num then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RABD_TIGER_NEW_COW_OLD,TigerNewCowOldCtrl.ReqType.Awaken, tiger_type)
        end
    elseif flag == -1 then
        local time_mono = self.States[3]
        time_mono:StampTime(state_data.time,TimeType.Type_Special_4,Language.TigerNew.Text7)
        time_mono:SetCallBack(function ()
            self:SetData(self.data)
        end)
    end
    self.RedPoint:SetNum(remind_num)
end


TigerNewCowOldDetails = TigerNewCowOldDetails or DeclareMono("TigerNewCowOldDetails", UIWFlushPanel)
function TigerNewCowOldDetails:TigerNewCowOldDetails()
    self.data = TigerNewCowOldData.Instance
    self.act_type = ACTIVITY_TYPE.RABD_TIGER_NEW_COW_OLD
    self.stuff_item_id = self.data:GetActItemId()
    self.tiger_type = 0
    self.stuff_item_id = self.data:GetActItemId()
    self.act_info = self.data.act_info
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
    }
end

function TigerNewCowOldDetails:SetTigerType(tiger_type)
    self.tiger_type = tiger_type
    for i = 1, self.Tigers:Length() do
        self.Tigers[i]:SetActive(tiger_type + 1 == i)
    end
    self:FlushPanel()
end

function TigerNewCowOldDetails:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(self.act_type,TimeType.Type_Special_4,Language.TigerNew.ActTime)
    UH.SetIcon(self.StuffIcon, Item.GetIconId(self.stuff_item_id))
end

function TigerNewCowOldDetails:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function TigerNewCowOldDetails:FlushPanel()
    self:FlushRewardList()
    self:FlushItem()
end

function TigerNewCowOldDetails:FlushRewardList()
    local list_data = self.data:GetGiftCfg(self.tiger_type)
    local end_data = list_data[#list_data]
    local tip = string.format(Language.TigerNew.Text1, end_data.target_desc, Language.TigerNew.Tiger[self.tiger_type])
    UH.SetText(self.Tip, tip)
    self.List:SetData(list_data)
end

function TigerNewCowOldDetails:FlushItem()
    local num = Item.GetNum(self.stuff_item_id)
    UH.SetText(self.StuffNum, num)
    local state_data = self.data:GetTigerStateData(self.tiger_type)
    self.RedPoint:SetNum((num > 0 and state_data.progress_max_num ~= state_data.progress_num) and 1 or 0)
end

function TigerNewCowOldDetails:OnAddTigerValue()
    if self.is_playing then
        return
    end
    local state_data = self.data:GetTigerStateData(self.tiger_type)
    self.state_data = state_data
    if state_data.flag == 1 then
        PublicPopupCtrl.Instance:Error(Language.TigerNew.Tip3)
        return
    end
    if state_data.flag == 0 and state_data.progress_num == state_data.progress_max_num then
        PublicPopupCtrl.Instance:Error(Language.TigerNew.Tip8)
        return
    end
    local num = Item.GetNum(self.stuff_item_id)
    if num <= 0 then
        PublicPopupCtrl.Instance:Error(Language.TigerNew.Tip4)
        MainOtherCtrl.Instance:GetWayViewById(self.stuff_item_id)
        return
    end
    local seq = self.tiger_type
    ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TigerNewCowOldCtrl.ReqType.AddValue,seq)
    --PublicPopupCtrl.Instance:Center(string.format(Language.TigerNew.Tip6, Config.ciniuyingxinhu_auto.other[1].add_tiger_air))
    self.EffectTool:Play("6165140")
    self.is_playing = true
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.is_playing = false
    end, 0.5)
end

function TigerNewCowOldDetails:OnIconClick()
    local info = {
        item_list = {Item.Create({item_id = self.stuff_item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
    local pos = UH.GetRootPosition(self.StuffIcon.gameObject)
    ItemInfoData.Instance:SetClickDataPos(pos)
end

function TigerNewCowOldDetails:OnGetReawrdClick(data)
    local flag = TigerNewCowOldData.Instance:GetGiftFlag(data)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.TigerNew.Tip5)
    elseif flag == 1 then
        ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TigerNewCowOldCtrl.ReqType.GetReward, self.tiger_type, data.seq)
    end
end

TigerNewCowOldDetailsReward = TigerNewCowOldDetailsReward or DeclareMono("TigerNewCowOldDetailsReward", UIWidgetBaseItem)
function TigerNewCowOldDetailsReward:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_list = DataHelper.FormatItemList(data.reward_item)
    self.List:SetData(item_list)
    local need_value = data.parameter
    local tiger_type = data.module_type
    local info = TigerNewCowOldData.Instance.act_info.awaken_param[tiger_type]
    local has_value = info.tiger_value > need_value and need_value or info.tiger_value
    local progress_str = string.format("%s/%s", has_value, need_value)
    if has_value == need_value then
        progress_str = ColorStr(progress_str, COLORSTR.Green10)
    end
    local desc = string.format(Language.TigerNew.Text2, data.target_desc , progress_str)
    UH.SetText(self.Desc, desc)
    local flag = TigerNewCowOldData.Instance:GetGiftFlag(data)
    self.flag = flag
    
    self.GetBtn:SetActive(flag >= 0)
    self.GetFlag:SetActive(flag == -1)
    self.RedPoint:SetNum(flag == 1 and 1 or 0)
    self.Interactor.Interactable = flag == 1
    self.BtnEffect:SetActive(flag == 1)
end