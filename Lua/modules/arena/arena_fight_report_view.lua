ArenaFightReportView = ArenaFightReportView or DeclareView("ArenaFightReportView","arena/arena_fight_report")

function ArenaFightReportView:LoadCallback()
    -- local data = {
    --     {name = "aaa",rank = 13,profession = 2,level = 100,is_win = 0},
    --     {name = "bbb",rank = 13,profession = 2,level = 11,is_win = 1},
    --     {name = "ccc",rank = 12,profession = 2,level = 23,is_win = 0},
    --     {name = "ddd",rank = 1,profession = 2,level = 130,is_win = 1},
    --     {name = "eee",rank = 1,profession = 2,level = 110,is_win = 1},
    --     {name = "fff",rank = 1,profession = 2,level = 105,is_win = 0},
    --     {name = "ggg",rank = 1,profession = 2,level = 108,is_win = 0},
    -- }
    self.Board:SetData(self:GetType(),Language.Arena.FightReport,Vector2.New(825, 634))
    self.data = ArenaData.Instance.record_list
    local flip_data = {}
    local index = #self.data
    for k=1,#self.data do
        flip_data[k] = self.data[index]
        index = index - 1
    end
    self.no_recordOj:SetActive(self.data[1] == nil)
    self.record_list:SetData(flip_data)
    ArenaData.Instance.has_new_report = false
end

function ArenaFightReportView:OnClose()
    -- ViewMgr:CloseView(ArenaFightReportView)
end

ArenaFightReportItem = ArenaFightReportItem or DeclareMono("ArenaFightReportItem",UIWidgetBaseItem)
function ArenaFightReportItem:SetData(data)
    local result = false
    if data.is_win == 1 then
        result = true
    end
    self.avatar:SetAvatar(data.avatar_type,data.avatar_id,data.avatar_quality)
    self.MyAvatar:SetAvatar(RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    self.winOj:SetActive(result)
    self.name.text = data.name
    self.MyName.text = RoleData.Instance:GetRoleName()
    -- self.level.text = data.level .. Language.Arena.level
    -- self.prof.text = Language.Common.Prof[data.profession]
    UH.SpriteName(self.TargetProfImg, ProfSettingB[data.profession])
    UH.SpriteName(self.MyProfImg, ProfSettingB[RoleData.Instance:GetRoleProf()])
    self.rank.text = string.format(Language.Arena.rank,data.rank)
end