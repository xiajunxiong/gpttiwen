LandsBetweenProduceView = LandsBetweenProduceView or DeclareView("LandsBetweenProduceView", "lands_between/lands_between_produce",Mod.LandsBetween.Produce)
function LandsBetweenProduceView:LandsBetweenProduceView()
    self.Board:SetData(self:GetType(),Language.LandsBetween.Title.Produce,Vector2.New(730,657))
    self.Board:SetCloseFunc(BindTool.Bind(self.OnCloseFunc, self))
end

function LandsBetweenProduceView:OnCloseFunc()
    ViewMgr:CloseView(LandsBetweenProduceView)
end

LandsBetweenProducePanel = LandsBetweenProducePanel or DeclareMono("LandsBetweenProducePanel", UIWFlushPanel)
function LandsBetweenProducePanel:LandsBetweenProducePanel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.brief_info, func = self.FlushPanel},
    }
end 

function LandsBetweenProducePanel:FlushPanel()
    local param = self.data:GetProduceInfo()
    self.list:SetData(param.list)

    UH.SetText(self.rate,param.main_tips)
end

LandsBetweenProduceCell = LandsBetweenProduceCell or DeclareMono("LandsBetweenProduceCell", UIWidgetBaseItem)
function LandsBetweenProduceCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.main_info,data.main_str)
    UH.SetText(self.str_tips,data.tips)
    local item = Item.Create(data.item)
    self.Type1:SetActive(data.index == 1)
    self.Type2:SetActive(data.index == 2)
    -- self.calls:SetData(item)
end 