KFHeroRecordView = KFHeroRecordView or DeclareView("KFHeroRecordView", "kf_hero_record/kf_hero_record", Mod.KFHeroRecord.Main)
VIEW_DECLARE_LEVEL(KFHeroRecordView, ViewLevel.L1)
VIEW_DECLARE_MASK(KFHeroRecordView, ViewMask.BgBlock)
function KFHeroRecordView:KFHeroRecordView()

end

function KFHeroRecordView:LoadCallback(param_t)
    
end

function KFHeroRecordView:CloseCallback()

end

function KFHeroRecordView:OnCloseClick()
    ViewMgr:CloseView(KFHeroRecordView)
end

function KFHeroRecordView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[339].desc)
end

----------------------------KFHeroRecordPanel----------------------------
KFHeroRecordPanel = KFHeroRecordPanel or DeclareMono("KFHeroRecordPanel", UIWFlushPanel)
function KFHeroRecordPanel:KFHeroRecordPanel()
    self.Data = KFHeroRecordData.Instance
    self.language = Language.KFHeroRecord
    self.data = self.Data.info_data
    self.now_rank_type = -1
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = self.Data.rank_data, func = self.FlushCenter, init_call = false},
        {data = self.Data.value_data, func = self.FlushCenter, init_call = false},
    }
end

function KFHeroRecordPanel:Awake()
    UIWFlushPanel.Awake(self)
    if not self:ActJudge() then
        return
    end
    self.Data:ClearFirstRemind()
    self.ht = TimeHelper:AddRunTimer(function()
        KFHeroRecordCtrl.Instance:SendRankInfo()
        KFHeroRecordCtrl.Instance:SendReq(KFHeroRecordReqType.GetValue)
    end, 60, -1, true)
    self:FlushAll()
end

function KFHeroRecordPanel:ActJudge()
    if not self.Data:IsOpenTime() then
        PublicPopupCtrl.Instance:Center(self.language.ActEnd)
        ViewMgr:CloseView(KFHeroRecordView)
        return false
    end
    return true
end

function KFHeroRecordPanel:FlushAll()
    if not self:ActJudge() then
        return
    end
    self.LeftList:SetData({})
    self.left_data = self.Data:TimeConfig()
    self.LeftList:SetData(self.left_data)
    if self.now_rank_type == -1 or self.Data:RankOpenState(self.now_rank_type) ~= 1 then
        self.now_rank_type = self.left_data[1].rank_type
    end
    self.LeftList:SetectItem(self:GetIndexByRank(self.now_rank_type), true)
    self.task_data = self.Data:TaskData(self.now_rank_type)
    self.TaskList:SetData(self.task_data)
end

function KFHeroRecordPanel:GetIndexByRank(rank_type)
    for k, v in pairs(self.left_data) do
        if v.rank_type == rank_type then
            return k
        end
    end
    return 1
end

function KFHeroRecordPanel:FlushCenter()
    if self.now_rank_type == -1 then
        return
    end
    self.rank_data, self.my_rank_data = self.Data:BaseData(self.now_rank_type)
    self.RankList:SetData(self.rank_data)
    self.MyRankItem:SetData(self.my_rank_data)
    self.Timer:StampTime(self.Data:StateEndTimes(self.now_rank_type), TimeType.Type_Time_3)
end

function KFHeroRecordPanel:OnLeftClick(data)
    self.now_rank_type = data.rank_type
    self:FlushCenter()
end

function KFHeroRecordPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
end

--------------KFHeroRecordLeftItem--------------
KFHeroRecordLeftItem = KFHeroRecordLeftItem or DeclareMono("KFHeroRecordLeftItem", UIWidgetBaseItem)
function KFHeroRecordLeftItem:KFHeroRecordLeftItem()
    self.Data = KFHeroRecordData.Instance
    self.language = Language.KFHeroRecord
    self.Timer:SetShowCallBack(function (t)
        return Format(self.language.DayTime, tonumber(t.day) + 1)
    end)
    self.Timer2:SetShowCallBack(function (t)
        return Format(self.language.DayTime2, tonumber(t.day) + 1)
    end)
    self.Timer:SetCallBack(function()
        self.Data.info_data:Notify()
    end)
end

function KFHeroRecordLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtTitle, self.language.Title[data.rank_type])
    self.ObjLine:SetActive(data.my_index ~= 1)
    self.state = self.Data:RankOpenState(data.rank_type)
    self.ObjBlock:SetActive(self.state ~= 1)
    self.ObjEnd:SetActive(self.state == -1)
    self.ObjReady:SetActive(self.state == 0)
    self.ObjStart:SetActive(self.state == 1)
    self.Timer:CloseCountDownTime()
    self.Timer2:CloseCountDownTime()
    if self.state == 0 then
        self.start_time = self.Data:StateEndTimes(data.rank_type) - data.continuou_times * TIME_DAY_SECOND_NUM
        self.Timer:StampTime(self.start_time, TimeType.Type_Time_3)
    elseif self.state == 1 then
        self.Timer2:StampTime(self.Data:StateEndTimes(data.rank_type), TimeType.Type_Time_3)
    end
end

function KFHeroRecordLeftItem:OnBlockClick()
    if self.state == -1 then
        PublicPopupCtrl.Instance:Center(self.language.End)
    elseif self.state == 0 then
        local day = TimeCtrl.Instance:GetDataDayNum((self.start_time - 1) or 0, TimeManager.GetServerTime())
        PublicPopupCtrl.Instance:Center(Format(self.language.DayTime, day))
    end
end

--------------KFHeroRecordRankItem--------------
KFHeroRecordRankItem = KFHeroRecordRankItem or DeclareMono("KFHeroRecordRankItem", UIWidgetBaseItem)
function KFHeroRecordRankItem:KFHeroRecordRankItem()
    self.Data = KFHeroRecordData.Instance
    self.language = Language.KFHeroRecord
end

function KFHeroRecordRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank_info = data.rank_info
    local reward_info = data.reward_info
    local rank = rank_info.rank
    if self.ObjLine then
        self.ObjLine:SetActive(rank ~= 1)
    end
    self.ImgRank:SetObjActive(rank <= 3)
    if rank <= 3 then
        UH.SpriteName(self.ImgRank, "Rank" .. rank)
    else
        UH.SetText(self.TxtRank, rank_info.no_rank and self.language.NoRank or rank)
    end
    self.Avatar:SetData(rank_info.role)
    UH.SetText(self.TxtName, rank_info.role.name)
    UH.SetText(self.TxtValue, self.language.Title[reward_info.rank_type] .."："..rank_info.rank_value)
    if not TableIsEmpty(reward_info.reward1_item) then
        self.RewardList:SetData(DataHelper.FormatItemList(reward_info.reward1_item))
        self.RewardList:SetObjActive(true)
    else
        self.RewardList:SetObjActive(false)
    end
end

--------------KFHeroRecordTaskItem--------------
KFHeroRecordTaskItem = KFHeroRecordTaskItem or DeclareMono("KFHeroRecordTaskItem", UIWidgetBaseItem)
function KFHeroRecordTaskItem:KFHeroRecordTaskItem()
    self.Data = KFHeroRecordData.Instance
    self.language = Language.KFHeroRecord
end

function KFHeroRecordTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.task_info = self.Data:GetTaskInfo(data.rank_type, data.seq)
    local param = self.task_info.param > data.parameter2 and data.parameter2 or self.task_info.param
    UH.SetText(self.TxtDesc, Format(self.language.Desc, data.describe, param, data.parameter2))
    local is_get = self.Data:TaskIsGet(data.rank_type, data.seq)
    local can_get = self.Data:TaskIsCanGet(data)
    self.RewardList:SetData(self:GetRewardData(data.reward1_item, can_get, data.rank_type, data.seq))
    self.ObjGeted:SetActive(is_get)
    self.ObjGet:SetActive(can_get)
    self.ObjGoto:SetActive(self.data.open_panel > 0 and (not is_get) and (not can_get))
end

function KFHeroRecordTaskItem:OnGotoClick()
    if self.data.open_panel > 0 then
        ViewMgr:OpenViewByKey(self.data.open_panel)
        ViewMgr:CloseView(KFHeroRecordView)
    end
end

function KFHeroRecordTaskItem:GetRewardData(reward, can_get, rank_type, seq)
    local list = {}
    for k, v in pairs(reward) do
        local t = {}
        t.item = v
        t.can_get = can_get
        t.rank_type = rank_type
        t.seq = seq
        table.insert(list, t)
    end
    return list
end

--------------KFHeroRecordCellItem--------------
KFHeroRecordCellItem = KFHeroRecordCellItem or DeclareMono("KFHeroRecordCellItem", UIWidgetBaseItem)
function KFHeroRecordCellItem:KFHeroRecordCellItem()
    self.Data = KFHeroRecordData.Instance
    self.language = Language.KFHeroRecord
end

function KFHeroRecordCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.item))
    self.ObjEffect:SetActive(data.can_get)
end

function KFHeroRecordCellItem:OnGetClick()
    KFHeroRecordCtrl.Instance:SendReq(KFHeroRecordReqType.GetReward, self.data.rank_type, self.data.seq)
end