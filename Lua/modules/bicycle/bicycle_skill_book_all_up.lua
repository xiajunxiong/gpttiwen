BicycleSkillBookAllUp = BicycleSkillBookAllUp or DeclareView("BicycleSkillBookAllUp", "bicycle/bicycle_skill_book_all_up")
function BicycleSkillBookAllUp:BicycleSkillBookAllUp()
    self.Board:SetData(self:GetType(), Language.Bicycle.Title8, Vector2.New(973, 600))
end 

-- function BicycleSkillBookAllUp:CloseCallback()
--     self.Board:SetData(self:GetType(), Language.Resolve.Title)
-- end 

function BicycleSkillBookAllUp:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(413)
end

BicycleSkillBookAllUpPanel = BicycleSkillBookAllUpPanel or DeclareMono("BicycleSkillBookAllUpPanel", UIWFlushPanel)
function BicycleSkillBookAllUpPanel:BicycleSkillBookAllUpPanel()
    self.data = BicycleOtherData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false}
    }
    self.select_data_list = {
        {qua = 0},
        {qua = 4},
        {qua = 5},
        {qua = 6},
    }
end

function BicycleSkillBookAllUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.BtnList:SetData(self.select_data_list)
end

function BicycleSkillBookAllUpPanel:onFlush()
    self.select_data = nil
    local list_data = self.data:GetBookCanAllUpgradeList()
    self.MedalList:SetData(list_data)
    self.PreViewList:SetData({})

    self.Empty:SetActive(true)
end

function BicycleSkillBookAllUpPanel:OnSelect()
    local data = self.MedalList:GetSelectedData()
    --获取分解内容
    local upgrade = self.data:GetBookPreViewAllUpgrade(data)
    self.PreViewList:SetData(upgrade)
    self.select_data = data

    for k,v in pairs(self.select_data or {}) do 
        if v.item_id == 0 then table.remove( self.select_data, k) end 
    end 

    self.Empty:SetActive(#upgrade == 0)
end

function BicycleSkillBookAllUpPanel:OnClickUpgrade()
    local upgrade = self.data:GetBookPreViewAllUpgrade(self.select_data)
    if TableIsEmpty(upgrade) then 
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Tip5)
        return 
    end 

    if self.select_data == nil then
        return
    end
    if self.is_playing == true then
        return
    end
    self.is_playing = true
    local select_items = self.MedalList:GetSelectedItem()
    --做个动画做完再发协议
    for i, v in ipairs(select_items) do
        v:PlayAnim()
    end
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if self.EffectTool ~=nil and self.EffectTool.isActiveAndEnabled then
                self.EffectTool:Play(2161057)
            end 
        end,
        0.4
    )
    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            if self.EffectTool ~=nil and self.EffectTool.isActiveAndEnabled then
                self.EffectTool:Play(2161059)
            end 
        end,
        1.65
    )
    self.handle_delay2 =
        TimeHelper:AddDelayTimer(
        function()
            local res = {}
            for _, value in ipairs(self.select_data or {}) do
                table.insert(res, value.index)
            end
            local num = table.nums(res)
            local int_group = BagData.Instance:GetBagMarkIntGroup(res)
            BicycleCtrl.Instance:SendCSSmartMountsSystemKeyComposeReq(num,ItemColumnType.Item,int_group)
            self.is_playing = false
        end,
        2.45
    )
end

function BicycleSkillBookAllUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)

end

function BicycleSkillBookAllUpPanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function BicycleSkillBookAllUpPanel:OnClickLevelBtn(data)
    local qua = data.qua
    if qua > 0 then
        local color = QualityColorStrShallow[qua]
        UH.SetText(self.SelectedText, ColorStr(Language.Bicycle.SelectBtn[qua], color))
    else
        UH.SetText(self.SelectedText, Language.Bicycle.SelectBtn[qua])
    end

    for k,v in pairs(self.MedalList.item_list) do 
        v:SetSelected(false)
        v.Cell.MengBan:SetActive(false)
    end 

    for k,v in pairs(self.MedalList.item_list) do 
        local item_data = v:GetData()
        if data.qua == 0 or data.qua == item_data:Color() then
            self:CheckAndClickItem(k,v)
        end
    end 

    self:OnSelect()
    self:OnClickLevelSelect()
end

function BicycleSkillBookAllUpPanel:CheckAndClickItem(k,v)
    if v:GetData().item_id == 0 then return end 
    v:SetSelected(true)
    v.Cell.MengBan:SetActive(true)
end 

function BicycleSkillBookAllUpPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

-- function BicycleSkillBookAllUpPanel:CheckHadChange()
--     local data = self.MedalList:GetSelectedData()
--     --获取分解内容
--     local upgrade = MedalData.Instance:MedalPreViewAllUpgrade(data)
--     for k,v in pairs(upgrade) do 
--         for i,j in pairs(data) do 
--             if j.param.grade == v.param.grade then 
--                 table.remove(data,i)
--             end 
--         end 
--     end 

--     return #data == 0
-- end


BicycleSkillBookAllUpSelectBtn = BicycleSkillBookAllUpSelectBtn or DeclareMono("BicycleSkillBookAllUpSelectBtn", UIWidgetBaseItem)
function BicycleSkillBookAllUpSelectBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua = data.qua
    if qua > 0 then
        local color = QualityColorStr[qua]
        UH.SetText(self.Name, ColorStr(Language.Bicycle.SelectBtn[qua], color))
    else
        UH.SetText(self.Name, Language.Bicycle.SelectBtn[qua])
    end
end


MedalAllUpgradeCell = MedalAllUpgradeCell or DeclareMono("MedalAllUpgradeCell", UIWidgetBaseItem)
function MedalAllUpgradeCell:MedalAllUpgradeCell()
    self.is_long_click = false
end
function MedalAllUpgradeCell:SetData(data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)
    self.Cell.MengBan.transform:SetAsLastSibling()
    self.Cell.MengBan:SetActive(false)
    self:OnSelect(false)
    self.inter.Interactable = self.data.item_id > 0
end
function MedalAllUpgradeCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_func)
end
function MedalAllUpgradeCell:OnSelect(selected)
    if self.data.item_id == 0 then return end 

    if not self.is_long_click then

        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
    else
        self:SetSelected(not selected)
    end
end
function MedalAllUpgradeCell:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end
function MedalAllUpgradeCell:OnRelease()
    TimeHelper:CancelTimer(self.handle_delay)
end
function MedalAllUpgradeCell:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if self.EffectTool ~=nil and self.EffectTool.isActiveAndEnabled then
                self.EffectTool:Play(2161058)
            end 
        end,
        0.3336
    )
end
function MedalAllUpgradeCell:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                if self.Cell ~= nil then 
                    CellClicks.BagGridNoButton(self.Cell)
                end 
            end
        end,
        0.2
    )
end
