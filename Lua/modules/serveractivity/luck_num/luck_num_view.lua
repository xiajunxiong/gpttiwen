-- 幸运数字
LuckNumView = LuckNumView or DeclareView("LuckNumView", "serveractivity/act_luck_num_view", Mod.Act_Halloween.LuckNum)
function LuckNumView:OnCloseClick()
    ViewMgr:CloseView(LuckNumView)
end

function LuckNumView:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(395)
end

LuckNumPanel = LuckNumPanel or DeclareMono("LuckNumPanel", UIWFlushPanel)
function LuckNumPanel:LuckNumPanel()
    self.data = LuckNumData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false}
    }
    self.task_list = {
        {seq = 0},
        {seq = 1},
        {seq = 2},
    }
    self.act_type = self.data.act_type
end

function LuckNumPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(self.act_type,TimeType.Type_Time_8,Language.LuckNum.Text2)
    UH.SetText(self.Tip1, Language.LuckNum.Tip1)
    UH.SetText(self.Tip2, Language.LuckNum.Tip2)

    self:FlushPanel()
end

function LuckNumPanel:FlushPanel()
    self.List:SetData(self.task_list)

    local is_luck = self.data:IsLuck()
    local reward_state = self.data:GetRewaredState()
    self.LuckGetFlag:SetActive(is_luck and reward_state == -1)
    self.NormalGetFlag:SetActive(not is_luck and reward_state == -1)
    self.LuckGetClick:SetActive(is_luck and reward_state == 1)
    self.NormalGetClick:SetActive(not is_luck and reward_state == 1)

    local luck_num = self.data:GetLuckNum()
    self.NotNum:SetActive(luck_num < 0)
    if luck_num < 0 then
        self.LuckNumList:SetData({})
    else
        local nums = NumToTable(luck_num)
        local list_data = {}
        for i,v in pairs(nums) do
            table.insert(list_data, {num = v})
        end
        self.LuckNumList:SetData(list_data)
    end

    local reward_cfg = self.data:GetRewardPrevewCfg()
    local l_reward_list = {}    -- 幸运奖
    local r_reward_list = {}    -- 参与奖
    for i,v in pairs(reward_cfg) do
        table.insert(l_reward_list,{
            item = v.show_lucky_reward_group,
            is_rate = v.is_get_rate_1 == 1,
            effect = is_luck and reward_state == 1,
        })
        table.insert(r_reward_list,{
            item = v.show_join_reward_group,
            is_rate = v.is_get_rate_2 == 1,
            effect = not is_luck and reward_state == 1,
        })
    end
    self.LuckRewardList:SetData(l_reward_list)
    self.NormalRewardList:SetData(r_reward_list)
end

function LuckNumPanel:OnRewardClick()
    ServerActivityCtrl.Instance:SendActivityReq(self.act_type, LuckNumCtrl.ReqType.GetReward)
end


LuckNumTaskRewardChildItem = LuckNumTaskRewardChildItem or DeclareMono("LuckNumTaskRewardChildItem", UIWidgetBaseItem)
function LuckNumTaskRewardChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.item)
    self.Item:SetData(item)
    self.Rate:SetActive(data.is_rate)
    self.Effect:SetActive(data.effect == true)
end

LuckNumTaskRewardNumItem = LuckNumTaskRewardNumItem or DeclareMono("LuckNumTaskRewardNumItem", UIWidgetBaseItem)
function LuckNumTaskRewardNumItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local num = data.num
    UH.SpriteName(self.NumImg, num)
    local child_count = self.NumEffect.childCount
    for i = 0, child_count - 1 do
        local obj = self.NumEffect:GetChild(i).gameObject
        obj:SetActive(i == num)
    end
end


LuckNumTaskItem = LuckNumTaskItem or DeclareMono("LuckNumTaskItem", UIWidgetBaseItem)
function LuckNumTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local seq = data.seq
    local index = seq + 1
    local index_str = DataHelper.GetDaXie(index)
    UH.SetText(self.Title, Language.LuckNum.Text1 .. index_str)
    UH.SetText(self.Task, Language.LuckNum.Tasks[seq])

    local task_info = LuckNumData.Instance:GetTaskInfo(seq)
    if TableIsEmpty(task_info) then
        task_info = {seq = seq, luck_num = -1, is_finish = 0}
    end
    local is_active = task_info.luck_num >= 0
    local bg_img = is_active and "TangGuoKai" or "TangGuoWeiKai"
    UH.SpriteName(self.BG, bg_img)
    self.Img:SetActive(not is_active)
    self.Num:SetObjActive(is_active)
    if is_active then
        self.Num.text = task_info.luck_num
    end
    local btn_state = LuckNumData.Instance:GetTaskBtnState(seq)
    self.Btn1:SetActive(btn_state == 1)
    self.Btn2:SetActive(btn_state == 2)
    self.Btn3:SetActive(btn_state == 3)
    self.btn_state = btn_state
    self.RedPoint:SetNum(btn_state == 1 and 1 or 0)
end

function LuckNumTaskItem:OnBtnClick()
    local seq = self.data.seq
    local btn_state = LuckNumData.Instance:GetTaskBtnState(seq)
    local act_type = LuckNumData.Instance.act_type
    if btn_state == 1 then
        ServerActivityCtrl.Instance:SendActivityReq(act_type, LuckNumCtrl.ReqType.GetLuckNum, seq)
    elseif btn_state == 3 then
        if seq == 1 then
            ViewMgr:OpenViewByKey(Mod.Activity.Daily)
        elseif seq == 2 then
            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
        end
    end
end