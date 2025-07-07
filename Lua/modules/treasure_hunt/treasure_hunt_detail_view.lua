TreasureHuntDetailView = TreasureHuntDetailView or DeclareView("TreasureHuntDetailView","treasure_hunt/treasure_hunt_detail")
VIEW_DECLARE_MASK(TreasureHuntDetailView,ViewMask.BgBlockNoAnim)

function TreasureHuntDetailView:LoadCallback()
    TreasureHuntData.Instance.is_detail_opened = true
end

function TreasureHuntDetailView:CloseCallback()
    TreasureHuntData.Instance.smart_data:SetDirty("delay_flush")
    TreasureHuntData.Instance.last_index = -1
end

TreasureHuntDetailFlush = TreasureHuntDetailFlush or DeclareMono("TreasureHuntDetailFlush",UIWFlushPanel)

function TreasureHuntDetailFlush:TreasureHuntDetailFlush()
    if TreasureHuntData.Instance.treasure_hunt_count == 1 then
        self:HideDi()
    end
    self.data_instance = TreasureHuntData.Instance
    self.is_first_enter = true
    self.ui_obj = {}
    self.pet_skin_eff_handles = {}
    for k=1,5 do
        self.ui_obj[k] = UIObjDrawer.New()
        self.pet_array[k]:SetShowData({
            ui_obj = self.ui_obj[k],
            view = self
        })
    end
    self.data_cares = {
        {data = TreasureHuntData.Instance.smart_data,func = self.TreasureFlushGet, keys = {"is_get"}},
        {data = TreasureHuntData.Instance.smart_data,func = self.TreasureFlushChange, keys = {"is_change"}, init_call = false},
        {data = TreasureHuntData.Instance.smart_data, func = self.CheckAuto, keys = {"detail_auto"}, init_call = false}
    }
end

function TreasureHuntDetailFlush:DataFlush()
    self.data = TreasureHuntData.Instance.smart_data.node_item_list
    local treasure_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    if TreasureHuntData.Instance.treasure_hunt_count == 1 then
        if self.data[1].type == TREASURE_HUNT_ITEM_TYPE.Pet then
            local pet_info = PetData.Instance:GetPetInfo(self.data[1].id)
            self.ui_obj[3]:SetData(DrawerHelper.GetNpcPath(pet_info:ResId()))
            self.pet_txt_array[3].text = pet_info:Name()
        elseif self.data[1].type == TREASURE_HUNT_ITEM_TYPE.Partner or PartnerData.IsPartnerItem(self.data[1].id) then
            local partner = PartnerData.IsPartnerItem(self.data[1].id) or Cfg.PartnerById(self.data[1].id)
            self.ui_obj[3]:SetData(DrawerHelper.GetNpcPath(partner.res_id))
            self.pet_txt_array[3].text = partner.name
        elseif treasure_cfg.skin_id ~= 0 and self.data[1].id == treasure_cfg.skin_id then
            local skin_cfg = Cfg.SkinPetInfoByActiveStuffId(treasure_cfg.skin_id)
            local item_cfg = Item.GetConfig(treasure_cfg.skin_id)
            self.ui_obj[3]:SetData(DrawerHelper.GetNpcPath(skin_cfg.res_id))
            self.pet_txt_array[3].text = item_cfg.name
        elseif self.data[1].type == TREASURE_HUNT_ITEM_TYPE.Item then
            --self.item_array[3]:SetDataByItemId(self.data[1].id)
            local item_id = self.data[1].id or 0
             self.item_array[3]:SetData(Item.Create({item_id = item_id}))
            if self.data[1].num > 1 then
                self.item_txt_array[3].text = string.format("%s*%s",Item.GetName(self.data[1].id),tostring(self.data[1].num))
            else
                self.item_txt_array[3].text = Item.GetName(self.data[1].id)
            end
            -- self.item_txt_array[3].text = Item.GetName(self.data[1].id)
        end
    else
        if #self.data ~= 5 then
            LogError("[ERROR] TreasureHuntDetailFlush:TreasureHuntDetailFlush======> Recv treasure count ~= 5") 
            return 
        end
        for k, v in pairs(self.data)do
            if v.type == TREASURE_HUNT_ITEM_TYPE.Pet then
                local pet_info = PetData.Instance:GetPetInfo(v.id)
                self.ui_obj[k]:SetData(DrawerHelper.GetNpcPath(pet_info:ResId()))
                self.pet_txt_array[k].text = pet_info:Name()
                self.petOj_array[k]:SetActive(false)
            elseif v.type == TREASURE_HUNT_ITEM_TYPE.Partner or PartnerData.IsPartnerItem(v.id) then
                local partner = PartnerData.IsPartnerItem(v.id) or Cfg.PartnerById(v.id)
                self.ui_obj[k]:SetData(DrawerHelper.GetNpcPath(partner.res_id))
                self.pet_txt_array[k].text = partner.name
                self.petOj_array[k]:SetActive(false)
            elseif treasure_cfg.skin_id ~= 0 and v.id == treasure_cfg.skin_id then
                local skin_cfg = Cfg.SkinPetInfoByActiveStuffId(treasure_cfg.skin_id)
                local item_cfg = Item.GetConfig(treasure_cfg.skin_id)
                self.ui_obj[k]:SetData(DrawerHelper.GetNpcPath(skin_cfg.res_id))
                self.pet_txt_array[k].text = item_cfg.name
            elseif v.type == TREASURE_HUNT_ITEM_TYPE.Item then
                --self.item_array[k]:SetDataByItemId(v.id)
                local item_id = v.id or 0
                self.item_array[k]:SetData(Item.Create({item_id = item_id}))
                if v.num > 1 then
                    self.item_txt_array[k].text = string.format("%s*%s",Item.GetName(v.id),tostring(v.num))
                else
                    self.item_txt_array[k].text = Item.GetName(v.id)
                end
            end
        end
        self:Reset()
    end
    for i=1,5 do
        self.petOj_array[i]:SetActive(false)
    end
    self:ShowReward()
end

--寻宝结果物品展示特效
function TreasureHuntDetailFlush:PlayEffectByColor(index,color, not_show_anim)
    if not ViewMgr:IsOpen(TreasureHuntDetailView) then
        return
    end
    if not_show_anim then
        self.save_skip_handle = self.save_skip_handle or {}
        if self["skipEffects"..index] then
            for i=1, self["skipEffects"..index]:Length() do
                if self["skipEffects"..index][i] then
                    self["skipEffects"..index][i]:SetActive(i == color)
                end
                if i == color then
                    self.save_skip_handle[index] = i
                end
            end
        end
    else
        self.save_handle = self.save_handle or {}
        if self["effects"..index] then
            for i=1, self["effects"..index]:Length() do
                self["effects"..index][i]:SetActive(i == color)
                if i == color then
                    self.save_handle[index] = i
                end
            end
        end
    end
end

function TreasureHuntDetailFlush:StopEffect()
    if self.save_handle ~= nil then
        for k, v in pairs(self.save_handle)do
            self["effects"..k][v]:SetActive(false)
        end
        self.save_handle = nil
    end
    if self.save_skip_handle ~= nil then
        for k, v in pairs(self.save_skip_handle) do
            self["skipEffects"..k][v]:SetActive(false)
        end
        self.save_skip_handle = nil
    end
end

function TreasureHuntDetailFlush:TreasureFlushChange()
    self:TreasureFlush()
end

function TreasureHuntDetailFlush:TreasureFlushGet()
    if self.left_show_times and self.left_show_times ~= 0 then
        return
    end
    self:TreasureFlush()
end

function TreasureHuntDetailFlush:TreasureFlush()
    -- Debuger.LogError("fluse....count :"..#TreasureHuntData.Instance.save_index)
    for i = 1, self.PetSkinEffPars:Length() do
        self.PetSkinEffPars[i]:SetActive(false)
    end
    if #TreasureHuntData.Instance.save_index == 5 then
        self:Reset()
        -- Debuger.LogError("------return------")
        return
    end
    self.ToggleJump.isOn = TreasureHuntData.Instance.not_show_anim
    self:OnEffectStart()
    -- self:ShowReward()
end

--宝箱打开特效
function TreasureHuntDetailFlush:OnEffectStart()
    if self.showed_open_effect then
        self:ShowReward()
        return
    end
    local count = TreasureHuntData.Instance.treasure_hunt_count
    local item_list = TreasureHuntData.Instance.smart_data.node_item_list
    local treasure_hunt_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    self.effectIndex = treasure_hunt_cfg.is_limit == 1 and 1 or 2
    if TreasureHuntData.Instance.smart_data.hunt_type == HUNT_TYPE.XUN_QI then
        self.effectIndex = 3
    end
    self.show_eff_index = {}
    for i = 1, count do
        local item_cfg
        local color = 1
        if item_list[i].type == 1 then
            item_cfg = PetData.Instance:GetPetInfo(item_list[i].id).config
            if item_cfg.quality == 2 then
                color = 3
            elseif item_cfg.quality >= 4 then
                color = 7
            elseif item_cfg.quality ~= 1 then
                color = 5
            else
                color = 2
            end
        elseif item_list[i].type == 2 or PartnerData.IsPartnerItem(item_list[i].id) then
            color = 2
            local co = item_list[i].type == 2 and Cfg.PartnerById(item_list[i].id) or PartnerData.IsPartnerItem(item_list[i].id)
            if co then
                color = co.partner_quality
            end
            if color == 5 then
                color = 7
            end
        else
            item_cfg = Item.GetConfig(item_list[i].id)
            color = item_cfg.color
        end
        self["Box"..self.effectIndex.."EffectBallPos"..i][color]:SetActive(true)
        self.show_eff_index[i] = color
    end
    
    
    if TreasureHuntData.Instance.not_show_anim then
        self:DelayFlushDetail()
    else
        self.AnimateObj:SetActive(true)
        self.BG1:SetActive(TreasureHuntData.Instance.smart_data.hunt_type ~= HUNT_TYPE.XUN_QI)
        self.BG2:SetActive(TreasureHuntData.Instance.smart_data.hunt_type == HUNT_TYPE.XUN_QI)
        self["OpenEffect"..self.effectIndex]:SetActive(true)
        if TreasureHuntData.Instance.treasure_hunt_count > 1 then
            AudioMgr:PlayEffect(AudioType.UI,"treasure_hunt_five")
        else
            AudioMgr:PlayEffect(AudioType.UI,"treasure_hunt_one")
        end
        if self.delay_flush_detail_view ~= nil then
            TimeHelper:CancelTimer(self.delay_flush_detail_view)
        end
        self.delay_flush_detail_view = TimeHelper:AddDelayTimer(function ()
            self:DelayFlushDetail()
        end, 9)
    end
end

function TreasureHuntDetailFlush:DelayFlushDetail()
    self.AnimateObj:SetActive(false)
    if self.effectIndex ~= nil then
        self["OpenEffect"..self.effectIndex]:SetActive(false)
        for k,v in pairs(self.show_eff_index) do
            self["Box"..self.effectIndex.."EffectBallPos"..k][v]:SetActive(false)
        end
    end
    for i = 1, self.PetSkinEffPars:Length() do
        self.PetSkinEffPars[i]:SetActive(false)
    end
    self.showed_open_effect = true
    self:DataFlush()
end

function TreasureHuntDetailFlush:ShowReward()
    if TreasureHuntData.Instance.treasure_hunt_count == 1 then
        self.left_show_times = 1
        TimeHelper:AddDelayTimer(function()
            self:Show(self.data,1,0.3)
        end,0.3)
    else
        local isrun = true
        self.left_show_times = 5 - #TreasureHuntData.Instance.save_index
        TimeHelper:AddRunTimer(function()
                if isrun then
                    local index = self:GetRandomIndex()
                    self:Show(self.data,index,0.3)
                    -- self.is_first_enter = false
                    if self.data[index].type == 1 or self.data[index].type == 2 or PartnerData.IsPartnerItem(self.data[index].id) then
                        isrun = false
                    end
                    -- LogError("self.left_show_times",self.left_show_times)
                    -- isrun = (self.data[index].type == 1 and false) or true
                end
            end,0.5,5 - #TreasureHuntData.Instance.save_index,false)

    end
end

function TreasureHuntDetailFlush:Show(data,index,delay_time)
    --index2 为对应格子位置，单抽时默认在3位置播放
    local index2 = (#data == 1 and 3) or index
    local not_show_anim = TreasureHuntData.Instance.not_show_anim
    local treasure_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    
    if data[index].type == TREASURE_HUNT_ITEM_TYPE.Pet then
        --宠物
        local pet = PetData.Instance:GetPetInfo(self.data[index].id)
        local call_back = function ()
            if self.left_show_times then
                self.left_show_times = self.left_show_times - 1
            end
        end
        if pet and pet.config.quality >= 4 then
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_special")
        else
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_normal")
        end
        if self["effect_delay_"..index] ~= nil then
            TimeHelper:CancelTimer(self["effect_delay_"..index])
        end
        self["effect_delay_"..index] = TimeHelper:AddDelayTimer(function ()
            local color = 3
            if pet then
                if pet.config.quality == 2 then
                    color = 3
                elseif pet.config.quality >= 4 then
                    color = 7
                elseif pet.config.quality ~= 1 then
                    color = 5
                elseif pet.config.quality == 1 then
                    color = 2
                end
            end
            self:PlayEffectByColor(index2, color, not_show_anim)
            self.left_show_time2 = TimeHelper:AddDelayTimer(function()
                if not ViewMgr:IsOpen(TreasureHuntDetailView) then
                    return
                end
                self.petOj_array[index2]:SetActive(true)
                if pet then
                    PetData.Instance:SetNewPet(pet)
                    ViewMgr:OpenView(PetGain, {is_xilian = false, close_call = call_back})
                else
                    TreasureHuntData.Instance.smart_data.is_change = not TreasureHuntData.Instance.smart_data.is_change
                    call_back()
                end
            end, not_show_anim and 0.5 or 2)
        end,0.5)
    elseif data[index].type == TREASURE_HUNT_ITEM_TYPE.Partner or PartnerData.IsPartnerItem(data[index].id) then
        --伙伴
        self["effect_delay_"..index] = TimeHelper:AddDelayTimer(function ()
            local co = data[index].type == 2 and Cfg.PartnerById(data[index].id) or PartnerData.IsPartnerItem(data[index].id)
            local color = co and co.partner_quality or 2
            if color == 5 then
                color = 7
            end
            local item_list = BagData.Instance:GetItemsByItemId(co.activation_item_norex)
            if #item_list > 0 then
                MainOtherCtrl.Instance:OpenKeyUseView(item_list[1])
            end
            self:PlayEffectByColor(index2, color, not_show_anim)
            self.left_show_time2 = TimeHelper:AddDelayTimer(function()
                if not ViewMgr:IsOpen(TreasureHuntDetailView) then
                    return
                end
                self.petOj_array[index2]:SetActive(true)
                PartnerData.Instance:SetNewPartner(co.partner_id)
                local call_back = function ()
                    if self.left_show_times then
                        self.left_show_times = self.left_show_times - 1
                    end
                end
                TimeHelper:AddDelayTimer(function ()
                    ViewMgr:OpenView(PartnerViewLock,{close_call = call_back})
                end,2)
            end, not_show_anim and 0.5 or 2)
        end,0.5)
    elseif treasure_cfg.skin_id ~= 0 and data[index].id == treasure_cfg.skin_id then
        --宠物皮肤（激活道具）
        self.PetSkinEffPars[index2]:SetActive(true)
        local color = Item.GetColor(data[index].id)
        if color >= 5 then
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_special")
        else
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_normal")
        end
        
        if self["effect_delay_"..index] ~= nil then
            TimeHelper:CancelTimer(self["effect_delay_"..index])
        end
        self["effect_delay_"..index] = TimeHelper:AddDelayTimer(function ()
            self:PlayEffectByColor(index2,color, not_show_anim)
            if self.pet_skin_eff_handles[index2] == nil then
                self.pet_skin_eff_handles[index2] = self.EffectTool:Play(3165160, self.PetSkinEffPars[index2])
            end
            self.left_show_time1 = TimeHelper:AddDelayTimer(function()
                if not ViewMgr:IsOpen(TreasureHuntDetailView) then
                    return
                end
                if self:Destroyed() then
                    self:OnDestroy()
                else
                    self.petOj_array[index2]:SetActive(true)
                    if self.left_show_times then
                        self.left_show_times = self.left_show_times - 1
                    end
                end
                -- 播放完自己飘字
                if data[index].is_active then
                    local co = Item.GetConfig(data[index].id)
                    local skin_cfg = Cfg.SkinPetInfoByActiveStuffId(data[index].id)
                    PublicPopupCtrl.Instance:CenterI(Item.Init(skin_cfg.decompose_item, skin_cfg.decompose_num))
                    PublicPopupCtrl.Instance:Center(Language.TreasureHunt.HasPetSkinTip)
                end
            end, not_show_anim and 0.5 or 2)
        end, 0.5)
    elseif data[index].type == TREASURE_HUNT_ITEM_TYPE.Item then
        local color = Item.GetColor(self.data[index].id)
        if color >= 5 then
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_special")
        else
            AudioMgr:PlayEffect(AudioType.UI,"treasure_get_normal")
        end
        
        if self["effect_delay_"..index] ~= nil then
            TimeHelper:CancelTimer(self["effect_delay_"..index])
        end
        self["effect_delay_"..index] = TimeHelper:AddDelayTimer(function ()
            self:PlayEffectByColor(index2,color, not_show_anim)
            self.left_show_time1 = TimeHelper:AddDelayTimer(function()
                if not ViewMgr:IsOpen(TreasureHuntDetailView) then
                    return
                end
                if self:Destroyed() then
                    self:OnDestroy()
                else
                    self.itemOj_array[index2]:SetActive(true)
                    if self.left_show_times then
                        self.left_show_times = self.left_show_times - 1
                    end
                end
            end, not_show_anim and 0.5 or 2)
        end, 0.5)
    end
    if #data == 5 then
        self:TableInsert(index)
    end
end

function TreasureHuntDetailFlush:JumpEffect()
    if self.delay_flush_detail_view ~= nil then
        TimeHelper:CancelTimer(self.delay_flush_detail_view)
        self.delay_flush_detail_view = nil
    end
    self:DelayFlushDetail()
end

function TreasureHuntDetailFlush:ClickNotAgain(toggle)
    TreasureHuntData.Instance.not_show_anim = toggle
end

function TreasureHuntDetailFlush:TableInsert(value)
    for k, v in pairs(TreasureHuntData.Instance.save_index)do
        if v == value then
            return 
        end
    end
    table.insert(TreasureHuntData.Instance.save_index,value)
end

function TreasureHuntDetailFlush:CheckAuto()
    if TreasureHuntData.Instance.smart_data.detail_auto then
        self.showed_open_effect = false
        self:HideAll()
        self:StopEffect()
        TreasureHuntCtrl.Instance:AutoTreasureHuntReq()
        TreasureHuntData.Instance.smart_data.detail_auto = false
    end
end

function TreasureHuntDetailFlush:OnePrizeDraw()
    --未播放完展示时不进行下一次寻宝，否则会报错
    if self.left_show_times and self.left_show_times ~= 0 then
        LogError("OnePrizeDraw return 1")
        return
    end
    local treasure_hunt_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    if treasure_hunt_cfg.treasure_id ~= 5 and self:JudgePetIsFull() then
        self:OnClickClose()
        return
    end
    if not self:JudgeCanClick(TreasureHuntData.Instance.treasure_hunt_count) then
        return
    end

    self:Reset()
    local item_id = 0
    
    if TreasureHuntData.Instance.treasure_hunt_count == 1 then
        item_id = treasure_hunt_cfg.once_item_id
    else
        item_id = treasure_hunt_cfg.five_item_id
    end

    local item = BagData.Instance:IsHasItemById(item_id)
    local item_list = {item}
    local item_num = item ~= nil and -1 or 0
    if item_num == 0 then
        item_num, item_list = BagData.Instance:GetAllItemsByItemId(treasure_hunt_cfg.once_item_id)
    end
    local function sendOneMoreTime()
        if self:IsActiveAndEnabled() then
            self.showed_open_effect = false
            self:HideAll()
            self:StopEffect()
            local comsume_info = {}
            if item_num == -1 then
                comsume_info = {[1] = {col = ItemColumnType.Item, idx = item:GridIndex(), num = 1}}
            else
                local need_num = 5
                for k,v in pairs(item_list) do
                    local t = {}
                    t.col = ItemColumnType.Item
                    t.idx = v:GridIndex()
                    if need_num - v.num > 0 then
                        need_num = need_num - v.num
                        t.num = v.num
                        table.insert(comsume_info, t)
                    else
                        t.num = need_num
                        table.insert(comsume_info, t)
                        break
                    end
                end
            end
            TreasureHuntCtrl.Instance:onCSTreasureHuntReq(TreasureHuntData.Instance.treasure_hunt_count, TreasureHuntData.Instance.smart_data.cur_treasure_id,false,comsume_info)
        end
        ViewMgr:CloseView(DialogTipsView)
    end
    if item_num == -1 or item_num >= 5 then
        sendOneMoreTime()
    else
        -- local param = {item_id = item_id, treasure_id = treasure_hunt_cfg.treasure_id, draw_times = TreasureHuntData.Instance.treasure_hunt_count}
        -- ViewMgr:OpenView(TreasureHuntQuickBuy,param)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id})})
        -- if TreasureHuntData.Instance.treasure_hunt_count == 1 then
        --     PublicPopupCtrl.Instance:Center(string.format(Language.TreasureHunt.NeedItemHint,Item.GetName(item_id)))
        -- else
        --     PublicPopupCtrl.Instance:Center(string.format(Language.TreasureHunt.NeedItemHintFive,Item.GetName(item_id), Item.GetName(treasure_hunt_cfg.once_item_id)))
        -- end
    end
end

function TreasureHuntDetailFlush:JudgePetIsFull()
    if HuntNotPet[self.data_instance.smart_data.cur_treasure_id]
        or TreasureHuntData.Instance:GetCurSelectCfg().treasure_type == HUNT_TYPE.XUN_QI then --抽奖类型为非宠物时
        return false
    end 
    local pet_list = PetData.Instance:GetPetList()
    local max = PetData.Instance:GetCurMaxPetCount()
    if table.nums(pet_list) + TreasureHuntData.Instance.treasure_hunt_count > max then
        PublicPopupCtrl.Instance:Center(ErrorInfo[152])
        return true
    end
    return false
end

function TreasureHuntDetailFlush:OnClickClose()
    if self.left_show_times and self.left_show_times ~= 0 then
        return
    end
    self:Reset()
    ViewMgr:CloseView(TreasureHuntDetailView)
    if TreasureHuntData.Instance.smart_data.is_guide_draw then
        ViewMgr:CloseView(TreasureHuntView)
    end
    TreasureHuntData.Instance.smart_data.is_guide_draw = false
end

function TreasureHuntDetailFlush:OnDestroy()
    TreasureHuntData.Instance.is_detail_opened = false
    for k,v in pairs(self.ui_obj) do
        v:DeleteMe()
    end
    for i=1,5 do
        if self["effect_delay_"..i] ~= nil then
            TimeHelper:CancelTimer(self["effect_delay_"..i])
            self["effect_delay_"..i] = nil
        end
    end
    TreasureHuntData.Instance.smart_data.is_guide_draw = false
    TimeHelper:CancelTimer(self.delay_flush_detail_view)
    TimeHelper:CancelTimer(self.left_show_time1)
    TimeHelper:CancelTimer(self.left_show_time2)
    UIWFlushPanel.OnDestroy(self)
end

function TreasureHuntDetailFlush:JudgeCanClick(type)
    -- if BattleData.Instance:IsServerFighting() then
    --     PublicPopupCtrl.Instance:Center(Language.TreasureHunt.CannotDoInBattle)
    --     return false
    -- end
    if type == 1 then
        if self.data ~= nil then
            if self.data[1].type == 0 and PartnerData.IsPartnerItem(self.data[1].id) == nil then
                if not self.itemOj_array[3].activeSelf then return false end
            else
                if not self.petOj_array[3].activeSelf then return false end
            end
        else
            LogError("寻宝结果物品展示界面self.data为空 type = 1")
        end
    else
        if self.data ~= nil then
            for k, v in pairs(self.data)do
                if v.type == 0 then
                    if v.id == TreasureHuntData.Instance:GetCurSelectCfg().skin_id then
                        if not self.petOj_array[k].activeSelf then return false end
                    elseif PartnerData.IsPartnerItem(v.id) == nil then
                        if not self.itemOj_array[k].activeSelf then return false end
                    end
                else
                    if not self.petOj_array[k].activeSelf then return false end
                end
            end
        else
            LogError("寻宝结果物品展示界面self.data为空")
        end
    end
    return true
end

function TreasureHuntDetailFlush:GetRandomIndex()
    local ran_num = math.random(1,5)
    local function isHave(num)
        for k, v in pairs(TreasureHuntData.Instance.ran_index)do
            if v == num then
                return true
            end
        end
        return false
    end
    local continue_flag = TreasureHuntData.Instance.last_index ~= -1
    while(isHave(ran_num) or continue_flag) do
        if #TreasureHuntData.Instance.ran_index == 5 then
            TreasureHuntData.Instance.ran_index = {}
        end
        ran_num = math.random(1,5)
        if TreasureHuntData.Instance.last_index ~= -1 then
            if #TreasureHuntData.Instance.ran_index < 4 and ran_num == TreasureHuntData.Instance.last_index then
                continue_flag = true
            else
                continue_flag = false
            end
        end
    end
    if not isHave(ran_num) then
        table.insert(TreasureHuntData.Instance.ran_index,ran_num)
        TreasureHuntData.Instance.cur_count = #TreasureHuntData.Instance.save_index
        return ran_num
    end
    -- Debuger.LogError("aa...."..ran_num)
end

function TreasureHuntDetailFlush:Reset()
    TreasureHuntData.Instance.cur_count = 0
    TreasureHuntData.Instance.ran_index = {}
    TreasureHuntData.Instance.save_index = {}
end

function TreasureHuntDetailFlush:HideAll()
    for i=1, 5 do
        self.itemOj_array[i]:SetActive(false)
        self.petOj_array[i]:SetActive(false)
    end
end

function TreasureHuntDetailFlush:HideDi()
    for i=1,4 do
        self.diOj_array[i]:SetActive(false)
    end
end