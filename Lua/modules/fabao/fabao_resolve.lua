FabaoResolve = FabaoResolve or DeclareView("FabaoResolve", "fabao/fabao_resolve")
----Board=======UIWBoard
----Tabbar======UIWidgetList
function FabaoResolve:FabaoResolve()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
end
function FabaoResolve:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title3, Vector2.New(482, 536))
--self.Board:SetTabbarVisible(true)
-- self.Currency:DefaultSet()
end

function FabaoResolve:OnFlush()
-- body
end
function FabaoResolve:CloseCallback()
end

FabaoResolvePanel = FabaoResolvePanel or DeclareMono("FabaoResolvePanel", UIWFlushPanel)
function FabaoResolvePanel:FabaoResolvePanel()
    self.fabao_data = FabaoData.Instance
    self.lang = Language.Fabao
    self.data_cares = {
        {data = self.fabao_data:ViewData(), func = self.FlushView, keys = {"select_data"}}
    }
end

function FabaoResolvePanel:FlushView()
    self.data = self.fabao_data:ViewData().select_data
    self.FabaoCell:SetData(self.data)
    UH.SetText(self.LblName, self.data.name)
    UH.SetText(self.LblType, self.lang.FBType[self.data.type])
    UH.SetText(self.LblLevel, string.format("Lv.%d", self.data.info.fabao_level))
    --显示分解返回
    local info = self.fabao_data:ExlainInfo(self.data.id, self.data.info.fabao_level)
    self.ItemList:SetData(info.rewards)
    local jie_level = self.data.info.fabao_jie
    for i = 1,self.JieList:Length() do
        self.JieList[i].enabled = i > jie_level
    end
end

function FabaoResolvePanel:OnClickFenJie()
    FabaoCtrl.SendFenJie(self.data.id)
end
