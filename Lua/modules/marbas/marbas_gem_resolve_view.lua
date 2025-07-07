-- 造化装备·元魂批量分解
MarbasGemResolveView = MarbasGemResolveView or DeclareView("MarbasGemResolveView", "marbas/marbas_gem_resolve_view",Mod.Marbas.GemResolve)
function MarbasGemResolveView:MarbasGemResolveView()
    -- self.board
end

function MarbasGemResolveView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Marbas.GemResolveTitle)
end

function MarbasGemResolveView:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[323].desc})
end

MarbasGemResolvePanel = MarbasGemResolvePanel or DeclareMono("MarbasGemResolvePanel", UIWFlushPanel)
function MarbasGemResolvePanel:MarbasGemResolvePanel()
    self.data = MarbasData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false}
    }

    -- 预选中
    self.GemList:SetCompleteCallBack(function()
        if self.data:GetPreGemResolveItem() ~= nil then 
            for k,v in pairs(self.GemList.item_list) do 
                if BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),self.data:GetPreGemResolveItem()) then 
                    v:SetSelected(true)
                    v.Cell.MengBan:SetActive(true)

                    self:OnSelect()
                end 
            end 
        end 
    end)
end

function MarbasGemResolvePanel:onFlush()
    if self.is_playing then return end 
    self.select_data = nil
    local data = self.data:GetCanGemResolveItem()
    self.GemList:SetData(data)
    self.Single.gameObject:SetActive(false)
end
function MarbasGemResolvePanel:OnSelect()
    local data = self.GemList:GetSelectedData()
    --获取分解内容
    local resolve = self.data:GetGemResolveList(data)
    self.select_data = data

    self.Single.gameObject:SetActive(true)
    local single_data = Item.Create({item_id = CommonItemId.CanHun})
    self.Single:SetData(single_data)
    UH.SetText(self.SingleStr, single_data:QuaName().."："..resolve)
end

function MarbasGemResolvePanel:OnClickResolve()
    if self.select_data == nil then
        return
    end
    if self.is_playing == true then
        return
    end
    self.is_playing = true
    local select_items = self.GemList:GetSelectedItem()
    --做个动画做完再发协议
    for i, v in ipairs(select_items) do
        v:PlayAnim()
    end
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(30000825)
        end,
        0.4
    )
    -- self.handle_delay1 =
    --     TimeHelper:AddDelayTimer(
    --     function()
    --         local res = {}
    --         for _, value in ipairs(self.select_data or {}) do
    --            
    --         end
    --         self.is_playing = false
    --     end,
    --     2.2
    -- )
    self.block:SetActive(true)
end

function MarbasGemResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
end
function MarbasGemResolvePanel:OnEffectComplete()
    for _, value in pairs(self.select_data or {}) do
        MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.GemDecompose,value:GridIndex(),value:GridColumn(),0)
    end

    self.is_playing = false
    self.block:SetActive(false)
end

ResolveMarbasGemItem = ResolveMarbasGemItem or DeclareMono("ResolveMarbasGemItem", UIWidgetBaseItem)
function ResolveMarbasGemItem:ResolveMarbasGemItem()
    self.is_long_click = false
end
function ResolveMarbasGemItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_func)
    TimeHelper:CancelTimer(self.handle_delay)
end
function ResolveMarbasGemItem:SetData(data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)
    self.Cell.MengBan.transform:SetAsLastSibling()
    self:OnSelect(false)

end
function ResolveMarbasGemItem:OnSelect(selected)
    if not self.is_long_click then
        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
    else
        self:SetSelected(not selected)
    end
end
function ResolveMarbasGemItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end

function ResolveMarbasGemItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if ViewMgr:IsOpen(MarbasGemResolveView) then 
                self.EffectTool:Play(30000813)
            end 
        end,
        0.3336
    )
end
function ResolveMarbasGemItem:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.Cell)
            end
        end,
        0.2
    )
end
