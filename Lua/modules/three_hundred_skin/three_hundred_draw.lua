ThreeHundredDraw = ThreeHundredDraw or DeclareView("ThreeHundredDraw", "three_hundred_skin/three_hundred_draw")
function ThreeHundredDraw:CloseCallback()
    ThreeHundredSkinData.Instance:SetIndex(-1)
end

ThreeHundredDrawPanel = ThreeHundredDrawPanel or DeclareMono("ThreeHundredDrawPanel",UIWFlushPanel)
function ThreeHundredDrawPanel:ThreeHundredDrawPanel()
    self.state = 0 -- 0 默认 1 可抽 2可进选 3开选 -1不可操作 4开飘
    self.anim_state = 0 --0可操作 ，1不可操作
    self.close_all = false
    self.data = ThreeHundredSkinData.Instance
    self.data_cares = {
        {data = self.data:Info(), func = self.FlushCard, keys = {"group"}},
        {data = self.data:SelectList(), func = self.FlushCard, init_call = false},
    }
end
function ThreeHundredDrawPanel:onFlush()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN,TimeType.Type_Time_8, Language.UiTimeMeter.ActivityTime)
end

function ThreeHundredDrawPanel:FlushCard()
    -- local data = self.data:RamdomReward()
    local data = self.data:SerInfo(self.data:Info().group)
    if data then
        if data.select_seq ~= -1 then
            self.Face:SetActive(true)
            local info = self.data:RewardInfo(data.select_seq + 1)
            self.item = Item.Create(info.reward_item)
            self.Cell:SetData(self.item)
            self.Cell:SetObjActive(true)
            UH.SetText(self.Name, self.item:Name())
            local is_setter = false
            local path = IllusionData.Instance:GetShowPngPathByItemId(info.reward_item.item_id)
            if path then
                self.Setter:SetByPath(path)
                self.Cell:SetObjActive(false)
                is_setter = true
            end
            if Item.GetUseType(info.reward_item.item_id) == ItemUseType.ParnerSkin then
                self.Setter:SetByPath(string.format(PartnerConfig.PngPath, self.item:Config().param2))
                self.Cell:SetObjActive(false)
                is_setter = true
            end
            if not is_setter then
                self.Setter:SetTextureNil()
            end
            UH.SetText(self.Tip1, Language.ThreeHundredSkin.SelectTip[1])
        else
            self.Face:SetActive(false)
            UH.SetText(self.Tip1, Language.ThreeHundredSkin.SelectTip[2])
            UH.SetText(self.Name, "")
        end
        UH.SetText(self.Tip2, string.format(Language.ThreeHundredSkin.Tip2, data.times))
        self.List:SetCompleteCallBack(function ()
            if self.state == 2 then
                self.anim_state = 1
                for i, v in ipairs(self.List.item_list) do
                    v:Open()
                end 
            else
                if self.close_all == true then
                    self.anim_state = 1
                    for i, v in ipairs(self.List.item_list) do
                        v:Close()
                    end
                end
            end
            self.close_all = true
        end)
        self.List:SetData(data.pool)
        if data.pool[1].value == -1 then
            self.state = 1
        else
            self.state = 2
        end
        self:FlushButton()
    else
        LogDG("协议数据获取不到有问题")
    end
end
function ThreeHundredDrawPanel:FlushButton()
    if self.handle_select then
        self.EffectTool:Stop(self.handle_select)
    end
    if self.handle_draw then
        self.EffectTool:Stop(self.handle_draw)
    end
    if self.state == 1 then
        UH.SetText(self.DrawText, Language.ThreeHundredSkin.Draw10)
        UH.SetText(self.SelectText, Language.ThreeHundredSkin.Select)
        local flag = (self.data:IsMaxTime(self.data:Config()[self.data:Info().group]))
        self.Inter.Interactable = not flag
        if not flag then
            self.handle_draw = self.EffectTool:Play(6161105)
        end
        self.handle_select = self.EffectTool:Play(6161106)
    elseif self.state == 3 then
        UH.SetText(self.SelectText, Language.ThreeHundredSkin.Cancel)
        UH.SetText(self.DrawText, Language.ThreeHundredSkin.OK)
        self.Inter.Interactable = true
        self.handle_draw = self.EffectTool:Play(6161105)
    else
        local flag = (self.data:IsMaxTime(self.data:Config()[self.data:Info().group]))
        self.Inter.Interactable = not flag
        if not flag then
            self.handle_draw = self.EffectTool:Play(6161105)
        end
        self.handle_select = self.EffectTool:Play(6161106)
    end
end
function ThreeHundredDrawPanel:OnAnimEvent()
    self.anim_state = 0
end
function ThreeHundredDrawPanel:OnClickSelect()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.anim_state == 1 then
        PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.Drawing)
        return
    end
    if self.state == 1 then
        PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NoDraw)
    elseif self.state == 2 then
        self.state = 3
        self:FlushButton()
        for i, v in ipairs(self.List.item_list) do
            v:Float()
        end
    elseif self.state == 3 then
        if self.data:IsCloseTimesButNoSelect(self.data:Config()[self.data:Info().group]) then
            PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NoDrawNoSelect2)
            return
        end
        if self.data:IsCloseTimesCanSelect(self.data:Config()[self.data:Info().group]) then
            ViewMgr:CloseView(ThreeHundredDraw)
        end
        ThreeHundredSkinCtrl.SetReward(self.data:Info().group - 1, -1)
    end
end
function ThreeHundredDrawPanel:OnClickDraw()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.anim_state == 1 then
        PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.Drawing2)
        return
    end
    if self.state == 1 then
        ThreeHundredSkinCtrl.Lottery(self.data:Info().group - 1)
    elseif self.state == 3 then
        if self.data:Index() == -1 then
            PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NeedSelect)
        else
            self.state = 4
            
            --做一个动画飘的动画然后再翻过来
            local item_idx = self.data:Index() + 1
            local item = self.List:GetItem(item_idx)
            local clone_item = item.gameObject:Clone(self.RightObj, true)
            item:Hide()
            local v1 = clone_item:GetComponent(typeof(UnityEngine.RectTransform))
            v1.anchorMax = Vector2.New(0.5, 0.5)
            v1.anchorMin = Vector2.New(0.5, 0.5)
            local p1 = v1.anchoredPosition:Clone()
            local p2 = self.ResultItem.anchoredPosition:Clone()
            local time = 0
            local total = 12
            self.handle_run = TimeHelper:AddRunFrameTimer(function ()
                UH.AnchoredPosition(v1, Vector2.Lerp(p1, p2, time / total))
                time = time + 1
                if time > total then
                    UH.AnchoredPosition(v1, p2)
                    UnityGameObj.Destroy(clone_item)
                    self.state = 3
                    if self.data:IsCloseTimesButNoSelect(self.data:Config()[self.data:Info().group]) then
                        ViewMgr:CloseView(ThreeHundredDraw)
                    end
                    ThreeHundredSkinCtrl.SetReward(self.data:Info().group - 1, self.data:Index())
                    self.data:SetIndex(-1)
                end
            end,1, total + 1)
        end
    elseif self.state == 2 then
        --满一百只能预选
        if self.data:IsCloseTimesCanSelect(self.data:Config()[self.data:Info().group]) then
            PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NoDrawCanSelect)
            return
        end
        if self.data:IsCloseTimesButNoSelect(self.data:Config()[self.data:Info().group]) then
            PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NoDrawNoSelect)
            return
        end
        self.state = -1
        for i, v in ipairs(self.List.item_list) do
            v:Close()
        end
       self.handle_delay = TimeHelper:AddDelayTimer(function ()
            ThreeHundredSkinCtrl.Lottery(self.data:Info().group - 1)
        end,1)
    end
end
function ThreeHundredDrawPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self, data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_run)
    --[[ if self.data:IsCloseTimesCanSelect(self.data:Config()[self.data:Info().group]) then
        ThreeHundredSkinCtrl.SetReward(self.data:Info().group - 1, -1)
    end ]]
end
function ThreeHundredDrawPanel:OnClickTip()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[326].desc)
end
function ThreeHundredDrawPanel:OnClickClose()
    if self.state == 4 then
        return
    end
    --假如当前是第10轮且已有预选，关闭界面时默认取消第10轮选择
    if self.data:IsCloseTimesCanSelect(self.data:Config()[self.data:Info().group]) then
        ThreeHundredSkinCtrl.SetReward(self.data:Info().group - 1, -1)
    end
    ViewMgr:CloseView(ThreeHundredDraw)
end
ThreeHundredDrawItem = ThreeHundredDrawItem or DeclareMono("ThreeHundredDrawItem", UIWidgetBaseItem)
function ThreeHundredDrawItem:ThreeHundredDrawItem()
    self.skin_data = ThreeHundredSkinData.Instance
end
function ThreeHundredDrawItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Root:SetActive(true)
    self.is_can_select = false
    self.ItemToggle.isOn = false
    self.ItemToggle.enabled = true
    self.ItemToggle.enabled = false
    if data.value ~= -1 then
        local info = self.skin_data:RewardInfo(data.value + 1)
        self.item = Item.Create(info.reward_item)
        self.Cell:SetData(self.item)
        self.Cell:SetObjActive(true)
        UH.SetText(self.Name1, self.item:Name())
        UH.SetText(self.Name2, self.item:Name())
        local is_setter = false
        local path = IllusionData.Instance:GetShowPngPathByItemId(info.reward_item.item_id)
        if path then
            self.Setter:SetByPath(path)
            self.Cell:SetObjActive(false)
            is_setter = true
        end
        if Item.GetUseType(info.reward_item.item_id) == ItemUseType.ParnerSkin then
            self.Setter:SetByPath(string.format(PartnerConfig.PngPath, self.item:Config().param2))
            self.Cell:SetObjActive(false)
            is_setter = true
        end
        if not is_setter then
            self.Setter:SetTextureNil()
        end
    end
end
function ThreeHundredDrawItem:Open()
    self.Root:SetActive(true)
    self.Anim:Play("open")
end
function ThreeHundredDrawItem:Float()
    self.Root:SetActive(true)
    self.Anim:Play("float")
    self.is_can_select = true
    self.ItemToggle.enabled = true
end
function ThreeHundredDrawItem:Close()
    self.Root:SetActive(true)
    self.ItemToggle.isOn = false
    self.ItemToggle.enabled = true
    self.ItemToggle.enabled = false
    self.Anim:Play("close")
end
function ThreeHundredDrawItem:OnClick()
    if self.is_can_select == true then
        self.skin_data:SetIndex(self.data.index - 1)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    end
end
function ThreeHundredDrawItem:Hide()
    self.Root:SetActive(false)
end