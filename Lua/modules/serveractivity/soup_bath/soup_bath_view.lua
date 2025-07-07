SoupBathView = SoupBathView or DeclareView("SoupBathView", "serveractivity/soup_bath/soup_bath", Mod.DragonBoat.SoupBath)
VIEW_DECLARE_LEVEL(SoupBathView, ViewLevel.L1)
VIEW_DECLARE_MASK(SoupBathView, ViewMask.BgBlock)
function SoupBathView:SoupBathView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SOUP_BATH, TimeType.Type_Time_3)
end

function SoupBathView:LoadCallback(param_t)
    
end

function SoupBathView:CloseCallback()

end

function SoupBathView:OnCloseClick()
    ViewMgr:CloseView(SoupBathView)
end

function SoupBathView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[327].desc)
end

----------------------------SoupBathPanel----------------------------
SoupBathPanel = SoupBathPanel or DeclareMono("SoupBathPanel", UIWFlushPanel)
function SoupBathPanel:SoupBathPanel()
    self.Data = SoupBathData.Instance
    self.language = Language.SoupBath
    self.data = self.Data.info_data
    self.is_need_delay = false
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:ConsumeItem(), BindTool.Bind(self.FlushAll, self))
    end
end

function SoupBathPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function SoupBathPanel:FlushAll()
    if self.is_need_delay then
        TimeHelper:CancelTimer(self.ht)
        self.ht = Invoke(function()
            self:ShowLastReward()
            self:FlushAll()
            self.is_need_delay = false
        end, 1)
        return
    end
    
    self.ObjCanBath:SetActive(self.data.info.last_reward_flag == 1)
    self.IHandler.Interactable = self.data.info.last_reward_flag ~= 2
    UH.SetText(self.TxtBtn, self.language.Btn[self.data.info.last_reward_flag])
    local left_remind = self.Data:LeftRemind()
    self.Remind:SetNum(left_remind)
    self.BtnEffect:SetActive(left_remind > 0)
    self.TaskList:SetData(self.Data:TaskConfig())
end

function SoupBathPanel:OnBtnClick()
    if self.data.info.last_reward_flag == 0 then
        ViewMgr:OpenView(SoupBathMedicineView)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    elseif self.data.info.last_reward_flag == 1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        SoupBathCtrl.Instance:SendReq(SoupBathReq.Reward)
        self.is_need_delay = true
        BagData.Instance:SetHideCurrencyCenterFlag(true)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end
end

function SoupBathPanel:OnRewardClick()
    local cfg = self.Data:RewardShowCfg()
    local t = {}
    t.item_list = DataHelper.FormatItemList(cfg.show_reward_item)
    ViewMgr:OpenView(ReedIncenseCityShowItemView, t)
end

function SoupBathPanel:ShowLastReward()
    if self.Data:GetLastReward() then
        ViewMgr:OpenView(NoonRewardView, {item_list = self.Data:GetLastReward()})
        self.Data:SetLastReward()
    end
    BagData.Instance:SetHideCurrencyCenterFlag(false)
end

function SoupBathPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
    BagData.Instance:RemoveListenItems(self.item_listen)
    self:ShowLastReward()
end

--------------SoupBathTaskItem--------------
SoupBathTaskItem = SoupBathTaskItem or DeclareMono("SoupBathTaskItem", UIWidgetBaseItem)
function SoupBathTaskItem:SoupBathTaskItem()
    self.Data = SoupBathData.Instance
end

function SoupBathTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_can_receive = self.Data:IsCanReceiveTask(data)
    self.reward_flag = self.Data:RewardFlag(data.seq)
    self.task_param = self.Data:TaskParam(data.seq)
    UH.SetText(self.TxtName, data.task_desc)
    self.ItemList:SetData(DataHelper.FormatItemList(data.reward_item))
    UH.SetText(self.TxtProg, Format(Language.Common.Slash, self.task_param, data.parameter))
    self.Remind:SetNum(self.is_can_receive and 1 or 0)
    self.Effect:SetActive(self.is_can_receive)
    self.ObjReceived:SetActive(self.reward_flag > 0)
    --task_type 9 是在线时间，不需要显示“前往”，条件打成后显示领取即可
    self.BtnObj:SetActive(self.reward_flag <= 0 and (data.task_type ~= 9 or self.task_param >= data.parameter))

    self.IHandler.Interactable = true
    if self.reward_flag == 0 then
        if data.jump_interface == 0 then
            UH.SetText(self.TxtReceive, Language.SoupBath.Btn2[self.task_param >= data.parameter and 2 or 1])
            self.IHandler.Interactable = self.task_param >= data.parameter
        else
            UH.SetText(self.TxtReceive, Language.SoupBath.Btn2[self.task_param >= data.parameter and 2 or 3])
        end
    end
end

function SoupBathTaskItem:OnBtnClick()
    if self.reward_flag == 0 then
        if self.is_can_receive then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            SoupBathCtrl.Instance:SendReq(SoupBathReq.Task, self.data.seq)
        elseif self.data.jump_interface > 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            ViewMgr:OpenViewByKey(self.data.jump_interface)
            ViewMgr:CloseView(ActDragonBoatMainView)
            ViewMgr:CloseView(SoupBathView)
        else
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        end
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end
end

----------------------------SoupBathMedicineView----------------------------
SoupBathMedicineView = SoupBathMedicineView or DeclareView("SoupBathMedicineView", "serveractivity/soup_bath/soup_bath_medicine")
VIEW_DECLARE_MASK(SoupBathMedicineView, ViewMask.BgBlock)
function SoupBathMedicineView:LoadCallback(param_t)
    
end

function SoupBathMedicineView:OnCloseClick()
    ViewMgr:CloseView(SoupBathMedicineView)
end

----------------------------SoupBathMedicinePanel----------------------------
SoupBathMedicinePanel = SoupBathMedicinePanel or DeclareMono("SoupBathMedicinePanel", UIWFlushPanel)
function SoupBathMedicinePanel:SoupBathMedicinePanel()
    self.Data = SoupBathData.Instance
    self.language = Language.SoupBath
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:ConsumeItem(), BindTool.Bind(self.FlushAll, self))
    end
end

function SoupBathMedicinePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function SoupBathMedicinePanel:FlushAll()
    self.has_num = {}
    self.now_put_num = {}
    local list_data = {}
    for k, v in pairs(self.Data:MedicineCfg()) do
        self.has_num[v.seq] = Item.GetNum(v.submit_item_id)
        local need = v.submit_num - self.Data:MedicineNum(v.seq)
        self.now_put_num[v.seq] = math.min(self.has_num[v.seq], need)

        list_data[k] = {}
        list_data[k].info = v
        list_data[k].show_num = self.now_put_num[v.seq]
    end
    self.List:SetData(list_data)
end

function SoupBathMedicinePanel:OnConfirmClick()
    local is_send = false
    for i = 0, 3 do
        if self.now_put_num[i] > 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            SoupBathCtrl.Instance:SendReq(SoupBathReq.Medicine, i, self.now_put_num[i])
            is_send = true
        end
    end
    if not is_send then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        PublicPopupCtrl.Instance:Center(self.language.NumTip[1])
    end
end

function SoupBathMedicinePanel:OnChangeClick(mono, value)
    local data = mono:GetData()
    local seq = data.info.seq
    local can_max_num = data.info.submit_num - self.Data:MedicineNum(seq)
    local name = Item.GetName(data.info.submit_item_id)
    if can_max_num == 0 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        PublicPopupCtrl.Instance:Center(Format(self.language.NumTip[4], name))
        return
    end
    if value == -1 then
        if self.now_put_num[seq] == 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            PublicPopupCtrl.Instance:Center(self.language.NumTip[5])
        else
            self.now_put_num[seq] = self.now_put_num[seq] - 1
            data.show_num = self.now_put_num[seq]
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            mono:FlushNow()
        end
    elseif value == 1 then
        if self.now_put_num[seq] == can_max_num then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            PublicPopupCtrl.Instance:Center(Format(self.language.NumTip[3], name))
        elseif self.now_put_num[seq] == self.has_num[seq] then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            PublicPopupCtrl.Instance:Center(Format(self.language.NumTip[2], name))
        else
            self.now_put_num[seq] = self.now_put_num[seq] + 1
            data.show_num = self.now_put_num[seq]
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            mono:FlushNow()
        end
    end
end

function SoupBathMedicinePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------SoupBathMedicineItem--------------
SoupBathMedicineItem = SoupBathMedicineItem or DeclareMono("SoupBathMedicineItem", UIWidgetBaseItem)
function SoupBathMedicineItem:SoupBathMedicineItem()
    self.Data = SoupBathData.Instance
end

function SoupBathMedicineItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Init(data.info.submit_item_id, 1, 1)
    UH.SetText(self.TxtName, item:QuaName(true))
    local can_max_num = data.info.submit_num - self.Data:MedicineNum(data.info.seq)
    UH.SetText(self.TxtTimes, Format(Language.SoupBath.Times, can_max_num, data.info.submit_num))
    self:FlushNow()
end

function SoupBathMedicineItem:FlushNow()
    UH.SetText(self.TxtNow, self.data.show_num)
    local item = Item.Init(self.data.info.submit_item_id, Item.GetNum(self.data.info.submit_item_id), 1)
    self.Cell:SetData(item)
end