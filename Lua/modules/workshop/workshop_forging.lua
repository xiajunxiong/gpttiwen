WorkshopForging = WorkshopForging or DeclareMono("WorkshopForging", UIWFlushPanel)
--职业类型/装备等级
--打开选中对应职业类型 和对应可制作等级（或者说可穿戴等级）
--等级1-13级
--显示配方是否学习
--显示是否可以强化
--配方获取逻辑
function WorkshopForging:WorkshopForging()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.forging_data = self.data:ForgingData()
    self.data_cares = {
        {data = self.forging_data, func = self.FlushLeftItem, keys = {"select_prof", "select_level"}, init_call = false},
        {data = BagData.Instance:GetEquipList(), func = self.FlushEquipList},
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false},
        {data = self.data.forged_flag, func = self.FlushProtectArea, init_call = false}
    }
    self.bag_equip_guide = false
    GuideManager.Instance:RegisterGetGuideUi("ForingItem", function ()
        if self.ItemList:SetupCompleted() then
            local id = GuideData.Instance:GetForingId()
            local data = self.data:GetSelectById(id)
            if data then
                self.bag_equip_guide = true
                local index = self.ItemList:GetSelectedIndex(data)
                return self.ItemList:GetItem(index).gameObject
            else 
                local cfg = self.data:GetForgeComposeCfg(id)
                self.forging_data.select_level = Equip.GetEquipLevel(cfg.target)
                self.forging_data.select_prof = cfg.type
            end
        end
        return nil,nil
    end)

    self.effect_play_list = {}
    self.temp_list = {}

    self.send_data = {}
    self.flush_left_item_handle = nil
    self.time_func = nil
end
function WorkshopForging:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    if self.bag_equip_guide then
        GuideManager.Instance:Start(19)
    end

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
    for index, value in ipairs(self.effect_play_list) do
        value:DeleteMe()
    end

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end

    self.data.forged_flag.flag = false

    LinkMarkData.Instance:MidCheck()
end
function WorkshopForging:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.BtnParent:SetActive(false)
    self.ProfArrow:SetActive(true)
    self.LevelArrow:SetActive(true)
end

function WorkshopForging:onFlush()
    self:FlushLeftItem(self.data:GetForgeMark())
    local show = self.data:GetTypeShow(ComposeGroup.Forging)
    UH.SetText(self.LblProf, show[self.forging_data.select_prof].type_name)
    show = self.data:GetTypeShow(ComposeGroup.Level)
    UH.SetText(self.LblLevel, show[self.forging_data.select_level - WorkshopData.Params.MinLevel + 1][2])
    --UH.SetText(self.LblLevel, string.format("Lv.%d", show[self.forging_data.select_level][1]))
end
function WorkshopForging:FlushLeftItem(_mark)
    local data = self.data:ForgingShowList()
    self.ItemList:SetData(data)
    if _mark == nil then 
        self.ItemList:ClickItem(1)
    else 
    local pos_ = 1
    for k,v in pairs(data) do 
        if _mark.seq == v.seq then 
            pos_ = k
        end 
    end
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    self.flush_left_item_handle = TimeHelper:AddDelayTimer(function()
            self.flush_left_item_handle = nil
            self.ItemList:ClickItem(pos_)
            if pos_ <= #data -4 then 
                self.ItemList:JumpVerticalIndex(pos_,104)
            else 
                self.ItemList:JumpVerticalIndex(#data -4,104)
            end 
        end,0.2)
    end 
end
function WorkshopForging:FlushLeftItem2()
    local data = self.data:ForgingShowList()
    self.ItemList:SetData(data)
end
function WorkshopForging:FlushRightItem()
    self:FlushLeftItem2()
    if self.target_info ~= nil then
        self:OnClickItem(self.target_info)
    end
end
function WorkshopForging:FlushEquipList()
    local list = BagData.Instance:GetEquipList()
	for i = 0, 4 do --八件套--4
		local equip = list[i]
		if equip then
			local old_data = self.Equiplist[i + 1]:GetData()
			if old_data ~= equip then
				self.Equiplist[i + 1]:SetData(equip)
				self.EquipLevelObjList[i + 1]:SetActive(true)
				self.EquipLevelList[i + 1]:GetComponent(typeof(UnityUI.Text)).text = equip:Level()..Language.Common.Level
            end
            self.EquipDesclist[i + 1]:SetActive(false)
		else
			self.Equiplist[i + 1]:SetData(Item.New())
            self.EquipLevelObjList[i + 1]:SetActive(false)
            self.EquipDesclist[i + 1]:SetActive(true)
		end
    end
end
function WorkshopForging:OnClickCreate()
    --直接制作和强化制作
    -- seq 数量 是否强化
    if not self.target_info then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Error(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        for _, v in ipairs(self.stuff_tip_list) do
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], v.name, v.lack))
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    -- if not BagData.Instance:TryFindEmptyInBag(self.target_info.target) then 
        if not BagData.Instance:TryFindEmptyInTempBag() then 
            PublicPopupCtrl.Instance:Error(ErrorText[100])
            return 
        end
    -- end 

    local param_t = {
        content = string.format(
            Language.Workshop.Forging,
            QualityColorStr[Item.GetColor(self.target_info.target)],
            Item.GetName(self.target_info.target)
        ),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                self:ShowCreateEffectEffect()
                AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
            end 
        },
        tnr = DTTodayNotRemind.WorkshopForge
    }
    if GuideManager.Instance:IsGuide() then 
        self:ShowCreateEffectEffect()
        AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
        return 
    end 
    
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function WorkshopForging:OnClickMulit()
    --直接制作和强化制作
    -- seq 数量 是否强化
    if not self.target_info then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end

    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Error(ErrorText[21])
        return
    end

    if self.is_not_enough_stuff then
        for _, v in ipairs(self.stuff_tip_list) do
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], v.name, v.lack))
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end

    if not BagData.Instance:TryFindEmptyInTempBagMulit(self.send_data.forge_time) then 
        PublicPopupCtrl.Instance:Error(Language.Bag.BagFetchWithoutEmpty)
        return 
    end 

    local param_t = {
        content = string.format(
            Language.Workshop.Forging,
            QualityColorStr[Item.GetColor(self.target_info.target)],
            Item.GetName(self.target_info.target)
        ),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                self:ShowCreateEffectEffect()
                AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
            end 
        },
        tnr = DTTodayNotRemind.WorkshopForge
    }

    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function WorkshopForging:OnClickMulitNum(is_pop)

    if self.Mulit_Num.text ~= "" then
        local num = tonumber(self.Mulit_Num.text)
        local test_num = num
        if num <= 0 then 
            test_num = 1 
            if not is_pop then PublicPopupCtrl.Instance:Center(Language.Workshop.ForgeMulitLimitedZero) end 
        end 
        if self:MulitLimitedCheck(test_num) then 
            self.send_data.forge_time = test_num
        else 
            if num > 1 and not is_pop then 
                PublicPopupCtrl.Instance:Center(Language.Workshop.ForgeMulitLimited) 
            end 
            -- if num <= 0 then  end 
            self.send_data.forge_time = self.send_data.limited_time
        end 

        self.Mulit_Num.text = self.send_data.forge_time
        self:ChangeStuffNeed()
        self:FlushResult()
    end 
end

-- 组件的回调可以自然适应修改，无须再敲
function WorkshopForging:OnClickBtnMulitNum(_num)
    local test_num = self.send_data.forge_time + _num
    self.Mulit_Num.text = test_num
end

function WorkshopForging:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickBtnMulitNum(num)
        end
	end,0.1)
end

function WorkshopForging:OnClickUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function WorkshopForging:MulitLimitedCheck(num)
    self.send_data.limited_time = 1

    for i = 1, 6 do
        if i ~= 6 then
            local item_id = self.target_info["stuff_id_" .. i] or 0
            local need_num = self.target_info["stuff_num_" .. i] or 0            
            if item_id > 0 then
                local limited_num = math.floor(Item.GetNum(item_id) / need_num)
                if limited_num < num then 
                    self.send_data.limited_time = limited_num > 0 and limited_num or 1
                    return false
                end 
            end
        end
    end

    return true
end

function WorkshopForging:ShowCreateEffectEffect()
    local wait_time = 0.05
    local effect_time = 0.05
    local stuff_complete = {}
    local index = 1
    local pos = nil
    local pos_num = 5
    local flag_forge_flag = false

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end

    self.effect_handle_run = Runner.Instance:RunUntilTrue(
        function()
            effect_time = effect_time + Time.deltaTime
            if effect_time >= wait_time then
                -- for i = 1, 5 do 
                -- 索检有效操作项
                for i = pos_num ,1 ,-1 do 
                    stuff_complete[i],pos = self:CheckAndTryCreateEffect(i)
                    if not stuff_complete[i] then 
                        pos_num = i
                        break 
                    end 
                end 
                -- 项操作
                if not stuff_complete[i] then 
                    -- 算好第一次有效判定启动特效
                    -- 全都走了一次就直接开始锻造
                    if index > 0 and index < 6 then
                        self.effect_play_list[index] = ForgeCreateEffect.New(self,pos)
                        self.effect_play_list[index]:Play()
                    elseif not flag_forge_flag then 
                        flag_forge_flag = true

                        self.NabingProg:SetActive(true)
                        self:StartToShowCreating()
                    end 
                    index = index + 1
    
                    -- if index > 0 and index < 5 then
                    --     self.EffectTool:Play(10010254, self.FlyEffectEnd)
                    -- end 
                end
                pos_num = pos_num -1 
                if pos_num <= 0 then pos_num = 5 end 
                    
                effect_time = 0
                -- end 

                local total_complete = stuff_complete[1] and stuff_complete[2] and stuff_complete[3] and stuff_complete[4] and stuff_complete[5]
                -- if total_complete then 
                --     self.effect_handle_delay = TimeHelper:AddDelayTimer(
                --         function()
                --             self.WaitNabe:SetActive(false)
                --             self.NabingProg:SetActive(true)
                --             self:StartToShowCreating()
                --         end,
                --         0.5
                --     )
                --     -- self.EffectTool:Play(10010254, self.FlyEffectEnd)
                -- end 

                return total_complete
            end

            return false
        end
    )

    -- self:StartToShowCreating()
    self.data.forged_flag.flag = true
    -- self.ProtectArea:SetActive(true)
end

function WorkshopForging:StartToShowCreating()
    self.EffectTool:Play("10010315")

    TimeHelper:AddDelayFrameTimer(function()
        self.NabingEffect:SetActive(true)
        self.WaitNabe:SetActive(false)
        self.prog_step = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateCreateProgEffect, self), 0.1)
    end,5)
end

function WorkshopForging:UpdateCreateProgEffect()
    local value_prog = self.ProgSlider.value
    local cur_prog = math.floor(value_prog * 100)
    if not self.ProText:Equals(nil)  then 
        UH.SetText(self.ProText,cur_prog.."%")
    end 
end

function WorkshopForging:CanelCreateEffect()
    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
    self.WaitNabe:SetActive(true)
    self.NabingProg:SetActive(false)
    local forge_type = ( self.target_info.activation > 0 ) and WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ or WORLSHOP_OP.WORKSHOP_OP_BATCH_COMP_REQ

    if self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0 then
        self.ctrl:SendOperateReq(
            forge_type,
            self.seq,
            self.send_data.forge_time and self.send_data.forge_time or 1,
            self.Gou.activeSelf and 1 or 0
        )
    else
        self.ctrl:SendOperateReq(
            forge_type, 
            self.seq, 
            self.send_data.forge_time and self.send_data.forge_time or 1,
            0)
    end
    

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end
    if self.effect_handle_delay then
        TimeHelper:CancelTimer(self.effect_handle_delay)
    end
    self.temp_list = {}

    self.NabingEffect:SetActive(false)
end

function WorkshopForging:FlushProtectArea()
    self.ProtectArea:SetActive(self.data.forged_flag.flag)
end

function WorkshopForging:CheckAndTryCreateEffect(_index)
    -- for i = 1,5 do 
        local item = Item.New()
        local item_id = self.target_info["stuff_id_".._index] or 0

        if item_id > 0 then item.item_id = item_id end  

        if item_id > 0 and self.temp_list[item_id] == nil then 
            local vo = {}
            vo.temp_num = 0
            vo.real_num = item:Num()
            vo.jump_need_num = self.target_info["stuff_num_" .. _index] or 0
            self.temp_list[item_id] = vo
        end

        if self.temp_list[item_id] and self.temp_list[item_id].temp_num < self.temp_list[item_id].need_num then 
            self.temp_list[item_id].temp_num = self.temp_list[item_id].temp_num + 1

            local show_num = item:Num() - self.temp_list[item_id].temp_num 
            local flag = show_num < self.temp_list[item_id].need_num
            local color =  flag and COLORSTR.Red8 or COLORSTR.White
            if self.NumList[_index] ~= nil and self.NumList[_index].isActiveAndEnabled then 
                UH.SetText(self.NumList[_index], string.format(Language.Common.Cstr,color,  show_num , self.temp_list[item_id].need_num))
            end 

            return false,_index
        else 
            return true,_index
        end 
    -- end 

end

function WorkshopForging:OnClickItem(data)
    self.stuff_tip_list = {}
    local item = nil
    local item_id = 0
    local need_num = 0
    self.target_info = data
    self.seq = data.seq
    self.is_not_enough_stuff = false
    local flag_complete = true
    for i = 1, 6 do
        item = Item.New()
        if i == 6 then
            item_id = data.target
            item.item_id = item_id
            -- UH.SpriteName(self.QuaSpList[i], ForginQua[item:Color()])
            -- self.NoneList[i]:SetActive(true)
            UH.SetIcon(self.IconList[i], item:IconId())
            -- self.IconInteracters[i].Interactable = true
        else
            item_id = data["stuff_id_" .. i] or 0
            need_num = data["stuff_num_" .. i] or 0
            --[[ if i == 5 then
                if data.stuff_id_spe > 0 and data.stuff_num_spe > 0 then
                    item_id = data.stuff_id_spe
                    need_num = data.stuff_num_spe
                end
            end ]]
            if item_id > 0 then
                item.item_id = item_id
                UH.SetText(self.NameList[i], item:Name())
                UH.SetIcon(self.IconList[i], item:IconId())
                self.IconInteracters[i].Interactable = true
                UH.SpriteName(self.QuaSpList[i], ForginQua[item:Color()])
                UH.SetText(self.GetWayList[i],Language.ItemInfo.ItemGetSource[item:Config().get_the_source])
                local flag = item:Num() < need_num
                local color =  flag and COLORSTR.Red8 or COLORSTR.White
                -- UH.Color(self.NumList[i], flag and COLORS.Red8 or COLORS.Green3)
                self.GetWaysObjList[i]:SetActive(item:Config().get_the_source >0)
                UH.SetText(self.NumList[i], string.format(Language.Common.Cstr,color, item:Num(), need_num))
                if flag then 
                    flag_complete = false 
                end 
                self.NumBgList[i]:SetActive(true)
                -- self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
                -- 如果勾了强化
                --if i == 5 and not self.Gou.activeSelf then
                -- self.is_not_enough_stuff = self.is_not_enough_stuff
                --else
                self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
                --end
                if flag == true then
                    table.insert(self.stuff_tip_list, {name = item:Name(), lack = need_num - item:Num()})
                end
                self.QuaList[i]:SetActive(true)
                self.LockList[i]:SetActive(false)
                self.NoneList[i]:SetActive(false)
                self.StuffList[i]:SetActive(true)
                local is_active = self.data:GetAcitveInfo(data.active_seq)
                if data.active_seq ~= 0 and not is_active then
                    -- self.NoneList[i]:SetActive(false)--(not is_active)
                    -- UH.SetText(self.NameList[i], "")
                    -- UH.SetText(self.NumList[i], "")
                    -- self.NumBgList[i]:SetActive(false)
                    -- UH.SetIcon(self.IconList[i], 0)
                    self.LockList[i]:SetActive(true)
                    self.IconInteracters[i].Interactable = false
                end
            else
                UH.SetText(self.NameList[i], "")
                UH.SetText(self.NumList[i], "")
                self.NumBgList[i]:SetActive(false)
                self.QuaList[i]:SetActive(false)
                self.NoneList[i]:SetActive(false)
                self.LockList[i]:SetActive(false)
                self.IconInteracters[i].Interactable = false
                -- UH.SetIcon(self.IconList[i], 0)
                self.GetWaysObjList[i]:SetActive(false)
                self.StuffList[i]:SetActive(false)
            end
        end
    end
    local is_learned
    if data.activation == 0 then
        is_learned = true
    else
        -- 需要配方的物品需要验证是否激活
        is_learned = self.data:GetAcitveInfo(data.active_seq)
    end
    self.Active:SetActive(is_learned)
    self.NoActive:SetActive(not is_learned)
    if is_learned then
        local is_spe = data.stuff_id_spe > 0 and data.stuff_num_spe > 0
        self.QiangHua:SetActive(is_spe)
        self.Single:SetActive(is_spe)
        self.Mulit:SetActive(not is_spe)        
        if is_spe then
            local spe_item = Item.New()
            spe_item.item_id = data.stuff_id_spe
            self.SpeCell:SetData(spe_item)
            UH.SetText(self.SpeName, spe_item:Name())
            UH.SetText(self.SpeNum, string.format("%d/%d", spe_item:Num(), data.stuff_num_spe))
            local flag = spe_item:Num() < data.stuff_num_spe
            UH.Color(self.SpeNum, flag and COLORS.Red8 or COLORS.Green3)
            -- self.Gou:SetActive(true)
            if self.Gou.activeSelf == true and flag then
                self.is_not_enough_stuff = true
                table.insert(self.stuff_tip_list, {name = spe_item:Name(), lack = data.stuff_num_spe - spe_item:Num()})
            end
        end
    end
    if is_learned and flag_complete then 
        self.CanDoAnim:Play()
    end 
    self.send_data.forge_time = 1
    self:OnClickMulitNum(true)

    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)

    self.data:SetForgeMark(data)
end

function WorkshopForging:ChangeStuffNeed()
    self.stuff_tip_list = {}
    self.is_not_enough_stuff = false
    local flag_complete = true
    for i = 1, 6 do
        local item = Item.New()
        if i == 6 then
            local item_id = self.target_info.target
            item.item_id = item_id
            UH.SetIcon(self.IconList[i], item:IconId())
        else
            local item_id = self.target_info["stuff_id_" .. i] or 0
            local need_num = self.target_info["stuff_num_" .. i] and self.target_info["stuff_num_" .. i] * self.send_data.forge_time or 0
            if item_id > 0 then
                item.item_id = item_id
                local flag = item:Num() < need_num
                local color =  flag and COLORSTR.Red8 or COLORSTR.White
                UH.SetText(self.NumList[i], string.format(Language.Common.Cstr,color, item:Num(), need_num))
                if flag then 
                    flag_complete = false 
                end 
                self.NumBgList[i]:SetActive(true)
                self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
                if flag == true then
                    table.insert(self.stuff_tip_list, {name = item:Name(), lack = need_num - item:Num()})
                end
            end
        end
    end

    local is_learned = self.target_info.activation == 0 and true or self.data:GetAcitveInfo( self.target_info.active_seq)

    if is_learned then
        local is_spe = self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0
        if is_spe then
            local spe_item = Item.New()
            spe_item.item_id = self.target_info.stuff_id_spe
            local flag = spe_item:Num() < self.target_info.stuff_num_spe
            if self.Gou.activeSelf == true and flag then
                self.is_not_enough_stuff = true
                table.insert(self.stuff_tip_list, {name = spe_item:Name(), lack = self.target_info.stuff_num_spe - spe_item:Num()})
            end 
        end 
    end 

    if is_learned and flag_complete then 
        self.CanDoAnim:Play()
    end 
end

function WorkshopForging:FlushResult()
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.send_data.forge_time
    UH.SetText(self.Mulit_Cost,need)
    UH.SetText(self.LblCost, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
end
function WorkshopForging:OnClickBtn(data)
    --LogError("data?",data)
    if data.type_id == nil then
        self.forging_data.select_level = data.index
        UH.SetText(self.LblLevel, data.type_name)
    else
        UH.SetText(self.LblProf, data.type_name)
        self.forging_data.select_prof = data.index
    end
    -- self.BtnParent:SetActive(false)
    -- self.ClickAreaShuai:SetActive(false)
    self:OnClickShuaiXuan()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function WorkshopForging:OnClickEquipLevel()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.LevelArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data:GetTypeShow(ComposeGroup.Level))
    else
        self.ProfArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end
function WorkshopForging:OnClickEquipProf()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.ProfArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data:GetTypeShow(ComposeGroup.Forging))
    else
        self.LevelArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopForging:OnClickShuaiXuan()
    if self.ProfArrow.activeSelf then 
        self:OnClickEquipProf()
    elseif self.LevelArrow.activeSelf then 
        self:OnClickEquipLevel()
    end 
end

function WorkshopForging:OnClickStuff(index)
    if self.target_info == nil then
        return
    end
    -- if self.target_info.active_seq > 0 and not self.data:GetAcitveInfo(self.target_info.active_seq) then
    --     return
    -- end
    local data = Item.New()
    data.item_id = index == 5 and self.target_info.stuff_id_spe or self.target_info["stuff_id_" .. index]

    local stuff_num_spe = index == 5 and 1 or self.target_info["stuff_num_" .. index]
    if data.item_id == 0 then
        return
    end
    
    local info = {
        item_list = {data}
    }
    local key = MallData.Instance:GetShopItemConfig(data.item_id)
    if key ~= 0 and stuff_num_spe and Item.GetNum(data.item_id) < stuff_num_spe then
        MallData.JumpItemNum = stuff_num_spe - Item.GetNum(data.item_id)
    end
    ItemInfoCtrl.Instance:ItemInfoView(info)

    local param_t = {
        end_func = function()
            if Item.GetNum(data.item_id) >= stuff_num_spe then 
                ViewMgr:OpenViewByKey(Mod.Workshop.Forging)
            end 
        end 
    }

    LinkMarkData.Instance:MarkIntro(param_t)
end
function WorkshopForging:OnClickTarget()
    local data = Item.Create({item_id = self.target_info.target})
    data.show_key = "workshop_equip"
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopForging:OnClickQiangHua()
    self.Gou:SetActive(not self.Gou.activeSelf)
    self.SpeCell.gameObject:SetActive(self.Gou.activeSelf)
    if self.target_info ~= nil then
        self:OnClickItem(self.target_info)
    end
end

function WorkshopForging:OnClickGet()
    ItemInfoCtrl.Instance:ItemInfoView({show_ways = true,item_list = {Item.Init(self.target_info.activation)}})
    LinkMarkData.Instance:MarkIntro({end_func = function()
        if Item.GetNum(self.target_info.activation) > 0  then 
            ViewMgr:OpenViewByKey(Mod.Bag.Main)
        end 
    end})
end

-- function WorkshopForging:OnClickProtectArea()
--     LogError("here can stop player!")
-- end

function WorkshopForging:OnClickTipsStren()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[21].desc})
end

ForgeCreateEffect = ForgeCreateEffect or BaseClass()
function ForgeCreateEffect:__init(mono, index)
    self.WI = mono
    self.index = index
end
function ForgeCreateEffect:__delete()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
    end
    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_run)
    end
end
function ForgeCreateEffect:Play()
    local time = 0
    local endPos = self.WI.FlyEffectEnd.transform.localPosition
    local startPos = self.WI.EffectStarts[self.index].transform.localPosition
    local crossPos = self.WI.EffectCrosses[self.index].transform.localPosition
    local effectNode = self.WI.FlyEffectNodes[self.index]

    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, 0.5)
    UH.LocalPos(effectNode.transform, startPos)
    -- shoot_curve:TotalTime(20)
    if self.WI.EffectTool.isActiveAndEnabled then 
        self.WI.EffectTool:Play(10010254, effectNode)
    end
    self.handle_run = Runner.Instance:RunUntilTrue(
        function()
            if shoot_curve then
                time = time + Time.deltaTime * 3
                if time >= 1 then
                    -- self.WI.EffectTool:Play(10010255, effectParent)
                    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
                    -- self.handle_delay = TimeHelper:AddDelayTimer(
                    --     function()
                    --         if self.WI.select_item then
                    --             self.WI.select_item:Play()
                    --         end
                    --     end,
                    --     0.2
                    -- )
                    return true
                end
                UH.LocalPos(effectNode.transform, shoot_curve:GetPos(time))
            end
            return false
        end
    )
end

ForgingItem = ForgingItem or DeclareMono("ForgingItem", UIWidgetBaseItem)
function ForgingItem:ForgingItem()
    self.workshop_data = WorkshopData.Instance
end
function ForgingItem:SetData(data)
    local item = Item.Create({item_id = data.target})
    self.EquipCell:SetData(item)
    local level = RoleData.Instance:GetRoleLevel()
    local lock = not self.workshop_data:GetAcitveInfo(data.active_seq)
    if data.activation == 0 then
        lock = false
    end

    -- self.LblActive:SetActive(data.activation ~= 0)
    -- LogError("data????",data.group,data)
    self.ZheZao:SetActive(false)
    if data.group == ComposeGroup.Forging then
        --[[ if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(158,38,0))
        end
        local num = self.workshop_data:CheckCanCompos(data)
        self.red_point:SetNum(num) ]]
        -- 判断配方是否激活
        self.ZheZao:SetActive(lock)
        UH.SetText(self.LblLevel, item:LimitLv()..Language.Common.Level)-- string.format("Lv.%d", item:EquipLevel()))
        UH.Color(self.LblName, lock and COLORS.Gray4 or COLORS.Yellow13)
        UH.Color(self.LblType, lock and COLORS.Gray4 or COLORS.Yellow13)
        UH.Color(self.LblLevel, lock and COLORS.Gray4 or COLORS.Yellow13)
        -- self.EquipCell:MakeGrey(lock)
    elseif data.group == ComposeGroup.Life then
        self.ZheZao:SetActive(lock)
        UH.Color(self.LblName, lock and COLORS.Gray4 or COLORS.Yellow13)
        UH.Color(self.LblType, lock and COLORS.Gray4 or COLORS.Yellow13)
        self.EquipCell:MakeGrey(lock)
    elseif data.group == ComposeGroup.EleCrystal then
    elseif data.group == ComposeGroup.Stone then
    elseif data.group == ComposeGroup.Skill then
    --[[ if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(158,38,0))
        end
        local num = self.workshop_data:CheckSkillNum(data)
        self.red_point:SetNum(num) ]]
    elseif data.group == ComposeGroup.Card then
        local lock = data.level and data.level > RoleData.Instance:GetRoleLevel()
        self.EquipCell:MakeGrey(lock)
        self.ZheZao:SetActive(lock)
        if self.LblLockLevel and lock then 
            self.LblLockLevel.text = lock and string.format(Language.Workshop.Level,data.level) or ""
        end 
    elseif data.group == ComposeGroup.Jewelry then 
        self.ZheZao:SetActive(lock)
        UH.SetText(self.LblLevel, item:LimitLv()..Language.Common.Level)
        UH.Color(self.LblName, lock and COLORS.Gray4 or COLORS.Yellow13)
        UH.Color(self.LblType, lock and COLORS.Gray4 or COLORS.Yellow13)
        UH.Color(self.LblLevel, lock and COLORS.Gray4 or COLORS.Yellow13)
    end
    UH.SetText(self.LblName, item:Name())
    UH.SetText(self.LblType, item:TypeDesc())
    UIWidgetBaseItem.SetData(self, data)
    self:CheckCanForge()
    --检查并显示红点
    self:uncareData()
    self.care_handles = {}
    self:careData(self.workshop_data.remind_block, BindTool.Bind(self.CheckRemind, self))
   
    -- self:CheckRemind()
end
function ForgingItem:CheckRemind()
    if self.red_pos == nil then return end 
    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    local cached_flag = self.workshop_data.cache_item == nil
    local flag = true

    if self.data == nil then return end 

    for i = 1,6 do 
        local item_id = self.data["stuff_id_"..i] or 0
        local need_num = self.data["stuff_num_"..i] or 0

        if item_id > 0 then 
            if Item.GetNum(item_id) < need_num then 
                flag = false 
                break
            elseif Item.GetNum(item_id) >= need_num and item_id == self.workshop_data.cache_item then 
                cached_flag = true
            end 
        end 
    end 

    local block_flag = false
    if self.data.group == ComposeGroup.Forging then
        block_flag = WorkshopData.Instance:GetForgeRemindBlock()
    elseif self.data.group == ComposeGroup.Jewelry then
        block_flag = WorkshopData.Instance:GetJewerlyRemindBlock()
    end

    local role_level = RoleData.Instance:GetRoleLevel()
    local flag_level = true
    if Item.GetLimitLv(self.data.target) < math.floor(role_level/10) * 10 then 
        flag_level = false
    end 

    local role_prof = RoleData.Instance:ProfType()
    local flag_prof = true
    if role_prof ~= RoleData.ProfIdToProfType(Equip.GetLimitProf(self.data.target)) then 
        flag_prof = false
    end 

	self.red_point:SetNum( (flag and not block_flag and flag_level and cached_flag and flag_prof) and 1 or 0 )
end
function ForgingItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
function ForgingItem:CheckCanForge()
    if self.data == nil then return end 

    local is_can_forge = true
    for i = 1,6 do 
        local item_id = self.data["stuff_id_"..i] or 0
        local need_num = self.data["stuff_num_"..i] or 0

        if item_id > 0 then 
            local item = Item.New()
            item.item_id = item_id
            if item:Num() < need_num then 
                is_can_forge = false 
                break
            end 
        end 
    end 

    local is_unlock = true
    if self.data.activation > 0 then 
        is_unlock = self.workshop_data:GetAcitveInfo(self.data.active_seq)
    end 
    
    is_can_forge = self.data.type > 0 and is_can_forge and is_unlock
    if self.CanForge then 
       self.CanForge:SetActive(is_can_forge)
    end 
end

function ForgingItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    self:uncareData()
end 

WorkShopBtnItem = WorkShopBtnItem or DeclareMono("WorkShopBtnItem", UIWidgetBaseItem)
function WorkShopBtnItem:WorkShopBtnItem()
    -- body
end

function WorkShopBtnItem:SetData(data)
    if data.type_id == nil then
        UH.SetText(self.Name, data[2])
        data.type_name = data[2]
        data.index = data[1]
    else
        UH.SetText(self.Name, data.type_name)
        data.index = data.type_id
    end
    UIWidgetBaseItem.SetData(self, data)
end

WorkShopBtnItemD = WorkShopBtnItemD or DeclareMono("WorkShopBtnItemD")
function WorkShopBtnItemD:OnDisable()
    UH.SetText(self.Name, "")
end
