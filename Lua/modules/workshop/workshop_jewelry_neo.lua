WorkshopJewelryNeo = WorkshopJewelryNeo or DeclareMono("WorkshopJewelryNeo", UIWFlushPanel)
function WorkshopJewelryNeo:WorkshopJewelryNeo()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.lang = Language.WorkShop
    self.jewelry_data = self.data:JewerlyData()
    self.data_cares = {
        {data = self.jewelry_data, func = self.FlushLeftItem, keys = {"select_prof", "select_level"}, init_call = false},
        -- {data = self.data.remind_block, func = self.FlushLeftItem,keys = {"flush"} , init_call = false},
        {data = BagData.Instance:GetJewelryList(), func = self.FlushJewelryList},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushJewelryList, init_call = false},
        {data = self.jewelry_data, func = self.AutoCreate, keys = {"auto_create"}, init_call = false},
    }

    self.send_data = {}
    self.stuff_tip_list = {}

    local param_t = {
        mono = self,
    }
    
    for i = 1,4 do 
        self.FlyEffects[i]:SetData(param_t)
    end 


    self.ForgingAnim:ListenEvent("show", function(eve_param,anim_st_info)
        if "end" == eve_param then
            -- self:EndForgeEffectHandle()
            self:ShowCreateEndEffect()
        end
    end)

    self.ForgingAnim:ListenEvent("wait", function(eve_param,anim_st_info)
        if "end" == eve_param then
            self.nabe_show:SetActive(true)
            self.effectObj:SetActive(false)
            self.ProtectArea:SetActive(false)

            for i = 1,4 do 
                self.FlyEffects[i]:AllHandleCancel()
            end 
        end
    end)
    
    
    self.ForgingAnim:ListenEvent("anim_state",function (eve_param,anim_st_info)
            if eve_param == "complete" then 
            end 
        end)

    if GuideManager.Instance:IsGuide() then 
        self.data.jewelry_data.select_level = self.data:GetJewerlyShowLevel(30)
    end 
end 

function WorkshopJewelryNeo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end

    if self.delay_auto_timer then
        TimeHelper:CancelTimer(self.delay_auto_timer)
        self.delay_auto_timer = nil
    end
    LinkMarkData.Instance:MidCheck()
    self.data:SetJewerlyRemindBlock(true)
end 

-- 相当于OpenCallBack
function WorkshopJewelryNeo:onFlush()
    self:FlushLeftItem(self.data:GetJewerlyMark(),true)
end

function WorkshopJewelryNeo:OnClickItem(data)
    if self.auto_click then 
        self.auto_click = false
    else 
        if data.active_seq > 0 and self.data:GetAcitveInfo(data.active_seq) == false then
            PublicPopupCtrl.Instance:Center(Language.Workshop.UnActBlockTips)
        end 
        self.data:SetJewerlyRemindBlock(true)
    end 

    if data == self.target_info then return end 
    self.target_info = data

    self.send_data.forge_time = 1
    self.send_data.seq = data.seq
    self.send_data.enough_stuff = false
    self.send_data.enough_coin = false

    self.data:SetJewerlyMark(data)
    -- self.data:SetJewerlyRemindBlock(true)
    -- 点击侧栏按钮需要全更新
    self:FlushItemInfo()
    self:SetMulitNum(self.send_data.forge_time)
end

function WorkshopJewelryNeo:FlushItemInfo()
    self:FlushItemData()
    self:FlushItemNum()
    self:FlushItemOtherInfo()
    self:FlushResult()
end

-- 仅改变数量只需要更新数据
function WorkshopJewelryNeo:FlushBagChange()
    -- LogError("FlushBagChange")
    if self.data:GetJewerlyMark()== nil then return end 
    -- self:FlushItemNum()
    self:OnClickMulitNum(true)
    self:FlushItemInfo()
    -- self:OnClickItem(self.data:GetJewerlyMark())

    self:FlushLeftItem(nil,false)
end

function WorkshopJewelryNeo:FlushLeftItem(_mark,is_jump)
    local data = self.data:GetJweelryShowList()
    self.ItemList:SetData(data)

    local prof_str,level_str = self.data:GetJewerlyTypeShow()
    self.LblScreenProf.text = prof_str
    self.LblScreenLevel.text = level_str

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
        self.ItemList:ClickItem(pos_)
        if self.flush_left_item_handle ~= nil then
            TimeHelper:CancelTimer(self.flush_left_item_handle)
            self.flush_left_item_handle = nil
        end
        self.flush_left_item_handle = TimeHelper:AddDelayTimer(function()
            self.flush_left_item_handle = nil
            if pos_ <= #data -4 then 
                self.ItemList:JumpVerticalIndex(pos_,100)
            else 
                self.ItemList:JumpVerticalIndex(#data -4,100,38)
            end 
        end,0.5)
    end 

    self.auto_click = true
end

function WorkshopJewelryNeo:FlushItemOtherInfo()

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

function WorkshopJewelryNeo:FlushItemData()
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

function WorkshopJewelryNeo:FlushItemNum()
    if self.target_info == nil then return end 
    for i = 1, 4 do
        local need_num = self.target_info["stuff_num_"..i]
        self.MaterialList[i]:FlushStuffNum(need_num,self.send_data.forge_time,true)
    end 

    local can_stren = self.target_info.stuff_id_spe > 0 and self.target_info.stuff_num_spe > 0
    self.strengDesk:SetActive(can_stren)
    self.nomalDesk:SetActive(not can_stren)
    local stren_item = Item.New()
    stren_item.item_id = self.target_info.stuff_id_spe
    self.qianghuaCell:SetData(stren_item)
    UH.SetText(self.qianghuaNum, string.format("%d/%d", stren_item:Num(), self.target_info.stuff_num_spe))
    local flag = stren_item:Num() < self.target_info.stuff_num_spe
    UH.Color(self.qianghuaNum, flag and COLORS.Red8 or COLORS.Green3)

    self:CheckStuffNeed()
end

function WorkshopJewelryNeo:CheckStuffNeed()
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
        if self.qianghuaCheck.activeSelf and lack_num > 0 then 
            table.insert(self.stuff_tip_list, {name = spe_item:Name(), lack = lack_num,item = spe_item})
            self.send_data.enough_stuff = false
        end 
    end 
end

function WorkshopJewelryNeo:FlushResult()
    if  self.target_info == nil then return end 
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.send_data.forge_time
    local color = coin < need and COLORS.Red8 or COLORS.Green1
    UH.SetText(self.MulitCost,need)
    UH.SetText(self.LblCost, need)
    -- UH.Color(self.LblCost, color)
    -- UH.Color(self.MulitCost, color)
    self.send_data.enough_coin = coin < need
end 

function WorkshopJewelryNeo:FlushJewelryList()
    local list = BagData.Instance:GetJewelryList()
    for i = 0, 2 do
        self.EquipedShows[i + 1]:SetData(list[i]~=nil and list[i] or Item.New())
    end 
end

function WorkshopJewelryNeo:ShowCreateEndEffect()
    self.ForgingProg:SetActive(false)
    if self.prog_step then TimeHelper:CancelTimer(self.prog_step) end 
    self.ForgingAnim:SetTrigger(APH("close"))

    if self.target_info == nil then 
        LogError("target_info lost!")
        return 
    end 

    if self.send_data == nil then 
        LogError("send_data lost!")
        return 
    end 

    if  Item.GetColor(self.target_info.target) > ItemColor.Blue then -- self.target_info.activation > 0 then 
        self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_MAKE_JEWELTY,
        self.send_data.seq,
        1,
        self.qianghuaCheck.activeSelf and 1 or 0)
    else 
        self.ctrl:SendOperateReq(
            WORLSHOP_OP.WORKSHOP_OP_BATCH_COMP_REQ, 
            self.send_data.seq, 
            self.send_data.forge_time and self.send_data.forge_time or 0,
            0)
    end

    self.data:SetJewerlyRemindBlock(true)

    if self.effect_handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.effect_handle_run)
        self.effect_handle_run = nil
    end

    self.ProgSlider.value = 0
    -- self.ctrl:SendOperateReq(
    --     WORLSHOP_OP.WORKSHOP_OP_MAKE_JEWELTY,
    --     self.send_data.seq,
    --     self.send_data.forge_time and self.send_data.forge_time or 0,
    --     0
    -- )
end 

function WorkshopJewelryNeo:ShowCreateStartEffect()
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

function WorkshopJewelryNeo:StartToShowCreating()
    self.effectObj:SetActive(true)
    self.ForgingAnim:SetTrigger(APH("open"))
    self.ProtectArea:SetActive(true)
    self.nabe_show:SetActive(false)
    self:ShowForgeProgStep()
end

function WorkshopJewelryNeo:OnClickMulit()

end 

function WorkshopJewelryNeo:AutoCreate()
    if self.jewelry_data.auto_create then 
        self.delay_auto_timer = TimeHelper:AddDelayTimer(function()
            self.ProtectArea:SetActive(true)
            self:ShowCreateStartEffect() 
            AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
        end,0.2)
        self.jewelry_data.auto_create = false
    end 
end 

function WorkshopJewelryNeo:OnClickCreate()
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
            
            if is_limit or not is_require_limit then 
                single_pass = k 
                break
            end 

            if shop_data == nil then 
                table.insert(warn_pos,k)
            end 
        end 


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
                ViewMgr:OpenView(WorkshopQuickForgeBuy, {item_list = self.stuff_tip_list,target = self.target_info.target})
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
                if ViewMgr:IsOpen(Workshop) then 
                    self:ShowCreateStartEffect()
                end 
                AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
            end 
        },
        tnr = DTTodayNotRemind.WorkshopForge
    }

    if GuideManager.Instance:IsGuide() then 
        self:StartToShowCreating()
        AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
        return 
    end 
    
    PublicPopupCtrl.Instance:DialogTips(param_t)

end 

function WorkshopJewelryNeo:ShowForgeProgStep()
    self.ForgingProg:SetActive(true)
    self.prog_step = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateCreateProgEffect, self), 0.1)
end

function WorkshopJewelryNeo:UpdateCreateProgEffect()
    if self.prog_step == nil then return end 
    local value_prog = self.ProgSlider.value
    local cur_prog = math.floor(value_prog * 100)
    if not self.ProText:Equals(nil)  then 
        UH.SetText(self.ProText,cur_prog.."%")
    end 
end


function WorkshopJewelryNeo:OnClickEquipLevel()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.LevelArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data:GetJewerlyLevelShowList())
    else
        self.ProfArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopJewelryNeo:OnClickEquipProf()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.ProfArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data:GetTypeShow(ComposeGroup.Jewelry))
    else
        self.LevelArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopJewelryNeo:OnClickShuaiXuan()
    if self.ProfArrow.activeSelf then 
        self:OnClickEquipProf()
    elseif self.LevelArrow.activeSelf then 
        self:OnClickEquipLevel()
    end 
end

function WorkshopJewelryNeo:OnClickTipsStren()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[82].desc})
end

function WorkshopJewelryNeo:OnClickMulitNum(is_pop)
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
function WorkshopJewelryNeo:OnClickBtnMulitNum(_num)
    local test_num = self.send_data.forge_time + _num
    self.MulitNum.text = test_num
end

-- 内部接口
function WorkshopJewelryNeo:SetMulitNum(_num)
    self.MulitNum.text = _num
end

function WorkshopJewelryNeo:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickBtnMulitNum(num)
        end
	end,0.1)
end

function WorkshopJewelryNeo:OnClickUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function WorkshopJewelryNeo:MulitLimitedCheck(num)
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

function WorkshopJewelryNeo:OnClickTarget()
    local data = Item.Create({item_id = self.target_info.target})
    data.show_key = "workshop_equip"
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopJewelryNeo:OnClickBtn(data)
    if data.type_id == nil then
        self.jewelry_data.select_level = data.index 
        UH.SetText(self.LblScreenLevel, data.type_name)
    else
        UH.SetText(self.LblScreenProf, data.type_name)
        self.jewelry_data.select_prof = data.index
    end
    -- self.BtnParent:SetActive(false)
    -- self.ClickAreaShuai:SetActive(false)
    self:OnClickShuaiXuan()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function WorkshopJewelryNeo:OnClickQiangHua()
    self.qianghuaCheck:SetActive(not self.qianghuaCheck.activeSelf)
    self.qianghuaCell.gameObject:SetActive(self.qianghuaCheck.activeSelf)
    self:CheckStuffNeed()
end

function WorkshopJewelryNeo:OnClickGet()
    -- ItemInfoCtrl.Instance:ItemInfoView({show_ways = true,item_list = {Item.Init(self.target_info.activation)}})
    local activation = self.target_info.activation
    MainOtherCtrl.Instance:GetWayView({item = Item.Init(activation)})
    LinkMarkData.Instance:MarkIntro({end_func = function()
        if Item.GetNum(activation) > 0 then 
            ViewMgr:OpenViewByKey(Mod.Bag.Main)
        end 
    end})
end

EquipForgeStuffCell = EquipForgeStuffCell or DeclareMono("EquipForgeStuffCell", UIWidgetBaseItem)
function EquipForgeStuffCell:EquipForgeStuffCell()
    self.workshop_data = WorkshopData.Instance
end

function EquipForgeStuffCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = self.data.item_info

    local stuff_with = item.item_id > 0
    if stuff_with then 
        self.Name.text = item:Name()
        UH.SetIcon(self.Icon, item:IconId())
        UH.SpriteName(self.QuaImg, ForginQua[item:Color()])

        if self.GetWay then 
            self.GetWay:SetActive(item:Config().get_the_source > 0)
            self.lblGetWay.text = Language.ItemInfo.ItemGetSource[item:Config().get_the_source]
        end 

        local is_active = self.workshop_data:GetAcitveInfo(self.data.active_seq)
        local flag_lock = data.active_seq > 0 and not is_active
        self.Lock:SetActive(flag_lock)
        self.IconInteracter.Interactable = not flag_lock        
    end 
    
    self.QuaObj:SetActive(stuff_with)
    self.Total:SetActive(stuff_with)

    self.is_init = true
end 

function EquipForgeStuffCell:FlushStuffNum(need_num,forge_num)
    self.data.need_num = need_num
    self.data.forge_num = forge_num

    local flag_enough,lack_num = self:IsEnough()
    local color = flag_enough and COLORSTR.White or COLORSTR.Red8
    self.lblNum.text = string.format(Language.Common.Cstr,color, self.data.item_info:Num(), self.data.need_num * self.data.forge_num)
end

function EquipForgeStuffCell:IsEnough()
    local lack_num = -1
    local need_num = self.data.need_num * self.data.forge_num
    local is_enough = self.data.item_info:Num() >= need_num
    if not is_enough then 
        lack_num = need_num - self.data.item_info:Num()
    end 
    return is_enough,lack_num
end

function EquipForgeStuffCell:GetLimitedTime()
    if not self:IsRealStuff() then return -1 end 

    local limited_num = math.floor(self.data.item_info:Num() / self.data.need_num )
    limited_num = limited_num > 0 and limited_num or 1
    return limited_num
end 

function EquipForgeStuffCell:IsRealStuff()
    return self.is_init and self.data.item_info.item_id > 0
end

function EquipForgeStuffCell:OnClickStuff(index)
    if self.data == nil then
        return
    end
    if self.data.item_info.item_id == 0 then 
        return 
    end 
    local is_enough,lack_num = self:IsEnough()
    if MallData.Instance:GetShopItemConfig(self.data.item_info.item_id) ~= 0 and not is_enough then 
        MallData.JumpItemNum = lack_num
    end
    ItemInfoData.Instance:SetClickDataPos(UH.GetRootPosition(self.gameObject))
    ItemInfoCtrl.Instance:ItemInfoView({item_list = {self.data.item_info},show_ways = true})
    --MainOtherCtrl.Instance:GetWayView({item = self.data.item_info})
    local jump_mark = self.JumpType
    LinkMarkData.Instance:MarkIntro({
        end_func = function()
            if Item.GetNum(self.data.item_info.item_id) >= self.data.need_num * self.data.forge_num then 
                if jump_mark == 1 then 
                    ViewMgr:OpenViewByKey(Mod.Workshop.Forging)
                elseif jump_mark == 0 then 
                    ViewMgr:OpenViewByKey(Mod.Workshop.Jewelry)
                end 
                
            end 
        end 
    })
end

EquipedForegeShowCell = EquipedForegeShowCell or DeclareMono("EquipedForegeShowCell", UIWidgetBaseItem)

function EquipedForegeShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    local with_data = data.item_id > 0 
    self.LevelShow:SetActive(with_data)

    self.desc:SetActive(not with_data)
    local tisheng = WorkshopData.Instance:CanUlti(data) and FunOpen.Instance:GetFunIsOpened(Mod.Equip.Ulti)
    if self.TiSheng then self.TiSheng:SetActive(tisheng) end 
    if with_data then 
        local str = data:Level()..Language.Common.Level
        if data:Color() == ItemColor.Star or data:Color() == ItemColor.God then
            -- 神装装备后左上角
            local equip_level = DivineCostumeData.Instance:GetChangeShowLevel(data)
            str = equip_level .. Language.Common.Level
        end
        self.LevelText.text = str
    end 
end 

function EquipedForegeShowCell:OnClickUlti()
    WorkshopData.Instance:SetSelectUltiData(self.data)
	ViewMgr:OpenViewByKey(Mod.Equip.Ulti)
end

WorkShopJBtnItem = WorkShopJBtnItem or DeclareMono("WorkShopJBtnItem", UIWidgetBaseItem)

function WorkShopJBtnItem:SetData(data)
    if data.type_id == nil then
        UH.SetText(self.Name, data.name)
        data.type_name = data.name
        data.index = data.level
    else
        UH.SetText(self.Name, data.type_name)
        data.index = data.type_id
    end
    UIWidgetBaseItem.SetData(self, data)
end