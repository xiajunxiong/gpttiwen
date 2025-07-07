LandsBetweenExplainView  = LandsBetweenExplainView or DeclareView("LandsBetweenExplainView", "lands_between/lands_between_explain",Mod.LandsBetween.Explain)
function LandsBetweenExplainView:LandsBetweenExplainView()
    self.Board:SetData(self:GetType(),Language.LandsBetween.Title.Explain,Vector2.New(530,680))
    self.Board:SetCloseFunc(BindTool.Bind(self.OnCloseFunc, self))

end
function LandsBetweenExplainView:OnCloseFunc()
    ViewMgr:CloseView(LandsBetweenExplainView)
end

function LandsBetweenExplainView:LoadCallback(param_t)
    local type = 0
    local grid = 0
    if param_t ~= nil and param_t.type ~= nil then 
        type = param_t.type
        grid = param_t.grid
    end 
    self.List:SetData(LandsBetweenData.Instance:GetExplainList(LandsBetweenData.CfgLink({[1] =type,[2] =grid })))
end

LandsBetweenExplainCell = DeclareMono("LandsBetweenExplainCell", UIWidgetBaseItem)
function LandsBetweenExplainCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.Title,data.title)
    UH.SetText(self.Desc,data.desc)
    UH.SpriteName(self.Image, data.icon)
end
