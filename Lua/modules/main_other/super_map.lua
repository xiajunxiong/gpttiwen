--高级藏宝图兑换
SuperMapView = SuperMapView or DeclareView("SuperMapView", "main_other/super_map", Mod.SuperMap.Main)
----Board=======UIWBoard
----Tabbar======UIWidgetList
function SuperMapView:SuperMapView()
    self.lang = Language.MainOther
end
function SuperMapView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.MapTitle, Vector2.New(768, 660))
end

function SuperMapView:OnFlush()

end
function SuperMapView:CloseCallback()

end



SuperMapPanel = SuperMapPanel or DeclareMono("SuperMapPanel", UIWFlushPanel)
function SuperMapPanel:SuperMapPanel()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushView}
    }
end

function SuperMapPanel:FlushView()
    self.info = self.data:SuperMapInfo()[1]
    for i = 1, self.StuffList:Length() do
        self.StuffList[i]:SetDataByItemIdWithNeed(self.info["stuff_id_" .. i], self.info["stuff_num_" .. i])--SetData(Item.Create({item_id = info["stuff_id_" .. i]}))
    end
    local res = Item.Create({item_id = self.info.target})
    self.Target:SetData(res)
    -- UH.SetText(self.Name, res:Name())
    UH.SetText(self.LblUse,Language.SuperMap.UseDesc)
    UH.SetText(self.LblShow1,Language.SuperMap.ShowDesc[1])
    UH.SetText(self.LblShow2,Language.SuperMap.ShowDesc[2])

end

function SuperMapPanel:OnClickCreate()
    for i = 1, self.StuffList:Length() do
        local item = Item.Create({item_id = self.info["stuff_id_" .. i]})
        if item:Num() < self.info["stuff_num_" .. i] then
            PublicPopupCtrl.Instance:Center(ErrorText[62])
            return
        end
    end
    self.ctrl:SendCompoundReq(self.info.seq, 1, 0)
    self.EffectTool:Play(10010258)
end