RegressRewardView = RegressRewardView or DeclareView("RegressRewardView", "serveractivity/regress_reward_view", Mod.RandActivity.RegressReward)
VIEW_DECLARE_LEVEL(RegressRewardView, ViewLevel.L0)
function RegressRewardView:OnClickClose()
    ViewMgr:CloseView(RegressRewardView)
end

function RegressRewardView:CloseCallback()
    ActivityData.Instance:GetActivityEvent():Uncare(self.ht)
end
RegressRewardPanel = RegressRewardPanel or DeclareMono("RegressRewardPanel", UIWFlushPanel)
function RegressRewardPanel:RegressRewardPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.regress_reward_data, func = self.FulshTaskList, init_call = true},
        {data = self.data.regress_reward_data, func = self.FulshGoodsList, init_call = true},
        {data = self.data.regress_reward_data, func = self.FulshBtnList, keys = {"select_val"}, init_call = true}
    }
    local cur_open_day = self.data:GetRegressDay()
    self.data:SetRegressRewardSelectType(cur_open_day)
    self.ui_obj = UIObjDrawer.New()
    self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(322))
    ---延迟0.1秒
    TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayAnim, self),0.15)
end

function RegressRewardPanel:PlayAnim()  
  self.ShowAnim:Play()
end
function RegressRewardPanel:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    if self.btn_effect then
        self.EffectTool:Stop(self.btn_effect)
        self.btn_effect = nil
    end
    UIWFlushPanelGroup.OnDestroy(self)
end
function RegressRewardPanel:FulshGoodsList()
    local list = self.data.regress_reward_data_cfg[self.data.regress_reward_data.day].reward_item
    for i,v in ipairs(DataHelper.FormatList(list)) do
        v.index = i
    end
    self.GoodsList:SetData(list)
    if self.data.regress_reward_data.select_data[self.data.regress_reward_data.day] == 1 then 
        self.BtnInter.Interactable = true
        if self.EffectTool.isActiveAndEnabled then
            if self.btn_effect == nil then
                 self.btn_effect = self.EffectTool:Play(5161004)
            end
        end
    else
        if self.btn_effect then
            self.EffectTool:Stop(self.btn_effect)
            self.btn_effect = nil
        end
        self.BtnInter.Interactable = false
    end
end
function RegressRewardPanel:FulshBtnList()
    self.BtnList:SetDataList(self.data.regress_reward_data_cfg,self.data.regress_reward_data.day)
end
function RegressRewardPanel:FulshTaskList()
    local list = self.data:GetRegressTaskDataByDay(self.data.regress_reward_data.day)
    for i,v in ipairs(list) do
        v.index = i
    end
    self.TaskList:SetData(list)
end
function RegressRewardPanel:OnClickGift()
    ServerActivityCtrl.Instance:SendBackRewardReq(ServeractivityConfig.BackRewardReqType.TYPE_FETCH_DAY, self.data.regress_reward_data.day)
end


RegressTaskItem = RegressTaskItem or DeclareMono("RegressTaskItem", UIWidgetBaseItem)

function RegressTaskItem:SetData(data)
    self.data = data
    UH.RenameGame(self,Format("TaskItem%s",data.index))
    data.item = Item.New(data.reward_item)
    self.Call:SetData(data.item)
    self.cur_open_day = ServerActivityData.Instance:GetRegressDay()
    for i = 1,3 do 
         self.BtnState[i]:SetActive(data.state == i)
    end
    if data.state == 2 and self.data.activity_days <= self.cur_open_day then --2领取  0未完成 1已经领取
         self.BtnInter.Interactable = true
    else
         self.BtnInter.Interactable = false
    end
    UH.SetText(self.BtnName,Language.RegressReward.TaskState[data.state])
    UH.SetText(self.Desc,string.format(Language.RegressReward.TaskDesc,data.task_desc,ServeractivityConfig.RegressReward.Color[data.state],data.times,data.param_1))
    UIWidgetBaseItem.SetData(self, data)
end
function RegressTaskItem:OnClickItem()
     --if self.cur_open_day < self.data.activity_days then
        --PublicPopupCtrl.Instance:Center(string.format(Language.RegressReward.Tips,))
    -- else
     ServerActivityCtrl.Instance:SendBackRewardReq(ServeractivityConfig.BackRewardReqType.TYPE_FETCH_TASK,self.data.task_id)
end
function RegressTaskItem:OnClickTips()
    if self.cur_open_day < self.data.activity_days then
         PublicPopupCtrl.Instance:Center(string.format(Language.RegressReward.Tips2,self.data.activity_days - self.cur_open_day))
    end
end

RegressDayItem = RegressDayItem or DeclareMono("RegressDayItem", UIWidgetBaseItem)

function RegressDayItem:SetData(data)
    UH.RenameGame(self,Format("Item%s",data.activity_days))
    self.data = data
    UH.LocalPos(self.ItemPos,ServeractivityConfig.RegressReward.ItemPos[data.activity_days])
    UH.SetText(self.Day,string.format(Language.RegressReward.Day,data.activity_days))
    UIWidgetBaseItem.SetData(self, data)
    self:SetRedNum()
    self.cur_open_day = ServerActivityData.Instance:GetRegressDay()
    --[[if self.cur_open_day < self.data.activity_days then
        self.Lock:SetActive(true)
    else
        self.Lock:SetActive(false)
    end]]
end
function RegressDayItem:SetRedNum()
    local num = ServerActivityData.Instance:GetRegressRewardReminByDay(self.data.activity_days)
    ServerActivityData.Instance:AddRegressRewardRedNum(num)
    self.RedPos:SetNum(num)
end
function RegressDayItem:OnClickItem()
    -- if self.cur_open_day < self.data.activity_days then
     --   PublicPopupCtrl.Instance:Center(Language.RegressReward.Tips)
     --else
        ServerActivityData.Instance:SetRegressRewardSelectType(self.data.activity_days)
     --end
end
function RegressDayItem:Click()
    --if self.cur_open_day < self.data.activity_days then return end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

RegressGoodsItem = RegressGoodsItem or DeclareMono("RegressGoodsItem", UIWidgetBaseItem)

function RegressGoodsItem:SetData(data)
    self.data = data
    UH.RenameGame(self,Format("GiftItem%s",data.index))
    data.item = Item.New(data)
    self.Call:SetData(data.item)
    UIWidgetBaseItem.SetData(self, data)
end