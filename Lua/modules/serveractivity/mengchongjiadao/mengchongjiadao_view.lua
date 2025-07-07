MengChongJiaDaoView = MengChongJiaDaoView or DeclareView("MengChongJiaDaoView", "serveractivity/mengchongjiadao/mengchongjiadao", Mod.YunZe.MengChongJiaDao)
VIEW_DECLARE_LEVEL(MengChongJiaDaoView, ViewLevel.L1)

function MengChongJiaDaoView:CloseCallback()
    MengChongJiaDaoData.Instance:SaveWatchId()
    MengChongJiaDaoData.Instance:SetIsBattle(false)
end

function MengChongJiaDaoView:OnClickClose()
    ViewMgr:CloseView(MengChongJiaDaoView)
end

MengChongJiaDaoPanel = MengChongJiaDaoPanel or DeclareMono("MengChongJiaDaoPanel", UIWFlushPanel)
function MengChongJiaDaoPanel:MengChongJiaDaoPanel()
    self.data_cares = {
        {data = MengChongJiaDaoData.Instance:GetActInfo(), func = self.FlushView},
        {data = MengChongJiaDaoData.Instance.battle_info, func = self.FlushPK, init_call = false}
    }
end

function MengChongJiaDaoPanel:Awake()
    UIWFlushPanel.Awake(self)
    local title_str = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO)
    local char_list = DataHelper.GetStringWordToTable(title_str)
    local title1 = char_list[1] or ""
    local title2 = ""
    for i = 2, #char_list do
        title2 = title2 .. char_list[i]
    end
    UH.SetText(self.Title1Text, title1)
    UH.SetText(self.Title2Text, title2)
    self.ActTimer:CreateActTime(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO, TimeType.Type_Time_3, Language.MengChongJiaDao.Txt2)
end

function MengChongJiaDaoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.invoke_timer)
    self.invoke_timer = nil
end

function MengChongJiaDaoPanel:FlushView()
    local show_list = MengChongJiaDaoData.Instance:GetTodayTaskList()
    self.TaskList:SetData(show_list)
    self.TaskScrollRect.enabled = #show_list >= 4

    local day_list = MengChongJiaDaoData.Instance:GetDayList()
    self.DayList:SetData(day_list)
    self.DayScrollRect.enabled = #day_list > 5

    self:FlushPet()
end

function MengChongJiaDaoPanel:FlushPet()
    local monster_cfg = MengChongJiaDaoData.Instance:GetCurMonsterCfg()
    self.RoleItem:SetData(monster_cfg)
    self.EnemyItem:SetData(monster_cfg)

    local is_over = MengChongJiaDaoData.Instance:IsAllDefeat()
    self.BtnPK:SetActive(not is_over)
    self.RewardMask:SetActive(is_over)
    self.RewardNode:SetActive(true)
    local reward_cfg = MengChongJiaDaoData.Instance:GetRewardByMonsterId(monster_cfg.monster)
    self.RewardList:SetData(DataHelper.FormatItemList(reward_cfg.reward_item))
    UH.SetText(self.RewardText, Language.MengChongJiaDao.RewardTips[is_over and 2 or 1])
end

function MengChongJiaDaoPanel:FlushPK()
    --0失败，1成功，2平局
    self:ChanToPK(-1)
    self.RoleItem:SetActive(true)
    self.EnemyItem:SetActive(true)
    self.PetEffect:SetActive(true)
    local pk_result = MengChongJiaDaoData.Instance:GetPKResult()
    if pk_result == 1 then
        self.RoleItem:ShowWin()
        self.EnemyItem:ShowFail()
        self.ResultPing:SetActive(false)
    elseif pk_result == 0 then
        self.RoleItem:ShowFail()
        self.EnemyItem:ShowWin()
        self.ResultPing:SetActive(false)
    elseif pk_result == 2 then
        self.RoleItem:ShowPing()
        self.EnemyItem:ShowPing()
        self.ResultPing:SetActive(true)
    end
    if self.invoke_timer ~= nil then
        TimeHelper:CancelTimer(self.invoke_timer)
    end
    self.invoke_timer = Invoke(BindTool.Bind1(self.ToNomal, self), 3)
end

function MengChongJiaDaoPanel:ToNomal()
    self.invoke_timer = nil
    self.RoleItem:ShowIdle()
    self.EnemyItem:ShowIdle()
    self.ResultPing:SetActive(false)
    self:FlushPet()
end

function MengChongJiaDaoPanel:OnClickPK()
    if MengChongJiaDaoData.Instance:IsBattle() then
        return
    end
    AudioMgr:PlayEffect(AudioType.Battle, CommonAudio.XuanZhuanFeiFuShouJi)
    MengChongJiaDaoData.Instance:SetIsBattle(true)
    self.RoleItem:SetActive(false)
    self.EnemyItem:SetActive(false)
    self.PetEffect:SetActive(false)
    self.BtnPK:SetActive(false)
    self.RewardNode:SetActive(false)

    local monster_cfg = MengChongJiaDaoData.Instance:GetCurMonsterCfg()
    self:ChanToPK(monster_cfg.model)
    if self.invoke_timer ~= nil then
        TimeHelper:CancelTimer(self.invoke_timer)
    end
    self.invoke_timer = Invoke(BindTool.Bind1(self.OnPKFinish, self), 2)
end

function MengChongJiaDaoPanel:ChanToPK(index)
    for i = 1, 3 do
        self.PKAnimList[i].gameObject:SetActive(i == index)
    end
end

function MengChongJiaDaoPanel:OnPKFinish()
    self.invoke_timer = nil
    local monster_id = MengChongJiaDaoData.Instance:GetCurMonsterId()
    MengChongJiaDaoCtrl.Instance:SendFetchRewardReq(monster_id)
    -- local protocol = {is_challenge_status = 1, challenge_status = bit:And({511})}
    -- MengChongJiaDaoCtrl.Instance:OnSCRAYunZeCutePetChallengeInfo(protocol)
end

MengChongJiaDaoTaskItem = MengChongJiaDaoTaskItem or DeclareMono("MengChongJiaDaoTaskItem", UIWidgetBaseItem)
function MengChongJiaDaoTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local task_prog = MengChongJiaDaoData.Instance:GetTaskProg(data.type)
    UH.SetText(self.DescText, data.describe .. Format("\n(<color=#%s>%d</color>/%d)", COLORSTR.Green9, task_prog, data.parameter))
    UH.SetText(self.ValueText, data.combat)

    local task_status = MengChongJiaDaoData.Instance:GetTaskStatus(data.seq1)
    self.OverObj:SetActive(task_status == 1)
    self.BtnGo:SetActive(task_status ~= 1 and data.type ~= MengChongJiaDaoTaskType.TYPE_LOGIN)
end

function MengChongJiaDaoTaskItem:OnClickGo()
    ViewMgr:OpenViewByKey(self.data.mod_key)
    ViewMgr:CloseView(MengChongJiaDaoView)
end

MengChongJiaDaoDayItem = MengChongJiaDaoDayItem or DeclareMono("MengChongJiaDaoDayItem", UIWidgetBaseItem)
function MengChongJiaDaoDayItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local day_val = MengChongJiaDaoData.Instance:GetDayCombat(data.day)
    local desc_str = Format(Language.MengChongJiaDao.DayVal, DataHelper.GetWord(data.day), day_val)
    UH.SetText(self.DescText, desc_str)
end
