ThreeHundredSkin = ThreeHundredSkin or DeclareView('ThreeHundredSkin', 'three_hundred_skin/three_hundred_skin', Mod.ThreeHundredSkin.Main)
function ThreeHundredSkin:CloseCallback()
    ThreeHundredSkinData.Instance:SetIndex(-1)
end
ThreeHundredSkinPanel = ThreeHundredSkinPanel or DeclareMono('ThreeHundredSkinPanel', UIWFlushPanel)
function ThreeHundredSkinPanel:ThreeHundredSkinPanel()
    self.data = ThreeHundredSkinData.Instance
    self.offset = 120
    self.interval = -318 * 3
    -- 1.5 左不显示 总-1.5 右不显示
    self.data_cares = {
        {data = self.data:SelectList(), func = self.onFlush, init_call = false},
    }
end
function ThreeHundredSkinPanel:onFlush()
    --[[ local temp = {}
    for i = 1, 10, 1 do
        temp[i] = {level = i, reward_group = 2}
    end ]]
    local data = self.data:Config()
    -- data = temp
    self.List:SetData(data)
    self.count = table.nums(data)
    self.total_page = math.floor(self.count / 3)
    self.page = 0
    self.rightX = -318 * (self.count - 3 - 0.5)
    self.leftX = -318 * 1.5
    if self.count <= 3 then
        self.rightX = -self.interval
        self.leftX = self.interval
    end
    self:ArrowShow()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN,TimeType.Type_Time_8, Language.UiTimeMeter.ActivityTime)
    self.QiPao:SetActive(self.data:UpLevel() > 0)
    UH.SetText(self.QiPaoTip, string.format(Language.ThreeHundredSkin.QiPao, self.data:UpLevel()))
end
function ThreeHundredSkinPanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(ThreeHundredSkin)
end
function ThreeHundredSkinPanel:OnClickTip()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[326].desc)
end
function ThreeHundredSkinPanel:OnClickNext()
    TimeHelper:CancelTimer(self.handle_frame)
    self.is_anim = true
    self.page = self.page + 1
    -- self.BtnNext:SetActive(self.page == self.total_page)
    self.final = self.page * self.interval
    self.pos = Vector2.New(0, 0)
    self.pos:Set(self.ContentRect.anchoredPosition.x, 0)
    local tempX = self.ContentRect.anchoredPosition.x
    self.handle_frame =
        TimeHelper:AddRunFrameTimer(
        function()
            tempX = tempX - self.offset
            self.pos:Set(tempX, 0)
            if self.pos.x <= self.final then
                self.pos:Set(self.final, 0)
                UH.AnchoredPosition(self.ContentRect, self.pos)
                self.is_anim = false
                TimeHelper:CancelTimer(self.handle_frame)
            end
            UH.AnchoredPosition(self.ContentRect, self.pos)
        end,
        1,
        (-self.interval * 3) / self.offset
    )
end
function ThreeHundredSkinPanel:OnClickLast()
    TimeHelper:CancelTimer(self.handle_frame)
    self.is_anim = true
    self.page = self.page - 1
    self.final = self.page * self.interval
    self.pos = Vector2.New(0, 0)
    self.pos:Set(self.ContentRect.anchoredPosition.x, 0)
    local tempX = self.ContentRect.anchoredPosition.x
    self.handle_frame =
        TimeHelper:AddRunFrameTimer(
        function()
            tempX = tempX + self.offset
            self.pos:Set(tempX, 0)
            if self.pos.x >= self.final then
                self.pos:Set(self.final, 0)
                UH.AnchoredPosition(self.ContentRect, self.pos)
                self.is_anim = false
                TimeHelper:CancelTimer(self.handle_frame)
            end
            UH.AnchoredPosition(self.ContentRect, self.pos)
        end,
        1,
        (-self.interval * 3) / self.offset
    )
end
function ThreeHundredSkinPanel:OnValueChange()
    if not self.is_anim then
        self.page = math.floor(self.ContentRect.anchoredPosition.x / self.interval)
        self.page = self.page < 0 and 0 or self.page
        self:ArrowShow()
    end
end
function ThreeHundredSkinPanel:ArrowShow()
    self.BtnLast:SetActive(self.ContentRect.anchoredPosition.x < (self.leftX or 0))
    self.BtnNext:SetActive(self.ContentRect.anchoredPosition.x > (self.rightX or 0))
end
ThreeHundredSkinItem = ThreeHundredSkinItem or DeclareMono('ThreeHundredSkinItem', UIWidgetBaseItem)
function ThreeHundredSkinItem:ThreeHundredSkinItem()
    self.skin_data = ThreeHundredSkinData.Instance
end
function ThreeHundredSkinItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local level = RoleData.Instance:GetRoleLevel()
    if level >= data.level then
        self.RedPoint:SetNum(self.skin_data:TypeRedPoint(data))
    else
        self.RedPoint:SetNum(0)
    end
    -- UH.SetText(self.Level, string.format(Language.ThreeHundredSkin.Level, data))
    UH.SetText(self.Level, string.format(Language.ThreeHundredSkin.Level, data.level))
    UH.SetText(self.BtnText, Language.ThreeHundredSkin.BtnNormal)
    local reward = self.skin_data:SelectReward(data.type + 1)
    if reward then
        self.item = Item.Create(reward)
        UH.SetText(self.Name, self.item:Name())
        self.Face:SetActive(true)
        self.Setter:SetTextureNil()
        self.Cell:SetObjActive(true)
        local path = IllusionData.Instance:GetShowPngPathByItemId(reward.item_id)
        if path then
            self.Setter:SetByPath(path)
            self.Cell:SetObjActive(false)
        end
        if Item.GetUseType(reward.item_id) == ItemUseType.ParnerSkin then
            self.Setter:SetByPath(string.format(PartnerConfig.PngPath, self.item:Config().param2))
            self.Cell:SetObjActive(false)
        end
        self.Cell:SetData(self.item)
        local is_close = self.skin_data:IsCloseTimes(data)
        if is_close then
            if self.skin_data:FinalCanGet() then
                UH.SetText(self.BtnText, Language.ThreeHundredSkin.CanGet)
            else
                UH.SetText(self.BtnText, Language.ThreeHundredSkin.HadReward)
            end
        else
            if self.skin_data:FinalCanGet() == true then
                UH.SetText(self.BtnText, Language.ThreeHundredSkin.CanGet)
            end
        end
        if self.handle_reward then
            self.EffectTool:Stop(self.handle_reward)
        end
        self.handle_reward = self.EffectTool:Play(6161103)
    else
        if self.handle_reward then
            self.EffectTool:Stop(self.handle_reward)
        end
        self.Face:SetActive(false)
        UH.SetText(self.Name, "")
    end
    local is_level = self.skin_data:IsOpenLevel(data)
    if not is_level then
        if self.handle_button then
            self.EffectTool:Stop(self.handle_button)
        end
        self.BtnInter.Interactable = false --等级不足置灰次数满了置灰
    else
        local is_close = self.skin_data:IsCloseTimes(data)
        if is_close then
            if self.skin_data:FinalCanGet() then
                self.BtnInter.Interactable = true
                if self.handle_button then
                    self.EffectTool:Stop(self.handle_button)
                end
                self.handle_button = self.EffectTool:Play(6161102)
            else
                if self.handle_button then
                    self.EffectTool:Stop(self.handle_button)
                end
                self.BtnInter.Interactable = false
            end
        else
            if self.handle_button then
                self.EffectTool:Stop(self.handle_button)
            end
            self.handle_button = self.EffectTool:Play(6161102)
            self.BtnInter.Interactable = true
        end
    end

end
function ThreeHundredSkinItem:OnClickDraw()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local is_level = self.skin_data:IsOpenLevel(self.data)
    if not is_level then
        local info = {
            content = Language.ThreeHundredSkin.GotoUplevel,
            confirm = {
                func = function()
                    ViewMgr:OpenViewByKey(Mod.Activity.Daily)
                    ViewMgr:CloseView(ThreeHundredSkin)
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.NoLevel)
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    else
        local is_close = self.skin_data:IsCloseTimes(self.data)
        if is_close then
            if self.skin_data:FinalCanGet() then
                PublicPopupCtrl.Instance:AlertTip(string.format(Language.ThreeHundredSkin.SelectReward, self.item:Name()),
    function ()
        ThreeHundredSkinCtrl.Exchange(self.data.type)
        ViewMgr:CloseView(ThreeHundredSkin)
        PublicPopupCtrl.Instance:CloseDialogTips()
    end)
                -- ThreeHundredSkinCtrl.Exchange(self.data.type)
                -- ViewMgr:CloseView(ThreeHundredSkin)
                return
            else
                PublicPopupCtrl.Instance:Center(Language.ThreeHundredSkin.LockReward)
                return
            end
        else
            self.BtnInter.Interactable = true
        end
    end
    if self.skin_data:FinalCanGet() == true then
        PublicPopupCtrl.Instance:AlertTip(string.format(Language.ThreeHundredSkin.SelectReward, self.item:Name()),
    function ()
        ThreeHundredSkinCtrl.Exchange(self.data.type)
        ViewMgr:CloseView(ThreeHundredSkin)
        PublicPopupCtrl.Instance:CloseDialogTips()
    end)
        return
    end
    -- 判断是否100抽，100抽不打开界面
    self.skin_data:SetGroup(self.data.reward_group)
    ViewMgr:OpenView(ThreeHundredDraw)
end
