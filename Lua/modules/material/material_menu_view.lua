MaterialMenuView = MaterialMenuView or DeclareMono("MaterialMenuView", UIWFlushPanel)

function MaterialMenuView:MaterialMenuView()
    self.data = MaterialData.Instance
end

function MaterialMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushMenuView()
end

function MaterialMenuView:FlushMenuView()
    local config = self.data:GetChapterConfig(self.data:GetChallengeData())
    UH.SetText(self.Name,Format("%s-%s %s",config.chapter + 1,config.level + 1,config.level_name))
    local name = Cfg.NpcById(config.boss_id).name
    UH.SetText(self.Desc,Format(Language.DreamNotes.BossTip,name))
    local reward_list = self.data:GetRewardList(config)
    -- table.insert(reward_list,Item.New({item_id = CommonItemId.Exp,num = self.data:GetExpNum()}))
    self.Grid:SetData(reward_list)
    self.config = config
end

function MaterialMenuView:OnClickEnter()
    MainOtherCtrl.Instance:SetWaitViewFunc(nil)
    local npc_vo = self.data:GetSceneNpcVo(self.config.boss_id)
    SceneLogic.Instance:AutoToNpc(npc_vo,nil,true)
end

function MaterialMenuView:OnClickPreview()
    ViewMgr:OpenView(MaterialChallengeTipView)
end