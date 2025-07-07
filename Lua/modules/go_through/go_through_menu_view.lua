GoThroughMenuView = GoThroughMenuView or DeclareMono("GoThroughMenuView", UIWFlushPanel)

function GoThroughMenuView:GoThroughMenuView()
    self.data = GoThroughData.Instance
end

function GoThroughMenuView:OnEnable()
    self:FlushMenuView()
end

function GoThroughMenuView:FlushMenuView()
    local data = self.data:GetPassLevelData()
    if data == nil then return end
    local reward_list = DataHelper.FormatItemList(data.reward)
    self.Grid:SetData(reward_list)
    UH.SetText(self.Name,data.reorder)
    local name = Cfg.NpcById(data.npc_id).name
    UH.SetText(self.Desc,Format(Language.DreamNotes.BossTip,name))
    if #reward_list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
    else
        self.Content.pivot = Vector2.New(0.5,0.5)
    end
end

function GoThroughMenuView:OnClickEnter()
    local npc_vo = self:GetNpcVo(self.data:GetPassLevelData())
    SceneLogic.Instance:AutoToNpc(npc_vo,nil,true)
end

function GoThroughMenuView:GetNpcVo(data)
    local vo = {}
    vo.id = data.npc_id
    vo.scene_id = SceneData.Instance:SceneId()
    vo.obj_type = SceneObjType.Npc
    vo.obj_id = 0
    vo.x = data.x
    vo.y = data.y
    return vo  
end