MaterialTipView = MaterialTipView or DeclareView("MaterialTipView", "material/material_tip")

VIEW_DECLARE_MASK(MaterialTipView,ViewMask.BlockClose)

function MaterialTipView:MaterialTipView()
    self.data = MaterialData.Instance
end

function MaterialTipView:LoadCallback(data)
    local config = self.data:GetChapterList(data.seq-1)[#self.data:GetChapterList(data.seq-1)]
    local desc = Format(Language.Material.ChapterTip,config.chapter_name,config.level_name)
    local is_first_kill = self.data:IsFirstKill(config) or false
    UH.SetText(self.Desc,is_first_kill and ColorStr(desc,COLORSTR.Green7) or desc)
    self.DescImg.color = is_first_kill and COLORS.Green7 or COLORS.White

    local limit_desc = Format(Language.Material.ChapterLevelTip,data.chapter_level_limit)
    local is_not_limit = RoleData.Instance:GetRoleLevel() >= data.chapter_level_limit
    UH.SetText(self.Level,is_not_limit and ColorStr(limit_desc,COLORSTR.Green7) or limit_desc)
    self.LevelImg.color = is_not_limit and COLORS.Green7 or COLORS.White
end

function MaterialTipView:OnClickComfirm()
    ViewMgr:CloseView(MaterialTipView)
end

MaterialChallengeTipView = MaterialChallengeTipView or DeclareView("MaterialChallengeTipView", "material/material_challenge_tip")

VIEW_DECLARE_MASK(MaterialChallengeTipView,ViewMask.BlockClose)

function MaterialChallengeTipView:MaterialChallengeTipView()
    self.data = MaterialData.Instance
    self.Board:SetData(self:GetType(),Language.Material.Condition,Vector2.New(644,366))
end

function MaterialChallengeTipView:LoadCallback()
    local data = self.data:GetChapterConfig(self.data:GetChallengeData())
    for i=1,self.Condition:Length() do
        local flag = self.data:GetConditonFlag(data,i)
		self.Condition[i]:SetData({id = data["condition_"..i],flag = flag})
    end
end