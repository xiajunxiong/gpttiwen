DreamNotesMenuView = DreamNotesMenuView or DeclareMono("DreamNotesMenuView", UIWFlushPanel)

function DreamNotesMenuView:DreamNotesMenuView()
    self.data = DreamNotesData.Instance
end

function DreamNotesMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.BtnEnter:SetActive(true)
    self.BtnChallenge:SetActive(false)
    self:FlushMenuView()
end

function DreamNotesMenuView:FlushMenuView()
    self.config = self.data:GetPassLevelData(self.data:GetChallengeLevel())
    UH.SetText(self.Name,Format(Language.DreamNotes.Level,DataHelper.GetDaXie(self.config.level)))
    local name = Cfg.NpcById(self.config.boss_id).name
    UH.SetText(self.Desc,Format(Language.DreamNotes.BossTip,name))
    local reward_list = {}
    if not self.data:IsFirstKill(self.config) then
        self.data:GetFirstRewardData(self.config,reward_list)
    else
        self.data:GetRandomRewardData(self.config,reward_list)
        self.data:GetRewardData(self.config,reward_list)
    end
    self.Grid:SetData(reward_list)
    if #reward_list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
    else
        self.Content.pivot = Vector2.New(0.5,0.5)
    end
end

function DreamNotesMenuView:OnClickChallenge()
    DreamNotesCtrl.Instance:SendChallengeReq(2,self.config.level + 1)
end

function DreamNotesMenuView:OnClickEnter()
    MainOtherCtrl.Instance:SetWaitViewFunc(nil)
    local npc_vo = DreamNotesData.Instance:GetSceneNpcVo(self.config.boss_id)
    SceneLogic.Instance:AutoToNpc(npc_vo,nil,true)
end

----------------------------DreamNotesMenuCall----------------------------
DreamNotesMenuCall = DreamNotesMenuCall or DeclareMono("DreamNotesMenuCall", UIWidgetBaseItem)

function DreamNotesMenuCall:SetData(data)
    self.Call:SetData(Item.New(data))
    self.Label:SetActive(data.first_kill == true)
    self.Mark:SetActive(data.has_reward == true)
    if self.Random then
        self.Random:SetActive(data.is_random == true)
    end
    UIWidgetBaseItem.SetData(self, data)
end