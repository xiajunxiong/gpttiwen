MedalAllUpgradeView = MedalAllUpgradeView or DeclareView("MedalAllUpgradeView", "medal/medal_all_upgrade")
function MedalAllUpgradeView:MedalAllUpgradeView()
    self.Board:SetData(self:GetType(), Language.Medal.UpgradeAllView, Vector2.New(973, 600))
end 

function MedalAllUpgradeView:CloseCallback()
    self.Board:SetData(self:GetType(), Language.Resolve.Title)
end 

function MedalAllUpgradeView:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[154].desc})
end

MedalAllUpgradePanel = MedalAllUpgradePanel or DeclareMono("MedalAllUpgradePanel", UIWFlushPanel)
function MedalAllUpgradePanel:MedalAllUpgradePanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false}
    }

    self.MedalList:SetCompleteCallBack(function()
        -- if MedalData.Instance:GetPreUpgradeItem() ~= nil then 
        --     for k,v in pairs(self.MedalList.item_list) do 
        --         if BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),MedalData.Instance:GetPreUpgradeItem()) then 
        --             v:SetSelected(true)
        --             v.Cell.MengBan:SetActive(true)

        --             self:OnSelect()
        --         end 
        --     end 
        -- end 
    end)
end

function MedalAllUpgradePanel:onFlush()
    self.select_data = nil
    local data = BagData.Instance:GetMedalCanAllUpgrade()
    self.MedalList:SetData(data)
    self.PreViewList:SetData({})

    self.Empty:SetActive(true)
end

function MedalAllUpgradePanel:OnSelect()
    local data = self.MedalList:GetSelectedData()
    --获取分解内容
    local upgrade = MedalData.Instance:MedalPreViewAllUpgrade(data)
    self.PreViewList:SetData(upgrade)
    self.select_data = data

    for k,v in pairs(self.select_data or {}) do 
        if v.item_id == 0 then table.remove( self.select_data, k) end 
    end 

    self.Empty:SetActive(#upgrade == 0)
end

function MedalAllUpgradePanel:OnClickUpgrade()
    if self:CheckHadChange() then 
        PublicPopupCtrl.Instance:Center(Language.Medal.UpgradeAllUpgradeNoChange)
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
            BagCtrl.Instance:SendMedalOneKeyGradeReq({count = num,column = ItemColumnType.Item,index_flag = int_group})
            self.is_playing = false
        end,
        2.45
    )
end

function MedalAllUpgradePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)

end

function MedalAllUpgradePanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(MedalConfig.upgrade_select)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function MedalAllUpgradePanel:OnClickLevelBtn(data)
    self.SelectedText.text = data.name

    for k,v in pairs(self.MedalList.item_list) do 
        v:SetSelected(false)
        v.Cell.MengBan:SetActive(false)
    end 

    for k,v in pairs(self.MedalList.item_list) do 
        if data.grade > 0 and v:GetData().param.grade <= data.grade then 
            self:CheckAndClickItem(k,v)
        elseif data.grade == 0 then 
            self:CheckAndClickItem(k,v)
        end 
    end 

    self:OnSelect()
    self:OnClickLevelSelect()
end

function MedalAllUpgradePanel:CheckAndClickItem(k,v)
    if v:GetData().item_id == 0 then return end 
    v:SetSelected(true)
    v.Cell.MengBan:SetActive(true)

end 

function MedalAllUpgradePanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function MedalAllUpgradePanel:CheckHadChange()
    local data = self.MedalList:GetSelectedData()
    --获取分解内容
    local upgrade = MedalData.Instance:MedalPreViewAllUpgrade(data)
    for k,v in pairs(upgrade) do 
        for i,j in pairs(data) do 
            if j.param.grade == v.param.grade then 
                table.remove(data,i)
            end 
        end 
    end 

    return #data == 0
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
