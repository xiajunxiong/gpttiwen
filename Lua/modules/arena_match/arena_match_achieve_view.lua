ArenaMatchAchieveView = ArenaMatchAchieveView or DeclareView("ArenaMatchAchieveView","arena_match/arena_match_achievement")
function ArenaMatchAchieveView:ArenaMatchAchieveView()
    self.Panel:SetView(self:GetType())
    self.data = ArenaMatchData.Instance
    self.ht = self.data.achievement_data:Care(BindTool.Bind(self.FlushPanel,self))
end

function ArenaMatchAchieveView:LoadCallback(param_t)
    -- ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_ACHIEVEMENT,0)
    self:FlushPanel()
end

function ArenaMatchAchieveView:FlushPanel()
    local list_data = self.data:GetAchievementData()
    local end_time = TimeCtrl.Instance:GetServerTime() + self.data:GetDayEndTime()
    self.Panel:SetPanel(list_data , end_time , BindTool.Bind(self.GetFunc,self),
    Language.ArenaAchieve.TimeDesc2)
end

function ArenaMatchAchieveView:CloseCallback()
    self.data.achievement_data:Uncare(self.ht)
end

function ArenaMatchAchieveView:GetFunc(data)
    if data.flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.ArenaAchieve.GetTip2)
    elseif data.flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.ArenaAchieve.GetTip1)
    elseif data.flag == 1 then
        ArenaMatchCtrl.Instance:SendReq2(ArenaMatchConfig.OperType2.Achieve,data.type,data.seq)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
    end
end