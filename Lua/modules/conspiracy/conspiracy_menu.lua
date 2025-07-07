ConspiracySub = ConspiracySub or DeclareView("ConspiracySub", "conspiracy/conspiracy_sub")
 
function ConspiracySub:OnEnable()
    self.TeamBtn:SetActive(TeamData.Instance:InTeam())
    local scene_id = SceneData.Instance:SceneId()
    local info = ConspiracyData.Instance:SceneInfo(scene_id)
    if info == nil then
        return
    end
    UH.SetText(self.Name, info.name)
end
function ConspiracySub:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end
ConspiracyMenu = ConspiracyMenu or DeclareMono("ConspiracyMenu", UIWFlushPanel)
function ConspiracyMenu:ConspiracyMenu()
    self.data = ConspiracyData.Instance
    self.lang = Language.Conspiracy
    self.data_cares = {
        {data = self.data.pass_info, func = self.FlushMenu, init_call = false},
        {data = self.data.fetch_flag,func = self.FlushMenu, init_call = false},
    }
    self.time = 0
end
function ConspiracyMenu:onFlush()
end
function ConspiracyMenu:FlushMenu()
    local scene_id = SceneData.Instance:SceneId()
    local info = self.data:SceneInfo(scene_id)
    if info == nil then
        return
    end
    UH.SetText(self.Title, info.name)
    --进度
    local count, total = self.data:PassCount(info.index)
    UH.SetText(self.Count, string.format(self.lang.KillCount, count, total))
    local data = self.data:BossListByIndex(info.index)
    self.List:SetData(data)
    self.RedPoint:SetNum(self.data:PassAndNotGet(info.index))
end
function ConspiracyMenu:UpdataTime()
    self.time = self.time + 1
    UH.SetText(self.TimeText, string.format(self.lang.CostTime, os.date("%M:%S", self.time)))
end
function ConspiracyMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:CancelTimer(self.ht)
    self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdataTime, self), 1)
    self:FlushMenu()
end
function ConspiracyMenu:OnClickCheck()
    ConspiracyData.Instance:SetSceneID(SceneData.Instance:SceneId())
    ViewMgr:OpenView(ConspiracyWeekFirstKill)
end

function ConspiracyMenu:OnClickOut()
    --判断队长
    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then
        local info = {
            content = ErrorText[128],
            confirm = {
                func = function()
                    TeamCtrl.Instance:SendExitTeam()
                    TimeHelper:CancelTimer(self.ht)
                    self.time = 0
                    SceneCtrl.Instance:RequestLeaveFb()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    TimeHelper:CancelTimer(self.ht)
    self.time = 0
    SceneCtrl.Instance:RequestLeaveFb()
    -- MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

ConspiracyMenuItem = ConspiracyMenuItem or DeclareMono("ConspiracyMenuItem", UIWidgetBaseItem)
function ConspiracyMenuItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    local pass = ConspiracyData.Instance:WeekPass(data.seq + 1, data.index)
    UH.SetText(self.Desc, Language.Conspiracy.KillDesc[pass and 1 or 2])
    UH.Color(self.Desc, pass and COLORS.Green10 or COLORS.Red3)
    local alive = ConspiracyData.Instance:KillFlag(data.index)
    self.JiSha:SetActive(not alive)
    local cfg = NpcHelper.NpcCo(data.npc_id)
    UH.SetIcon(self.Icon, cfg.npc_icon)
    local monster = Monster.New(Cfg.MonsterById(data.boss_monster_id))
    self.Ele:SetData(monster:EleAttr(), false, monster:IsChaos())
end
function ConspiracyMenuItem:OnClickBoss()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    SceneLogic.Instance:AutoToNpc(CommonStruct.MonsterVo(self.data), nil, true)
end
ConspiracyEffect = ConspiracyEffect or DeclareView("ConspiracyEffect", "conspiracy/conspiracy_effect")
function ConspiracyEffect:ConspiracyEffect()
    self.data = ConspiracyData.Instance
    self.effect_list = {
        2161046,
        2161045,
        2161044,
    }
end
function ConspiracyEffect:LoadCallback()
    local result = self.data:BattleResult()
    -- LogDG("客户端计算结果", result)
    if result == 0 then
        ViewMgr:CloseView(ConspiracyEffect)
        local info = self.data:GetFinishReward()
        --如果超过次数就不显示翻牌
        local num = self.data:GetOffList()[1].time - self.data:JoinTimes()
        if num > 0 then
            PokerCtrl.Instance:OnOpenPokerView(info.mypoker, POKER_REQ_OPER_TYPE.TYPE_MI_YI)
        else
           --  PublicPopupCtrl.Instance:Center(ErrorText[183])
        end
        return
    end
    self.EffectTool:Play(self.effect_list[result])
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(ConspiracyEffect)
        local info = self.data:GetFinishReward()
        PokerCtrl.Instance:OnOpenPokerView(info.mypoker, POKER_REQ_OPER_TYPE.TYPE_MI_YI)
    end, 1.3)
end
function ConspiracyEffect:CloseCallback()
    TimeHelper:CancelTimer(self.handle_delay)
end