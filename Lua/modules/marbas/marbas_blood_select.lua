-- 造化血祭·元魂珠选择界面
MarbasBloodSelectView = MarbasBloodSelectView or DeclareView("MarbasBloodSelectView", "marbas/marbas_blood_select")
function MarbasBloodSelectView:MarbasBloodSelectView()
    -- self.board
end
function MarbasBloodSelectView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.BloodSelect, Vector2.New(538, 295))
end

function MarbasBloodSelectView:OnCloseClick()
    ViewMgr:CloseView(MarbasBloodSelectView)
end

--------------------------MarbasBloodSelectPanel--------------------------
MarbasBloodSelectPanel = MarbasBloodSelectPanel or DeclareMono("MarbasBloodSelectPanel", UIWFlushPanel)
function MarbasBloodSelectPanel:MarbasBloodSelectPanel()
    self.data = MarbasData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushBag, init_call = false},
        {data = self.data.item_grid, func = self.FlushBag,}
    }

end 

function MarbasBloodSelectPanel:onFlush()
    self:FlushBag()
end

function MarbasBloodSelectPanel:FlushBag()
    local list = self.data:GetMarbasGemList()
    self.bag_list:SetData(list)
end

function MarbasBloodSelectPanel:CatchItemByIndex(index)
    local obj = nil 
    for k,v in pairs(self.bag_list.index_item or {}) do 
        if v:GetData():GridIndex() == index then 
            obj = v.gameObject
        end 
    end 
    return obj
end
