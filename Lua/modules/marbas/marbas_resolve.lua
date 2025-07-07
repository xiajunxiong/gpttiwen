-- 造化装备·分解界面
MarbasResolveView = MarbasResolveView or DeclareView("MarbasResolveView", "marbas/marbas_resolve",Mod.Marbas.Resolve)
function MarbasResolveView:MarbasResolveView()
    -- self.board
end
function MarbasResolveView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.ResolveView, Vector2.New(590, 673))

    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Order)
end

function MarbasResolveView:OnCloseClick()
    ViewMgr:CloseView(MarbasResolveView)
end

--------------------------MarbasResolvePanel--------------------------
MarbasResolvePanel = MarbasResolvePanel or DeclareMono("MarbasResolvePanel", UIWFlushPanel)
function MarbasResolvePanel:MarbasResolvePanel()
    self.data = MarbasData.Instance
    self.language = Language.Marbas
    self.data_cares = {
        {data = self.data.item_grid, func = self.FlushResolve},
    }

    -- self.EquipList:SetCompleteCallBack(function() 
    --     if self.lock_data ~= nil then 
    --         for k,v in pairs(self.EquipList.item_list) do 
    --             if v:GetData() ~= nil and self.lock_data ~= nil and BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),self.lock_data) then 
    --                 self.EquipList:SetectItem(k,true)

    --                 self:OnSelect()
    --                 self.lock_data = nil
                    
    --                 break
    --             end
    --         end 
    --     end 
    -- end)

    self.fix_type = -1
end

function MarbasResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.time_func then 
        TimeHelper:CancelTimer(self.time_func)
    end 
end

function MarbasResolvePanel:OnEffectComplete()
    self.block:SetActive(false)

    self.is_sending = true
    local select_items = self.EquipList:GetSelectedItem()
    local params = {}

    for k,v in pairs(select_items) do 
        params[v:GetData():GridIndex()] = 1
    end 

    self.is_playing = false
    -- LogError("start ",params)

    -- 16 * 32
    local fit_param = {}
    for i = 1,16 do 
        local bits = bit:d2b(0) -- 1~32
        for k,v in pairs(bits) do 
            if params[(i-1) * 32 + (k - 1) ] ~= nil then 
                bits[32 - (k - 1)] = 1
            end 
        end 
        table.insert(fit_param,bit:b2d(bits))
    end 


    -- for k,v in pairs(fit_param) do 
    --     LogError("sisj",k,bit:d2b(v))
    -- end 

    MarbasCtrl.Instance:SendZaoHuaEquipDisconposeReq({bag_list = fit_param})
    -- for k,v in pairs(select_items) do 
    --     MarbasCtrl.Instance:SendEquipOper(
    --         MarbasReqType.Resolve,
    --         v:GetData():GridIndex())
    -- end 


    self.is_sending = false
end

function MarbasResolvePanel:onFlush()
    if self.is_playing or self.is_sending then return end 

    self:FlushResolve()    
end

function MarbasResolvePanel:OnSelect()
    local data = self.EquipList:GetSelectedData()
    --获取分解内容
    local resolve = self.data:GetResolveList(data)
    self.List:SetData(resolve)
    self.select_data = data

end

function MarbasResolvePanel:FlushResolve()

    if self.is_playing or self.is_sending then
        return
    end
    self.select_data = nil
    local data = self.data:GetMarbasResolve(self.fix_type)
    self.EquipList:SetData(data)

    self:OnSelect()    

    self.empty:SetActive(#data == 0)
end

function MarbasResolvePanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(MarbasConfig.fix_select)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function MarbasResolvePanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end 

function MarbasResolvePanel:OnClickLevelBtn(data)
    self.SelectedText.text = data.name
    
    self.fix_type = data.qua
    self:FlushResolve()
    self:OnClickLevelSelect()
end 

function MarbasResolvePanel:OnClickResolve()
    if self.select_data == nil then
        return
    end
    if self.is_playing == true then
        return
    end
    local select_items = self.EquipList:GetSelectedItem()
    if #select_items == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.ResolveEmptyTips)
        return 
    end 

    self.is_playing = true
    
    --做个动画做完再发协议
    for i, v in ipairs(select_items) do
        v:PlayAnim()
    end
    self.handle_delay =
    TimeHelper:AddDelayTimer(
    function()
        -- self.EffectTool:Play(30000825)
    end,
    0.4)

    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            local res = {}
            for _, value in ipairs(self.select_data or {}) do
                if value:HadStone() then 
                    self.with_stone = true 
                    value:UnStone()
                end 
                table.insert(res, value.index)
            end

            self.EffectTool:Play(3165113)
            self.EffectTool:Play(3165114)
            if self.with_stone then 
                PublicPopupCtrl.Instance:Center(Language.Resolve.StoneUnFix)
                self.with_stone = false
            end 
        end,
        0.8
    )
    self.block:SetActive(true)
end

MarbasResolveItem = MarbasResolveItem or DeclareMono("MarbasResolveItem", UIWidgetBaseItem)
function MarbasResolveItem:MarbasResolveItem()
    self.is_long_click = false
end

function MarbasResolveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)


    self.Cell.MengBan.transform:SetAsLastSibling()
    self:OnSelect(false)
end 

function MarbasResolveItem:OnSelect(selected)
    if not self.is_long_click then
        self:SetSelected(selected)
    else 
        self:SetSelected(not selected)
    end 

    self.Cell.MengBan:SetActive(selected)
end 

function MarbasResolveItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end
function MarbasResolveItem:OnRelease()
    -- TimeHelper:CancelTimer(self.handle_delay)
end
function MarbasResolveItem:OnPressDown()
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

function MarbasResolveItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if ViewMgr:IsOpen(MarbasResolveView) then 
                self.EffectTool:Play(30000813)
            end 
        end,
        0.3336
    )
end


MarbasResolvePreShowCell = MarbasResolvePreShowCell or DeclareMono("MarbasResolvePreShowCell", UIWidgetBaseItem)
function MarbasResolvePreShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.icon, data:IconId())
    UH.SetText(self.content, data:QuaName().."*"..data:Num())
end

