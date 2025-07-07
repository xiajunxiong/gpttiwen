ArenaAchievenmentView = ArenaAchievenmentView or DeclareView("ArenaAchievenmentView","arena/arena_achievement")
function ArenaAchievenmentView:ArenaAchievenmentView()
    self.Panel:SetView(self:GetType())
    self.data = ArenaData.Instance
    self.ht = self.data.achievement_data:Care(BindTool.Bind(self.FlushPanel,self))
end

function ArenaAchievenmentView:LoadCallback(param_t)
    ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_ACHIEVEMENT,0)
end

function ArenaAchievenmentView:FlushPanel()
    local list_data = self.data:GetAchievementData()
    self.Panel:SetPanel(list_data , self.data.achievement_data.next_reset_time , BindTool.Bind(self.GetFunc,self),
    Language.ArenaAchieve.TimeDesc)
end

function ArenaAchievenmentView:CloseCallback()
    self.data.achievement_data:Uncare(self.ht)
end

function ArenaAchievenmentView:GetFunc(data)
    if data.flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.ArenaAchieve.GetTip2)
    elseif data.flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.ArenaAchieve.GetTip1)
    elseif data.flag == 1 then
        ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_ACHIEVEMENT,data.sn)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
    end
end

ArenaAchievenmentPanel = ArenaAchievenmentPanel or DeclareMono("ArenaAchievenmentPanel",UIWFlushPanel)
function ArenaAchievenmentPanel:ArenaAchievenmentPanel()
    self.data = ArenaData.Instance
end
function ArenaAchievenmentPanel:Awake()
    UIWFlushPanel.Awake(self)
end

-- @params : list_data:{} , next_reset_time:number , get_func:function, time_desc:string
function ArenaAchievenmentPanel:SetPanel(list_data,next_reset_time,get_func,time_desc)
    self:SetListData(list_data)
    self.TimeMeter:StampTime(next_reset_time,TimeType.Type_Time_6)
    self.get_func = get_func
    self:SetTimeDesc(time_desc)
end

function ArenaAchievenmentPanel:SetListData(list_data)
    self.List:SetData(list_data)
end

function ArenaAchievenmentPanel:SetTimeDesc(time_desc)
    time_desc = time_desc or Language.ArenaAchieve.TimeDefaultDesc
    UH.SetText(self.TimeDesc,time_desc)
end

function ArenaAchievenmentPanel:OnDestroy()
    self.TimeMeter:CloseCountDownTime()
    UIWFlushPanel.OnDestroy(self)
end

function ArenaAchievenmentPanel:OnGetClick(data)
    if self.get_func then
        self.get_func(data)
    end
end

function ArenaAchievenmentPanel:SetView(view)
    self.view = view
end

function ArenaAchievenmentPanel:OnCloseClick()
    if self.view then
        ViewMgr:CloseView(self.view)
    end
end

-- @data : 在配置表的 data 上额外自添加 flag, progress_num 字段
ArenaAchievenmentItem = ArenaAchievenmentItem or DeclareMono("ArenaAchievenmentItem",UIWidgetBaseItem)
function ArenaAchievenmentItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Title,data.ach_name)
    UH.SetText(self.Desc,data.ach_dsc)
    local reward_list = {}
    if data.price_num > 0 then
        table.insert(reward_list,Item.Create({item_id = HuoBi[data.price_type],num = data.price_num}))
    end
    local item_list = DataHelper.FormatItemList(data.item_list)
    AndList(reward_list,item_list)
    self.RewardList:SetData(reward_list)
    self.Interactor.Interactable = data.flag == 1
    local btn_text = ""
    if data.flag == 1 then
        btn_text = Language.ArenaAchieve.State2
    elseif data.flag == -1 then
        btn_text = Language.ArenaAchieve.State3
    else
        btn_text = Language.ArenaAchieve.State1
    end
    UH.SetText(self.GetBtnText,btn_text)
    self.RedPoint:SetNum(data.flag == 1 and 1 or 0)
    self:ShowProgress(self.data.progress_way)
end

function ArenaAchievenmentItem:ShowProgress(type)
    local progress_num = 0
    local progress_max_num = 0
    local param = self.data.ach_param1 or self.data.param
    if type == 1 then
        progress_num = self.data.progress_num > param and param or self.data.progress_num
        progress_max_num = param
    else
        progress_num = self.data.flag == 0 and 0 or 1
        progress_max_num = 1
    end

    local progress_value = progress_num / progress_max_num
    self.Progress:SetProgress(progress_value)
    self.Progress:SetText(string.format("%s/%s",progress_num,progress_max_num))
end