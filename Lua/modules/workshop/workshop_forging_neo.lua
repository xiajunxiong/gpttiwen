WorkshopForgingNeo = WorkshopForgingNeo or DeclareMono("WorkshopForgingNeo", UIWFlushPanel)
function WorkshopForgingNeo:WorkshopForgingNeo()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.lang = Language.WorkShop
    self.forging_data = self.data:ForgingData()

    self.data_cares = {
        {data = self.forging_data, func = self.FlushLeftItem, keys = {"select_prof", "select_level"}, init_call = false},
        -- {data = self.data.remind_block, func = self.FlushLeftItem, keys = {"flush"} , init_call = false},
        {data = BagData.Instance:GetEquipList(), func = self.FlushEquipList},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushEquipList, init_call = false},
        {data = self.forging_data, func = self.AutoCreate, keys = {"auto_create"}, init_call = false},
    }
    self.bag_equip_guide = false
    GuideManager.Instance:RegisterGetGuideUi("ForingItem", function ()
        if self.ItemList:SetupCompleted() then
            local id = GuideData.Instance:GetForingId()
            local data =  self.data:GetSelectById(id)
            if data then
                self.bag_equip_guide = true
                local index = self.ItemList:GetSelectedIndex(data)

                self.ItemList:AutoSlide(data,false)
                return self.ItemList:GetItem(index).gameObject
            else 
                local cfg = self.data:GetForgeComposeCfg(id)
                self.forging_data.select_level = Equip.GetEquipLevel(cfg.target)
                self.forging_data.select_prof = cfg.type
            end
        end
        return nil,nil
    end)

    GuideManager.Instance:RegisterGetGuideUi("StuffCell2", function ()
        return self.MaterialList[2].gameObject
    end)

    self.send_data = {}
    self.stuff_tip_list = {}
    self.flush_left_item_handle = nil

    -- FlyStarEffectCell
    local param_t = {
        mono = self,
    }
    
    for i = 1,4 do 
        self.FlyEffects[i]:SetData(param_t)
    end 

    self:BehaviorSelect()
end 

function WorkshopForgingNeo:BehaviorSelect()
    if GuideManager.Instance:IsGuide() then
        return 
    end 
    local id = self.data:GetBehaviorSelect()

    if id == nil or id == 0 then 
        return 
    end 

    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end

    self.handle_runner = Runner.Instance:RunUntilTrue(function ()
        if self.ItemList:SetupCompleted() then
            local data =  self.data:GetSelectById(id)
            if data then
                self.bag_equip_guide = true
                local index = self.ItemList:GetSelectedIndex(data)

                self.ItemList:AutoSlide(data,false)
                self.ItemList:ClickItem(index)
                return self.ItemList:GetItem(index).gameObject ~= nil
            else 
                local cfg = self.data:GetForgeComposeCfg(id)
                if cfg == nil then
                    BuglyReport("WorkshopForgingNeo:BehaviorSelect CFG Cant Find ID=" .. tostring(id))
                else
                    self.forging_data.select_level = Equip.GetEquipLevel(cfg.target)
                    self.forging_data.select_prof = cfg.type
                end
            end
        end
        return false
    end)

    -- 完成操作后删除缓存
    self.data:SetBehaviorSelect()
end

function WorkshopForgingNeo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.bag_equip_guide then
        GuideManager.Instance:Start(19)
    end

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    if self.forge_handle ~= nil then
        TimeHelper:CancelTimer(self.forge_handle)
        self.forge_handle = nil
    end 

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 

    if self.effdelay then 
        TimeHelper:CancelTimer(self.effdelay)
    end 

    if self.delay_auto_timer then
        TimeHelper:CancelTimer(self.delay_auto_timer)
        self.delay_auto_timer = nil
    end
    
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end

    LinkMarkData.Instance:MidCheck()
    self.data:SetForgeRemindBlock(true)
end 

-- 相当于OpenCallBack
function WorkshopForgingNeo:onFlush()
    self:FlushLeftItem(self.data:GetForgeMark(),true)
end

function WorkshopForgingNeo:OnClickItem(data)
    self.target_info = data

    self.send_data.forge_time = 1
    self.send_data.seq = data.seq
    self.send_data.enough_stuff = false
    self.send_data.enough_coin = false

    self.data:SetForgeMark(data)
    -- 点击侧栏按钮需要全更新
    self:FlushItemInfo()
    self:SetMulitNum(self.send_data.forge_time)
    -- self.data:

    if self.auto_click then 
        self.auto_click = false
    else
        if data.active_seq > 0 and self.data:GetAcitveInfo(data.active_seq) == false then
            PublicPopupCtrl.Instance:Center(Language.Workshop.UnActBlockTips)
        end 
        self.data:SetForgeRemindBlock(true)
    end 

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopForgingNeo:FlushItemInfo()
    self:FlushItemData()
    self:FlushItemNum()
    self:FlushItemOtherInfo()
    self:FlushResult()
end

-- 仅改变数量只需要更新数据
function WorkshopForgingNeo:FlushBagChange()

    if self.data:GetForgeMark()== nil then return end 
    -- self:FlushItemNum()
    self:OnClickMulitNum(true)
    self:FlushItemInfo()
    -- self:OnClickItem(self.data:GetForgeMark())

    self:FlushLeftItem(nil,false)
end

function WorkshopForgingNeo:FlushLeftItem(_mark,is_jump)
    local data = self.data:ForgingShowList()
    self.ItemList:SetData(data)

    local prof_ = self.data:GetTypeShow(ComposeGroup.Forging)
    local level_ = self.data:GetTypeShow(ComposeGroup.Level)
    self.LblScreenProf.text = prof_[self.forging_data.select_prof].type_name
    -- LogError("happoy?",self.forging_data.select_level)
    -- LogError("happoy?2",WorkshopData.Params.MinLevel)
    -- LogError("happoy?3",level_)
    self.LblScreenLevel.text = level_[self.forging_data.select_level - WorkshopData.Params.MinLevel][2]

    if not is_jump and _mark == nil then return end 

    if _mark == nil or _mark.seq == nil then
        self.ItemList:ClickItem(1)
        self.ItemList:JumpVerticalIndex(1,100)
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
        self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
            self.flush_left_item_handle = nil
            self.ItemList:ClickItem(pos_)
            if pos_ <= #data -4 then 
                self.ItemList:JumpVerticalIndex(pos_,100)
            else 
                self.ItemList:JumpVerticalIndex(#data -4,100,38)
            end 
        end,5+math.floor(pos_/self.ItemList.StepCount))
    end 

    self.auto_click = true
end

function WorkshopForgingNeo:FlushItemOtherInfo()
    if self.target_info == nil then return end 
    local is_mulit = Item.GetColor(self.target_info.target) <= ItemColor.Blue-- self.target_info.activation == 0
    local is_learned = is_mulit and true or (self.target_info.active_seq == 0 or (self.target_info.active_seq > 0 and self.data:GetAcitveInfo(self.target_info.active_seq)))
    self.Actived:SetActive(is_learned)
    self.NoActive:SetActive(not is_learned)

    self.Single:SetActive(not is_mulit)
    self.Mulit:SetActive(is_mulit)   

    if is_learned and self.send_data.enough_stuff then 
        self.CanDoAnim:Play()
    end 

    self.blaze_tips:SetActive( Equip.CanBlazeStar(self.target_info.target ))
end

function WorkshopForgingNeo:FlushItemData()
    if self.target_info == nil then return end 
    self.send_data.stuff_num = 0
    for i = 1, 4 do
        local vo = {}
        local _item = Item.New({item_id = self.target_info["stuff_id_"..i]}) or 0
        vo.item_info = _item
        vo.active_seq = self.target_info.active_seq
        self.MaterialList[i]:SetData(vo)
        local plus =  self.target_info["stuff_id_"..i] > 0 and 1 or 0 
        self.send_data.stuff_num = self.send_data.stuff_num + plus
    end 
    
    -- 简单的归位逻辑
    local flag_scroll = self.target_info.stuff_id_4 ~= 0 
    self.MateScroll.enabled = flag_scroll
    local pos = self.MateContent.localPosition
    pos.x = 0
	self.MateContent.localPosition = pos
end

function WorkshopForgingNeo:FlushItemNum()

    if self.target_info == nil then return end 
    for i = 1, 4 do
        local need_num = self.target_info["stuff_num_"..i]
        self.MaterialList[i]:FlushStuffNum(need_num,self.send_data.forge_time,true)
    end 
    
    self.qianghua:SetActive(self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0)
    local stren_item = Item.New()
    stren_item.item_id = self.target_info.stuff_id_spe
    self.StrenCell:SetData(stren_item)
    UH.SetText(self.StrenNum, string.format("%d/%d", stren_item:Num(), self.target_info.stuff_num_spe))
    local flag = stren_item:Num() < self.target_info.stuff_num_spe
    UH.Color(self.StrenNum, flag and COLORS.Red8 or COLORS.Green3)

    self:CheckStuffNeed()
end

function WorkshopForgingNeo:CheckStuffNeed()
    self.send_data.enough_stuff = true

    self.stuff_tip_list = {}
    for i = 1,4 do 
        local is_enough,lack_num = self.MaterialList[i]:IsEnough()
        if not is_enough then 
            table.insert(self.stuff_tip_list, 
            {name = self.MaterialList[i].data.item_info:Name(), 
            lack = lack_num,
            item = self.MaterialList[i].data.item_info})
            self.send_data.enough_stuff = false
        end 
    end 

    local is_spe = self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0
    if is_spe then 
        local spe_item = Item.New()
        spe_item.item_id = self.target_info.stuff_id_spe
        local lack_num = self.target_info.stuff_num_spe - spe_item:Num()
        if self.Gou.activeSelf and lack_num > 0 then 
            table.insert(self.stuff_tip_list, {name = spe_item:Name(), lack = lack_num,item = spe_item})
            self.send_data.enough_stuff = false
        end 
    end 
end

function WorkshopForgingNeo:FlushResult()
    if self.target_info == nil then return end 
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.send_data.forge_time
    local color = coin < need and COLORS.Red8 or COLORS.Green1
    UH.SetText(self.MulitCost,need)
    UH.SetText(self.LblCost, need)
    -- UH.Color(self.LblCost, color)
    -- UH.Color(self.MulitCost, color)
    self.send_data.enough_coin = coin < need
end 

function WorkshopForgingNeo:FlushEquipList()
    local list = BagData.Instance:GetEquipList()
    for i = 0, 4 do
        self.EquipedShows[i + 1]:SetData(list[i]~=nil and list[i] or Item.New())
    end 
end

function WorkshopForgingNeo:ShowCreateEffectEffect()
    local lenght_time = 4
    local lenght_wait = 4
    local wait_time = 0.01
    local effect_time = 0.05
    local stuff_complete = {}

    self.effect_handle_run = Runner.Instance:RunUntilTrue(
        function()
            effect_time = effect_time + Time.deltaTime
            if effect_time >= wait_time then
                lenght_time = lenght_time + 1
                if lenght_time >= lenght_wait and #stuff_complete < self.send_data.stuff_num then 
                    lenght_time = 0

                    local operate_index = #stuff_complete + 1
                    self.FlyEffects[operate_index]:Play("10010254")

                    stuff_complete[operate_index] = {
                        effect_fly = true,
                        cur_num = Item.GetNum(self.target_info["stuff_id_"..operate_index]),
                        total_num = self.target_info["stuff_num_"..operate_index] * self.send_data.forge_time,
                        cost_num = 0,
                        num_complete = false
                    }

                    if #stuff_complete == self.send_data.stuff_num then
                        self:StartToShowCreating()
                    end  
                end 

                local complete_count = 0
                for k,v in pairs(stuff_complete) do 
                    if v.effect_fly and not v.num_complete then 
                        v.cur_num = v.cur_num - 1 
                        v.cost_num = v.cost_num + 1
                        local color = v.cur_num >= v.total_num and COLORSTR.White or COLORSTR.Red8
                        self.MaterialList[k].lblNum.text = string.format(Language.Common.Cstr,color, v.cur_num, v.total_num)

                        v.num_complete = v.cost_num == v.total_num
                    end 
                    if v.num_complete then 
                        complete_count = complete_count + 1
                    end 
                end 

                effect_time = 0
                return (complete_count == #stuff_complete) and #stuff_complete == self.send_data.stuff_num
            end
        end
    )
end 

function WorkshopForgingNeo:StartToShowCreating()
    self.EffectTool:Play("10010315")

    if self.forge_handle ~= nil then
        TimeHelper:CancelTimer(self.forge_handle)
        self.forge_handle = nil
    end 


    self.forge_handle = TimeHelper:AddDelayFrameTimer(function()
        self.forge_handle = nil 
        self.ForgingProg:SetActive(true)
        self.NabingEffect:SetActive(true)
        self.HanmaShow:SetActive(false)
        self.prog_step = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateCreateProgEffect, self), 0.1)
    end ,5)
end

function WorkshopForgingNeo:UpdateCreateProgEffect()
    if self.prog_step == nil then return end 
    local value_prog = self.ProgSlider.value
    local cur_prog = math.floor(value_prog * 100)
    if not self.ProText:Equals(nil)  then 
        UH.SetText(self.ProText,cur_prog.."%")
    end 
end

function WorkshopForgingNeo:CanelCreateEffect()

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
    -- self.WaitNabe:SetActive(true)
    local forge_type = ( Item.GetColor(self.target_info.target) > ItemColor.Blue ) and WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ or WORLSHOP_OP.WORKSHOP_OP_BATCH_COMP_REQ

    if self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0 then
        self.ctrl:SendOperateReq(
            forge_type,
            self.send_data.seq,
            self.send_data.forge_time and self.send_data.forge_time or 1,
            self.Gou.activeSelf and 1 or 0
        )
    else
        self.ctrl:SendOperateReq(
            forge_type, 
            self.send_data.seq, 
            self.send_data.forge_time and self.send_data.forge_time or 1,
            0)
    end
    
    
    self.data:SetForgeRemindBlock(true)
    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end
    if self.effect_handle_delay then
        TimeHelper:CancelTimer(self.effect_handle_delay)
    end
    if self.forge_handle then
        TimeHelper:CancelTimer(self.forge_handle)
    end 
    self.temp_list = {}
    self.effdelay = TimeHelper:AddDelayTimer(function ()
        if self.ProtectArea ~= nil then 
            self.ProtectArea:SetActive(false)
        end

        for i = 1,4 do 
            self.FlyEffects[i]:AllHandleCancel()
        end 
    end,0.5)
    self.HanmaShow:SetActive(true)

    self.NabingEffect:SetActive(false)
    self.ForgingProg:SetActive(false)
    self.ProgSlider.value = 0
end

function WorkshopForgingNeo:OnClickMulit()

end 

function WorkshopForgingNeo:AutoCreate()
    if self.forging_data.auto_create then 
        self.delay_auto_timer = TimeHelper:AddDelayTimer(function()
            self.ProtectArea:SetActive(true)
            self:ShowCreateEffectEffect()
            AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
        end,0.2)
        self.forging_data.auto_create = false
    end 
end

function WorkshopForgingNeo:OnClickCreate()
    if not self.target_info then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    if self.send_data.enough_coin then
        PublicPopupCtrl.Instance:Error(ErrorText[21])
        return
    end
    if not self.send_data.enough_coin and #self.stuff_tip_list > 0 then
        local single_pass = -1
        local warn_pos = {}
        -- 先检查一次声望限制
        for k,v in pairs(self.stuff_tip_list) do 
            local shop_data = self.data:CatchForgeBuyWay(v.item)
            local is_require_limit = shop_data ~= nil and ReputationCtrl.IsRequire(shop_data.require_type,shop_data.require_value)
            local is_limit = shop_data ~= nil and shop_data.relation and (PrestigeData.Instance:GetPrestigeLevel(shop_data.village_id) < shop_data.relation)
            
            if (is_limit or not is_require_limit) and shop_data ~= nil then 
                single_pass = k 
                break
            end 
            if shop_data == nil then 
                table.insert(warn_pos,k)
            end 
        end 

        -- 只有一个或者有声望限制则走单一逻辑
        if #self.stuff_tip_list == 1 or single_pass > -1 then
            local operate_pos = self.stuff_tip_list[1]
            operate_pos.item.vo.jump_need_num = operate_pos.lack
            ViewMgr:OpenView(GetWayView, {item = operate_pos.item})
            BagData.Instance:SetGetWaySweepMark({need = operate_pos.lack})

            if single_pass > -1 then 
                PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.QuickForgeBuyReputationLack,self.stuff_tip_list[single_pass].item:Name()))
            else 
                for _, v in ipairs(self.stuff_tip_list) do
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], v.name, v.lack))
                end
            end 
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        elseif #self.stuff_tip_list > 1 then
            local buy_list = {}
            for k,v in pairs(self.stuff_tip_list) do
                local is_warn = false
                for i,j in pairs(warn_pos)do 
                    if j ==k then 
                        is_warn = true
                    end 
                end 
                if not is_warn then table.insert(buy_list,v) end 
            end 
            if #buy_list > 0 then 
                ViewMgr:OpenView(WorkshopQuickForgeBuy, {item_list = buy_list,target = self.target_info.target})
            else 
                for _, v in ipairs(self.stuff_tip_list) do
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], v.name, v.lack))
                end
            end 
        end
        return
    end
    if not BagData.Instance:TryFindEmptyInTempBag() then 
        PublicPopupCtrl.Instance:Error(ErrorText[100])
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
                if self.ProtectArea then 
                    self.ProtectArea:SetActive(true)  
                    self:ShowCreateEffectEffect()
                end
                AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
            end 
        },
        tnr = DTTodayNotRemind.WorkshopForge
    }

    if GuideManager.Instance:IsGuide() then 
        -- self.ForgingAnim:SetTrigger(APH("open"))
        self.ProtectArea:SetActive(true)
        self:ShowCreateEffectEffect()
        AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
        return 
    end 
    
    PublicPopupCtrl.Instance:DialogTips(param_t)

end 

function WorkshopForgingNeo:OnClickEquipLevel()
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

function WorkshopForgingNeo:OnClickEquipProf()
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

function WorkshopForgingNeo:OnClickShuaiXuan()
    if self.ProfArrow.activeSelf then 
        self:OnClickEquipProf()
    elseif self.LevelArrow.activeSelf then 
        self:OnClickEquipLevel()
    end 
end

function WorkshopForgingNeo:OnClickTipsStren()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[21].desc})
end

function WorkshopForgingNeo:OnClickMulitNum(is_pop)
    if self.MulitNum.text ~= "" then
        local num = tonumber(self.MulitNum.text)
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
            self.send_data.forge_time = self.send_data.limited_time
        end 

        self.MulitNum.text = self.send_data.forge_time

        self:FlushItemNum()
        self:FlushResult()
    end 
end

-- 组件的回调可以自然适应修改，无须再敲
function WorkshopForgingNeo:OnClickBtnMulitNum(_num)
    local test_num = self.send_data.forge_time + _num
    if self.MulitNum then self.MulitNum.text = test_num end 
end

-- 内部接口
function WorkshopForgingNeo:SetMulitNum(_num)
    self.MulitNum.text = _num
end

function WorkshopForgingNeo:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickBtnMulitNum(num)
        end
	end,0.1)
end

function WorkshopForgingNeo:OnClickUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function WorkshopForgingNeo:MulitLimitedCheck(num)
    self.send_data.limited_time = 1
    
    for i = 1, 4 do
        local stuff_num = self.MaterialList[i]:GetLimitedTime()
        
        if stuff_num > 0 and stuff_num < num then 
            self.send_data.limited_time = stuff_num
            return false
        end 
    end

    
    return true
end

function WorkshopForgingNeo:OnClickTarget()
    local data = Item.Create({item_id = self.target_info.target})
    data.show_key = "workshop_equip"
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopForgingNeo:OnClickBtn(data)
    if data.type_id == nil then
        self.forging_data.select_level = data.index 
        UH.SetText(self.LblScreenLevel, data.type_name)
    else
        UH.SetText(self.LblScreenProf, data.type_name)
        self.forging_data.select_prof = data.index
    end
    -- self.BtnParent:SetActive(false)
    -- self.ClickAreaShuai:SetActive(false)
    self:OnClickShuaiXuan()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function WorkshopForgingNeo:OnClickQiangHua()
    self.Gou:SetActive(not self.Gou.activeSelf)
    self.StrenCell.gameObject:SetActive(self.Gou.activeSelf)
    self:CheckStuffNeed()
    -- self:FlushItemInfo()
    -- -- if self.target_info then
    -- --     self:OnClickItem(self.target_info)
    -- -- end
end

function WorkshopForgingNeo:OnClickGet()
    -- ItemInfoCtrl.Instance:ItemInfoView({show_ways = true,item_klist = {Item.Init(self.target_info.activation)}})
    MainOtherCtrl.Instance:GetWayView({item = Item.Init(self.target_info.activation)})
    LinkMarkData.Instance:MarkIntro({end_func = function()
        if Item.GetNum(self.target_info.activation) > 0  then 
            ViewMgr:OpenViewByKey(Mod.Bag.Main)
        end 
    end})
end