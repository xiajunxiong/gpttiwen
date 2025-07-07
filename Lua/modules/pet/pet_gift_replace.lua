PetGiftReplace = PetGiftReplace or DeclareView("PetGiftReplace", "pet/pet_gift_replace")

function PetGiftReplace:PetGiftReplace()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
end
function PetGiftReplace:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.ReplaceGift, Vector2.New(672, 668))
end
------------------------------------------
PetGiftReplacePanel = PetGiftReplacePanel or DeclareMono("PetGiftReplacePanel", UIWFlushPanel)
function PetGiftReplacePanel:PetGiftReplacePanel()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_list, func = self.FlushSkillList},
        {data = BagData.Instance.item_grid[ItemColumnType.Item], func = self.FlushSkillList, init_call = false}
    }
    self.select_num = 0
    self.totalStep = 16
    GuideManager.Instance:RegisterGetGuideUi("PetSkillReplaceLearnBtn",function ()
        return self.ReplaceLearnBtn, function ()
            local item_id = GuideData.Instance:GetPetHitBookSkillItem()
            if Item.GetNum(item_id) > 0 then
                PetCtrl.SendPetOp(PET_OP_TYPE.GuideReplacePassiveSkill, PetData.Instance:GetCurSelect().index)
            else
                MainOtherCtrl.Instance:GetWayView({item_data = Item.Init(item_id)})
            end
        end
    end)

end

function PetGiftReplacePanel:OnDestroy()
    if self.fly_run_timer then
        TimeHelper:CancelTimer(self.fly_run_timer)
        self.fly_run_timer = nil
    end
    self.seted_item = false
    self.skill_list = nil
    self.index_list = nil
    self.index_list2 = nil
    self.roll_start = false
    GuideManager.Instance:UnRegisterGetGuideUi("PetSkillReplaceLearnBtn")
    UIWFlushPanel.OnDestroy(self)
end

function PetGiftReplacePanel:FlushSkillList()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local new_list, has_skill_count = pet:GetLearnGiftViewList()
    local click_skill = self.pet_data:GetLearnPreSkill()
    self.lock_num = PetData.CanLockNum(has_skill_count)
    self.JumpAnimToggle.isOn = self.pet_data.pet_gift_replace_s.jump_anim
    if pet.info.replace_skill_id == 0 or self.skill_list == nil then
        self.SkillList:SetData(new_list)
    elseif not self.pet_data.pet_gift_replace_s.jump_anim then
        --播放替换动画
        local gift_max = pet:GiftMax()
        local new_info = pet:NewGiftData()
        self.fly_index = new_info.index
        local replace_type = new_info.replace_type
        if replace_type == 1 then--炼妖
            self.fly_index = has_skill_count + self.fly_index
        end
        -- self:flySkillIcon()
        local total_skill_count = has_skill_count--gift_max
        if pet:IsLianYao() then
            total_skill_count = total_skill_count + PetRefiningData.Instance.other_info.addskill_max
        end
        self:ShowSkillAnim(self.fly_index, total_skill_count)
    else
        ViewMgr:CloseView(PetGiftReplace)
        ViewMgr:OpenView(PetGiftReplaceResultView)
    end
    self.index_list = self.index_list or {}
    self.index_list2 = self.index_list2 or {}
    self.skill_list = new_list
    
    UH.SetIcon(self.SkillIconImg, click_skill.icon_id, ICON_TYPE.SKILL)
    UH.SetText(self.SkillName, click_skill.name)
    UH.SetText(self.SkillDescText, click_skill.desc_1)
    UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[click_skill.page])
    self:SetCostNum()
    UH.SetText(self.LockBtnText, self.pet_data.pet_gift_replace_s.lockable and Language.Pet.GiftReplaceLockConfim or Language.Pet.GiftReplaceLock)
end

function PetGiftReplacePanel:ShowSkillAnim(fly_index, has_skill_count)
    if self.roll_start then
        return
    end
    self.roll_start = true
    self.roll_item_list = {}
    for i=1, has_skill_count do
        if self.index_list[i] == nil or self.index_list[i] == 0 then
            local item = self.SkillList:GetItem(i)
            if item ~= nil then
                table.insert(self.roll_item_list, item)
                if i == fly_index then
                    self.fly_index = #self.roll_item_list
                end
            end
        end
    end
    self.cur_roll_index = 1
    self.delay_time = 0.1
    -- self:rollingSelect()
    self:flySkillIcon()
end

-- function PetGiftReplacePanel:rollingSelect()
--     if self:Destroyed() then
--         self.cur_roll_index = 0
--         self.delay_time = 0
--         return
--     end
--     local item = self.roll_item_list[self.cur_roll_index]
--     item:SetRollingShow(true)
--     local pre_index = self.cur_roll_index == 1 and #self.roll_item_list or self.cur_roll_index - 1
--     local pre_item = self.roll_item_list[pre_index]
--     pre_item:SetRollingShow(false)
    
--     if self.delay_time <= 0.5 then
--         TimeHelper:AddDelayTimer(BindTool.Bind1(self.rollingSelect, self), self.delay_time)
--         self.delay_time = self.delay_time + 0.1
--     elseif self.cur_roll_index ~= self.fly_index then
--         TimeHelper:AddDelayTimer(BindTool.Bind1(self.rollingSelect, self), 0.5)
--     elseif self.cur_roll_index == self.fly_index then
--         self:flySkillIcon()
--     end
--     self.cur_roll_index = self.cur_roll_index == #self.roll_item_list and 1 or self.cur_roll_index + 1
-- end

function PetGiftReplacePanel:flySkillIcon()
    if self.fly_run_timer ~= nil then
        return
    end
    local item = self.roll_item_list[self.fly_index]
    -- local item = self.SkillList:GetItem(self.fly_index)
    if item then
        local targetPos = self.GiftItemObj.transform:InverseTransformPoint(item.PosObj.transform.position)

        self.initPos = self.IconObj.transform.localPosition
        local crossPos = Vector3.New((targetPos.x + self.initPos.x + 2) / 2, (targetPos.y + self.initPos.y + 2) / 2, 0) --中心偏移一点点
        self.icon_curve = Game.CurveParabola(self.initPos, targetPos, crossPos, 0.4)
        self.start_frame = Time.frameCount
        
        self.fly_run_timer = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.UpdatePos, self), 1, self.totalStep)
    end
end

function PetGiftReplacePanel:UpdatePos()
    if self.icon_curve == nil then
        return
    end
    local time = (Time.frameCount - self.start_frame) / self.totalStep  
    if time >= 1 then
        if self.fly_run_timer ~= nil then
            TimeHelper:CancelTimer(self.fly_run_timer)
            self.fly_run_timer = nil
        end
        self:FlyEnd()
    else
        UH.LocalPosG(self.IconObj, self.icon_curve:GetPos(time))
    end
end

function PetGiftReplacePanel:FlyEnd()
    -- self.SkillList:SetData(self.skill_list)
    
    self.roll_start = false
    ViewMgr:CloseView(PetGiftReplace)
    ViewMgr:OpenView(PetGiftReplaceResultView)
    -- UH.LocalPosG(self.IconObj, self.initPos)
end

function PetGiftReplacePanel:OnPressUp(item)
    if self.roll_start then
        TimeHelper:CancelTimer(self.press_runner)
        PublicPopupCtrl.Instance:Center(Language.Pet.SkillLearningCanNotLock)
        return
    end
    local pet = self.pet_data:GetCurSelect()
    local data = item:GetData()
    if self.long_click then
        -- PetData.Instance:SetClickSkill(data)
        -- ViewMgr:OpenView(PetSkillTip)
        -- item:SetSelected(not item:IsSelected())
    else
        if data.show_lingwu then
            PublicPopupCtrl.Instance:Center(Language.Pet.NotLingWuYet)
            item:SetSelected(false)
            TimeHelper:CancelTimer(self.press_runner)
            return
        end
        local index = data.index
        local gift_max = pet:GiftMax()
        if data.is_yao_skill == true then
            index = gift_max + data.index
        end

        if self.index_list[index] == 1 then
            self.index_list[index] = 0
            if data.is_yao_skill == true then
                self.index_list2[data.index] = 0
            end
            self.select_num = self.select_num - 1
            item:SetSelected(false)
        else
            self.select_num = 0
            for k,v in pairs(self.index_list) do
                if v == 1 then
                    self.select_num = self.select_num + 1
                end
            end
            if self.select_num >= self.lock_num then
                PublicPopupCtrl.Instance:Center(Language.Pet.CanNotLockMore)
            else
                if data.is_yao_skill == true then
                    self.index_list2[data.index] = 1
                end
                self.index_list[index] = 1
                self.select_num = self.select_num + 1
                item:SetSelected(true)
            end
        end
        self:SetCostNum()
    end
    self.long_click = false
    TimeHelper:CancelTimer(self.press_runner)
end

function PetGiftReplacePanel:OnPressDown(item)
    self.press_runner = TimeHelper:AddRunTimer(function ()
        if not self.long_click then
            local data = item:GetData()
            PetData.Instance:SetClickSkill(data)
            ViewMgr:OpenView(PetSkillTip)
        end
        self.long_click = true
    end, 1, nil, false)
end

function PetGiftReplacePanel:SetCostNum()

    if self.select_num == 0 then
        self.CostObj:SetActive(false)
        self.enough_cost = true
    else
        local cost_cfg = self.pet_data:GetLearnGiftLockCost()
        local has_num = BagData.Instance:GetNumByItemId(cost_cfg.item_use) or 0
        local cost_num = cost_cfg["price_"..tostring(self.select_num)]
        -- for i = 1, self.select_num do
        --     cost_num = cost_num + cost_cfg["price_"..tostring(i)]
        -- end
        self.CostObj:SetActive(true)
        self.enough_cost = has_num >= cost_num
        local cost_str = string.format(Language.Common.Slash, has_num, cost_num)
        cost_str = string.format(Language.Common.Xstr, self.enough_cost and COLORSTR.Green7 or COLORSTR.Red10, cost_str)
        UH.SetText(self.CostNum, cost_str)
        local item_cfg = Item.GetConfig(cost_cfg.item_use)
        UH.SetText(self.CostName, item_cfg.name)
        if not self.seted_item then
            self.CostItem:SetData(Item.Init(cost_cfg.item_use))
            self.seted_item = true
        end
    end


    local lock_num_str = string.format(Language.Common.Slash, self.select_num, self.lock_num)
    lock_num_str = string.format(Language.Pet.GiftReplaceNotice,  self.enough_cost and COLORSTR.Green7 or COLORSTR.Red10, lock_num_str)
    UH.SetText(self.NoticeText, lock_num_str)
end

function PetGiftReplacePanel:OnClickJumpAnim(jump)
    self.pet_data.pet_gift_replace_s.jump_anim = jump
end

function PetGiftReplacePanel:OnClickConfirm()
    -- self.pet_data.pet_gift_replace_s.lockable = not self.pet_data.pet_gift_replace_s.lockable
    -- UH.SetText(self.LockBtnText, self.pet_data.pet_gift_replace_s.lockable and Language.Pet.GiftReplaceLockConfim or Language.Pet.GiftReplaceLock)
end

function PetGiftReplacePanel:OnClickLearnSkill()
    if self.enough_cost then
        local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, self.pet_data:GetLearnPreSkill().item_id)
        if index == nil then
            PublicPopupCtrl.Instance:Center(Language.Pet.BookNotEnough)
            return
        end
        local pet = self.pet_data:GetCurSelect()
        --这里处理self.index_list
        if pet:IsLianYao() then
            local gift_max = pet:GiftMax()
            local yao_max = PetRefiningData.Instance.other_info.addskill_max
            for i = 1, yao_max do
                if self.index_list[gift_max + i] then
                    self.index_list[gift_max + i] = 0
                end
            end
        end
        -- LogDG(self.index_list)
        -- LogDG(self.index_list2)
        local param_t = {}
        param_t.column = ItemColumnType.Item
        param_t.index = index - 1
        param_t.num = 1
        param_t.param1 = self.pet_data:GetSelectPetIndex()
        param_t.param2 = bit:b16d(self.index_list)
        param_t.param3 = bit:b16d(self.index_list2)
        BagCtrl.Instance:SendUseItem(param_t)
    else
        local cost_cfg = self.pet_data:GetLearnGiftLockCost()
        MainOtherCtrl.Instance:GetWayView({item_data = Item.Init(cost_cfg.item_use)})
    end
end

function PetGiftReplacePanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[87].desc})
end

-------------------------------------------------------------------
PetReplaceItem = PetReplaceItem or DeclareMono("PetReplaceItem", UIWidgetBaseItem)
function PetReplaceItem:PetReplaceItem()
    self.pet_data = PetData.Instance
end

function PetReplaceItem:SetData(data)
    local skill_cfg = nil
    local new_data = TableCopy(data)
    local desc_cfg = nil
    if self.UselessObj then
        self.UselessObj:SetActive(data.useLess)
    end
    if data.skill_id ~= 0 then
        skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id)

        UH.SetText(self.Name, skill_cfg.name)
        new_data.skill_id = data.skill_id or 0
        new_data.desc = skill_cfg.desc_2
        new_data.name = skill_cfg.name
        UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
        self.LingwuObj:SetActive(false)
    else
        self:showEmpty(data.show_lingwu)
    end
    if self.SkillQuaObj then
            --第一页的是初级，第二页的是高级
        local imgName = skill_cfg and tostring(skill_cfg.page)
        if imgName ~= nil and tonumber(imgName) then
            UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[skill_cfg.page])
            self.SkillQuaObj:SetActive(true)
        else
            self.SkillQuaObj:SetActive(false)
        end
    end
    if self.NewTipObj then
        self.NewTipObj:SetActive(data.new_flag)
    end
    if self.YaoSkillSign then
        self.YaoSkillSign:SetActive(data.is_yao_skill)
    end
    UIWidgetBaseItem.SetData(self, new_data)
end

function PetReplaceItem:showEmpty(show_lingwu)
    self.LingwuObj:SetActive(show_lingwu)
    UH.SetText(self.Name, "")
    self.SelectBgObj:SetActive(not show_lingwu)
end

function PetReplaceItem:SetRollingShow(show)
    self.SelectFrame:SetActive(show)
end

function PetReplaceItem:SetSelected(selected)
    self.SelectBgObj:SetActive(selected)
    self.is_selected = selected
end

function PetReplaceItem:OnDestroy()
    self:uncareData()
end

-------------------------------------------------
PetGiftReplaceResultView = PetGiftReplaceResultView or DeclareView("PetGiftReplaceResultView","pet/pet_gift_replace_result")
function PetGiftReplaceResultView:PetGiftReplaceResultView()
    self.data = PetData.Instance
    self.lang = Language.Pet
end

function PetGiftReplaceResultView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.ReplaceResult, Vector2.New(758.582, 678))
    self.pet_info = PetData.Instance:GetCurSelect()
    local new_gift_data = self.pet_info:NewGiftData()
    self.HasSkillList:SetData(self.pet_info:GetLearnGiftViewList())
    local new_list = TableCopy(self.pet_info:GetLearnGiftViewList())
    if new_gift_data ~= nil then
        if new_gift_data.replace_type == 1 then
            new_list[new_gift_data.yao_index] = new_gift_data
        else
            new_list[new_gift_data.index] = new_gift_data
        end
    end
    self.LearnSkillList:SetData(new_list)
end

function PetGiftReplaceResultView:OnClickGiftItem(data)
    PetData.Instance:SetClickSkill(data)
    ViewMgr:OpenView(PetSkillTip)
end

function PetGiftReplaceResultView:openGiveUpDialog(save_or_giveup)
    local info = {
        confirm = {
            func = function ()
                PetCtrl.SendPetOp(PET_OP_TYPE.ReplacePassiveSkillSave, PetData.Instance:GetSelectPetIndex(), save_or_giveup)
                ViewMgr:CloseView(PetGiftReplaceResultDialog)
                ViewMgr:CloseView(PetGiftReplaceResultView)
                if save_or_giveup == 1 then
                    --如果放弃当前技能，检查背包是否还有本技能书，如果有则再打开学习界面
                    local click_skill = PetData.Instance:GetLearnPreSkill()
                    if click_skill and Item.GetNum(click_skill.item_id) > 0 then
                        ViewMgr:OpenView(PetGiftReplace)
                    end
                end
            end
        },
        tnr = DTTodayNotRemind.PetSkillResult
    }
    if save_or_giveup == 1 then
        info.content = Language.Pet.ReplaceGiveUpNotice
    else
        local new_gift_data = self.pet_info:NewGiftData()
        if new_gift_data ~= nil then
            local new_gift_config = PetData.Instance:GetGiftDetail(new_gift_data.skill_id)
            local new_color = QualityColorStr[new_gift_config.page + 2]
            local old_gift = self.pet_info:GetLearnGiftViewList()[new_gift_data.index]
            if new_gift_data.replace_type == 1 then
                old_gift = self.pet_info:GetLearnGiftViewList()[new_gift_data.yao_index]
            end
            local old_gift_config = PetData.Instance:GetGiftDetail(old_gift.skill_id)
            local old_color = QualityColorStr[old_gift_config.page + 2]
            info.content = string.format(Language.Pet.ReplaceSaveNotive, new_color, new_gift_config.name, old_color, old_gift_config.name)
        else
            local confirm_func = info.confirm.func
            if confirm_func and type(confirm_func) == "function" then
                confirm_func()
            end
            BuglyReport("pet gift replace new_gift_data is nil, replace_skill_id is "..self.pet_info.info.replace_skill_id)
        end
    end
    if PublicPopupCtrl.Instance.dialog_tips_data.today_not_reminds[info.tnr] then
        if info and info.confirm then
            local confirm_func = info.confirm.func
            if confirm_func and type(confirm_func) == "function" then
                confirm_func()
            end
        end
    else
        ViewMgr:OpenView(PetGiftReplaceResultDialog, info)
    end
end

function PetGiftReplaceResultView:OnClickClose()
    -- PetCtrl.SendPetOp(PET_OP_TYPE.ReplacePassiveSkillSave, PetData.Instance:GetSelectPetIndex(), 0)
    -- ViewMgr:CloseView(PetGiftReplaceResultView)
    self:openGiveUpDialog(1)
end

function PetGiftReplaceResultView:OnClickGiveUp()
    -- PetCtrl.SendPetOp(PET_OP_TYPE.ReplacePassiveSkillSave, PetData.Instance:GetSelectPetIndex(), 0)
    -- ViewMgr:CloseView(PetGiftReplaceResultView)
    self:openGiveUpDialog(1)
end

function PetGiftReplaceResultView:OnClickConfirm()
    self:openGiveUpDialog(0)
end

--------------------------------------------------
PetGiftReplaceResultDialog = PetGiftReplaceResultDialog or DeclareView("PetGiftReplaceResultDialog", "pet/pet_gift_replace_dialog")
function PetGiftReplaceResultDialog:PetGiftReplaceResultDialog()
end

function PetGiftReplaceResultDialog:LoadCallback(param_info)
    self.param_info = param_info
    self.tnr = param_info.tnr
    self.save_or_giveup = param_info.save_or_giveup
    self.TodayNotRemind.isOn = PublicPopupCtrl.Instance.dialog_tips_data.today_not_reminds[self.tnr]
    UH.SetText(self.Content, param_info.content)
end

function PetGiftReplaceResultDialog:OnClickCancel()
    ViewMgr:CloseView(PetGiftReplaceResultDialog)
end

function PetGiftReplaceResultDialog:OnClickClose()
    ViewMgr:CloseView(PetGiftReplaceResultDialog)
end

function PetGiftReplaceResultDialog:OnClickConfirm()
    if nil ~= self.tnr then PublicPopupCtrl.Instance.dialog_tips_data.today_not_reminds[self.tnr] = self.TodayNotRemind.isOn end
    if self.param_info and self.param_info.confirm then
        local confirm_func = self.param_info.confirm.func
        if confirm_func and type(confirm_func) == "function" then
            confirm_func()
        end
    end
end
