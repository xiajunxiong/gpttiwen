local equip_type_sp = {
    [0] = "TouKui",
    [1] = "KaiJia",
    [2] = "FaQi",
    [3] = "DunPai",
    [4] = "XueZi",
    [5] = "JieZhi",
    [6] = "ShouZhuo",
    [7] = "DiaoZhui",
}

WorkshopUpgrade = WorkshopUpgrade or DeclareMono("WorkshopUpgrade", UIWFlushPanel)
function WorkshopUpgrade:WorkshopUpgrade()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushBagChange},
        {data = BagData.Instance.equip_sort_upgrade, func = self.FlushBagChange, init_call = false},
    }
    self.send_data = {}
    self.target_info = nil
    self.init_fly = false
    self.cur_suit_light = 0
    self.level_mark = 0
    self.break_mark = 0

    -- self.stuff_list:SetCompleteCallBack(function()
    --     self:CheckStuffNeed()
    -- end )

    for i = 1,4 do 
        self.stuff_fly_list[i]:SetData({
            mono = self,
            -- start_effect = "2161012",
            -- pre_pause = 0,
            end_func = i == 1 and BindTool.Bind1(self.EquipShowComplete, self) or nil,
            end_func_time = 0  
        })
    end 

    self.suit_list:SetCompleteCallBack(function()
        self.cur_suit_light = WorkshopData.Instance:CheckAllEquipUpGradeMark()
        if not self.init_fly and self.cur_suit_light ~= nil and self.cur_suit_light > 0 then 
            self.suit_list:AutoSlide(self.suit_list:GetItem(self.cur_suit_light):GetData(),false)
            self.init_fly = true
        end 
    end)

    GuideManager.Instance:RegisterGetGuideUi("UpgradeWeaponSelect", function ()
        if self.equip_list and self.equip_list[3] then 
            return self.equip_list[3]
        end 
        return nil,nil
    end)
end

function WorkshopUpgrade:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i = 1,8 do 
        self.equip_list[i]:RedDelete()
    end 

    if self.Complete_Timer ~= nil then
        TimeHelper:CancelTimer(self.Complete_Timer)
        self.Complete_Timer = nil
    end 
end 

function WorkshopUpgrade:onFlush()
    local simple_flag = false
    for i = 1,8 do 
        if not self.equip_list[i]:GetData().is_empty then 
            self.equip_list[i]:Click()
            simple_flag = true
            break
        end 
    end 
    if not simple_flag then 
        self.equip_list[1]:Click() 
    end 
end 

function WorkshopUpgrade:EquipShowComplete()
    local cur_cfg,next_cfg = WorkshopData.Instance:GetEquipUpgradeCfgByType(self.target_info.equip_type)
    local cur_break = BagData.Instance:GetEquipSortStrengthen(self.target_info.equip_type)
    local is_max = next_cfg == nil
    local is_need_break = not is_max and next_cfg.grade ~= cur_break and next_cfg.grade > 0 

    self.EffectTool:Play(2161013)
    self:CompleteBlust()
    for i = 1,4 do 
	    self.stuff_fly_list[i]:AllHandleCancel()
	end 
end

function WorkshopUpgrade:FlushRight()
    if self.target_info == nil then return end 

    self.equip_cell:SetData(self.target_info.equip)
    local cur_cfg,next_cfg = WorkshopData.Instance:GetEquipUpgradeCfgByType(self.target_info.equip_type)
    local cur_break_cfg,new_break_cfg = WorkshopData.Instance:GetEquipUpgradeBreakCfgByType(self.target_info.equip_type)
    UH.SetText(self.Level,(cur_cfg == nil and 0 or "+"..cur_cfg.hole_level))

    -- 刷新升级
    -- 先完成特效展示
    local old_level = self.level_mark
    local old_break = self.level_break
    self.level_mark = cur_cfg == nil and 0 or cur_cfg.hole_level
    self.level_break =  cur_break_cfg == nil and 0 or cur_break_cfg.grade
    if self.mark and (self.level_mark > old_level or  self.level_break > old_break) then 
        self.EffectTool:Play(2161025)

        for i=1 ,2 do 
            if cur_cfg["attr_add_"..i] then 
                self.EffectTool:Play(7165082,self.attr_eff_list[i])
            end 
        end 
        
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)

        local is_break =  self.level_break > old_break

        self.EffectTool:Play(is_break and 7165081 or 7165079)
        if cur_cfg.stage < 5 then 
            self.EffectTool:Play(7165080,self.eff_steps[cur_cfg.stage])
        else 
            self.EffectTool:Play(7165083,self.eff_steps[6])
        end 
        
        local old_light = self.cur_suit_light
        self.cur_suit_light = WorkshopData.Instance:CheckAllEquipUpGradeMark()
        if self.mark then 
            if self.cur_suit_light > old_light then 
                self.EffectTool:Play(2161014)
                local item = self.suit_list:GetItem(self.cur_suit_light)
                if item ~= nil then 
                    self.EffectTool:Play(2161015,item.effect)
                    self.suit_list:AutoSlide(item:GetData(),false)
                end
            end 
            if is_break then 
            end 
        end 

        self.mark = false

        if self.Complete_Timer ~= nil then
            TimeHelper:CancelTimer(self.Complete_Timer)
            self.Complete_Timer = nil
        end 

        self.Complete_Timer = TimeHelper:AddDelayTimer(function()
            self.ClickArea:SetActive(false)
		end, 1)
    end 


    -- 需不需要突破 
    local is_max = next_cfg == nil
    local cur_break = BagData.Instance:GetEquipSortStrengthen(self.target_info.equip_type)
    local is_need_break = not is_max and next_cfg.grade ~= cur_break and next_cfg.grade > 0 

    self.EquipShow.gameObject:SetActive(self.target_info.is_empty)
    UH.SpriteName(self.EquipShow, equip_type_sp[self.target_info.equip_type])
    UH.SetText(self.EquipType,  Language.Common.Equip[self.target_info.equip_type])
    self.break_obj:SetActive(not self.target_info.is_empty)
    UH.SetText(self.break_num,DataHelper.GetDaXie(cur_break)..Language.Common.StrengthenLevel)

    self.Complete:SetActive( next_cfg == nil)

    local flag = true
    if not is_max then 
        if self.target_info.equip:IsStarGod() then 
            flag = self.target_info.equip:StrengthenLevel() < next_cfg.need_godequip_level
        else 
            flag = self.target_info.equip:LimitLv() < next_cfg.need_equip_level
        end 
    end 

    self.ClickInter.Interactable = not self.target_info.is_empty and not flag
    self.ClickInter2.Interactable = not self.target_info.is_empty and not flag

    -- UH.SetText(self.BtnUpGradeTxt,is_need_break and Language.Workshop.UpGrade.BtnUpRank or Language.Workshop.UpGrade.BtnUpGrade )

    if is_need_break then 
        -- UH.SetText(self.upgrade_limit,string.format( Language.Workshop.BreakLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level))
        self.upgrade_limit.gameObject:SetActive(false)
        UH.SetText(self.SimpleTips, flag and string.format( Language.Workshop.BreakLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level) or Language.Workshop.SimpleTipsBreak)
         UH.LocalPosG(self.TipsObj, flag and Vector3.New(-165,-241.5)  or Vector3.New(-74,-241.5))

        local vo = {}
        vo.item = new_break_cfg.strengthen_consume_item[0]
        vo.is_lock = new_break_cfg.strengthen_consume_item[0] == nil
        self.stuff_list[1]:SetData(vo)

        self.BtnUpGrade:SetActive( false )
        self.BtnBreak:SetActive(not is_max)
        self.cost_tips:SetActive(not is_max)
        self.stuff_list_obj:SetActive(not is_max)
        self:CheckStuffNeed()
    else 
        if next_cfg ~= nil then 
            -- UH.SetText(self.upgrade_limit,string.format( Language.Workshop.StrenLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level))
            self.upgrade_limit.gameObject:SetActive(false)
            UH.SetText(self.SimpleTips, flag and string.format( Language.Workshop.BreakLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level) or 
             Language.Workshop.SimpleTipsStren)
            UH.LocalPosG(self.TipsObj, flag and Vector3.New(-165,-241.5) or Vector3.New(-202,-241.5) )
            

            local vo = {}
            vo.item = next_cfg.strengthen_consume_item[0]
            vo.is_lock = next_cfg.strengthen_consume_item[0] == nil
            self.stuff_list[1]:SetData(vo)

            self.BtnUpGrade:SetActive(not is_max)
            self.BtnBreak:SetActive(false)
            self.cost_tips:SetActive(not is_max)
            self.stuff_list_obj:SetActive(not is_max)
            self:CheckStuffNeed()
        else
            UH.SetText(self.SimpleTips,Language.Workshop.SimpleTipsStrenMax)
            self.upgrade_limit.gameObject:SetActive(false)
            UH.LocalPosG(self.TipsObj, Vector3.New(-120,-241.5) )

            self.BtnUpGrade:SetActive(not is_max)
            self.BtnBreak:SetActive(not is_max)
            self.cost_tips:SetActive(not is_max)
            self.stuff_list_obj:SetActive(not is_max)
        end         
    end 

    local combo_list = WorkshopData.Instance:GetEquipUpgradeCombo()
    self.suit_list:SetData(combo_list)
end

function WorkshopUpgrade:CheckStuffNeed()
    self.send_data.enough_stuff = true

    self.stuff_tip_list = {}
        local is_enough,lack_num,need_num = self.stuff_list[1]:IsEnough()
        if not is_enough then 
            local item = Item.New({item_id = self.stuff_list[1]:GetData().item.item_id})
            table.insert(self.stuff_tip_list, 
            {name = item:Name(), 
            lack = lack_num,
            need = need_num,
            item = item})
            self.send_data.enough_stuff = false
        end 
end 

function WorkshopUpgrade:FlushLeft()
    local equip_list = BagData.Instance:GetEquipList()
    local jewerly_list = BagData.Instance:GetJewelryList()
    for i = 0,4 do 
        local send_data = {
            equip = equip_list[i] or Item.New(),
            equip_type = i,
            is_empty = equip_list[i] == nil,
        }
        self.equip_list[i + 1]:SetData(send_data)
    end 
    for i = 0,2 do 
        local send_data = {
            equip = jewerly_list[i] or Item.New(),
            equip_type = i + 5,
            is_empty = jewerly_list[i] == nil,
        }
        self.equip_list[i + 6]:SetData(send_data)
    end 
end

function WorkshopUpgrade:FlushBagChange()
    self:FlushRight()
    self:FlushLeft()
end

function WorkshopUpgrade:OnClickEquip(data)
    self.target_info = data 
    self.AttrShow:FlushTarget(data)
    self:FlushRight()

    -- WorkshopData.Instance:AddUpgradeRemindIgnore(data.equip_type)
    -- self.equip_list[data.equip_type + 1]:RedDelete()

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function WorkshopUpgrade:CompleteBlust()
    BagCtrl.Instance:SendEquipmentOperaReq({
        oper_type = 5,
        param_1 = 1,
        param_2 = self.target_info.equip_type,
    })

end

function WorkshopUpgrade:OnClickUpGrade()
    -- if self.upgrade_limit.gameObject.activeSelf then 
    --     PublicPopupCtrl.Instance:Center(Language.Workshop.StrenLimitPopTips)
    --     return 
    -- end 
    -- 需不需要突破 
    local cur_cfg,next_cfg = WorkshopData.Instance:GetEquipUpgradeCfgByType(self.target_info.equip_type)
    local cur_break_cfg,new_break_cfg = WorkshopData.Instance:GetEquipUpgradeBreakCfgByType(self.target_info.equip_type)

    local is_max = next_cfg == nil
    local cur_break = BagData.Instance:GetEquipSortStrengthen(self.target_info.equip_type)
    local is_need_break = not is_max and next_cfg.grade ~= cur_break and next_cfg.grade > 0

    local flag = true
    if self.target_info.equip:IsStarGod() then 
        flag = self.target_info.equip:StrengthenLevel() < next_cfg.need_godequip_level
    else 
        flag = self.target_info.equip:LimitLv() < next_cfg.need_equip_level
    end 

    if flag and is_need_break then 
        PublicPopupCtrl.Instance:Center(string.format( Language.Workshop.BreakLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level))
        return 
    elseif flag then 
        PublicPopupCtrl.Instance:Center(string.format( Language.Workshop.StrenLimitTips,next_cfg.need_equip_level,next_cfg.need_godequip_level))
        return 
    end

    if self.target_info.is_empty then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.StrenEmptyTips)
        return 
    end

    if #self.stuff_tip_list > 0 then
        -- if #self.stuff_tip_list > 1 then 
        --     for _, v in ipairs(self.stuff_tip_list) do
        --         PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], v.name, v.lack))
        --     end  
        --     AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        -- else 
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            local operate_pos = self.stuff_tip_list[1]
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], operate_pos.name, operate_pos.lack))
            -- ViewMgr:OpenView(GetWayView, {item = operate_pos.item})
            ItemInfoCtrl.Instance:ItemInfoView({item_list = {operate_pos.item},show_ways = true})
            BagData.Instance:SetGetWaySweepMark({need = operate_pos.need})
        -- end
        return
    end

    self.mark = true
    if is_need_break or next_cfg.stage == 5 then 
        self.ClickArea:SetActive(true)
    end 
    if not self.stuff_list[1].data.is_lock then 
        self.stuff_fly_list[1]:Play(7165084)
    end 

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

UpgradeAttrShow = UpgradeAttrShow or DeclareMono("UpgradeAttrShow", UIWFlushPanel)
function UpgradeAttrShow:UpgradeAttrShow()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushBagChange},
        {data = BagData.Instance.equip_sort_upgrade, func = self.FlushBagChange, init_call = false},
    }

    self.progress_rate = {
        [0] = 0,
        [1] = 0.21,
        [2] = 0.41,
        [3] = 0.61,
        [4] = 0.807,
        [5] = 1,
    }
end

function UpgradeAttrShow:FlushTarget(data)
    self.target_info = data
    self:FlushBagChange()
end

function UpgradeAttrShow:FlushBagChange()
    if self.target_info == nil then return end 
    local cur_cfg,next_cfg = WorkshopData.Instance:GetEquipUpgradeCfgByType(self.target_info.equip_type)

    local is_max = next_cfg == nil
    local cur_break = BagData.Instance:GetEquipSortStrengthen(self.target_info.equip_type)
    local is_need_break = not is_max and next_cfg.grade ~= cur_break and  next_cfg.grade > 0

    local cur_break_cfg,new_break_cfg = WorkshopData.Instance:GetEquipUpgradeBreakCfgByType(self.target_info.equip_type)

    for i = 1,2 do 
        self.attrlefts[i].gameObject:SetActive( cur_cfg ~= nil and cur_cfg["attr_add_"..i] ~= 0 )
        self.attrmaxs[i].gameObject:SetActive( cur_cfg ~= nil and cur_cfg["attr_add_"..i] ~= 0)

        self.attrrights[i].gameObject:SetActive((new_break_cfg ~= nil and new_break_cfg["attr_add_"..i] ~= 0) or (next_cfg ~= nil and next_cfg["attr_add_"..i] ~= 0))
    end 

    local acc_tianming_plus = AccrssoryData.Instance:GetDestinyRate(TIAN_MING_ADD_ATTR_SYS_TYPE.EQUIP_STRENGTHEN) 
    for i = 1,2 do 
        if cur_cfg ~= nil and cur_break_cfg ~= nil then 
            local plus = acc_tianming_plus > 0 and math.floor( (cur_cfg["attr_add_"..i]+ cur_break_cfg["attr_add_"..i]) * acc_tianming_plus ) or 0
            local re_plus = plus > 0 and " <color=#"..COLORSTR.Green6.."> +"..plus.."</color>" or ""
            UH.SetText(self.attrlefts[i],Language.Common.AttrList[cur_cfg["attr_type_"..i]])
            UH.SetText(self.attrleftnums[i],"+"..cur_cfg["attr_add_"..i] + cur_break_cfg["attr_add_"..i] ..re_plus)

            UH.SetText(self.attrmaxs[i],Language.Common.AttrList[cur_cfg["attr_type_"..i]])
            UH.SetText(self.attrmaxnums[i],"+"..cur_cfg["attr_add_"..i]+ cur_break_cfg["attr_add_"..i]..re_plus)
            -- UH.SetText(self.attrmaxnums[i],"+"..cur_cfg["attr_add_"..i]+ cur_break_cfg["attr_add_"..i]..re_plus)
        elseif cur_cfg ~= nil then 
            local plus = acc_tianming_plus > 0 and math.floor( (cur_cfg["attr_add_"..i]) * acc_tianming_plus ) or 0
            local re_plus = plus > 0 and " <color=#"..COLORSTR.Green6.."> +"..plus.."</color>" or ""
            UH.SetText(self.attrlefts[i],Language.Common.AttrList[cur_cfg["attr_type_"..i]])
            UH.SetText(self.attrleftnums[i],"+"..cur_cfg["attr_add_"..i] ..re_plus)

            UH.SetText(self.attrmaxs[i],Language.Common.AttrList[cur_cfg["attr_type_"..i]])
            UH.SetText(self.attrmaxnums[i],"+"..cur_cfg["attr_add_"..i]..re_plus)
            -- UH.SetText(self.attrmaxnums[i],"+"..cur_cfg["attr_add_"..i]..re_plus)
        end 

        if is_need_break and new_break_cfg ~= nil then 
            local cur_break_value = cur_break_cfg == nil and 0 or cur_break_cfg["attr_add_"..i]

            local plus = acc_tianming_plus > 0 and math.floor( (cur_cfg["attr_add_"..i] + cur_break_value + new_break_cfg["attr_add_"..i]) * acc_tianming_plus ) or 0
            local re_plus = plus > 0 and " <color=#"..COLORSTR.Green6.."> +"..plus.."</color>" or ""
    
            UH.SetText(self.attrrights[i],Language.Common.AttrList[next_cfg["attr_type_"..i]])
            UH.SetText(self.attrrightnums[i],"+"..cur_cfg["attr_add_"..i] +cur_break_value + new_break_cfg["attr_add_"..i] ..re_plus)

        elseif not is_need_break and next_cfg ~= nil then 
            local cur_break_value = cur_break_cfg == nil and 0 or cur_break_cfg["attr_add_"..i]

            local plus = acc_tianming_plus > 0 and math.floor( (next_cfg["attr_add_"..i] + cur_break_value) * acc_tianming_plus ) or 0
            local re_plus = plus > 0 and " <color=#"..COLORSTR.Green6.."> +"..plus.."</color>" or ""
    
            UH.SetText(self.attrrights[i],Language.Common.AttrList[next_cfg["attr_type_"..i]])
            UH.SetText(self.attrrightnums[i],"+"..next_cfg["attr_add_"..i] + (cur_break_cfg ~= nil and cur_break_value or 0) ..re_plus)
        end 
    end 

    if is_need_break then 
        self.attr_left:SetActive(true)
        self.left_empty:SetActive(cur_cfg == nil)
        self.attr_max:SetActive(is_max)
    else 
        self.left_empty:SetActive(cur_cfg == nil)
        self.attr_max:SetActive(is_max)
        self.attr_left:SetActive(not is_max)
    end 

    self.arrow:SetActive(not is_max)

    if is_need_break then 
        self.progress:SetProgress( self.progress_rate[cur_cfg ~= nil and cur_cfg.stage or 0])
    else 
        local cur_break = BagData.Instance:GetEquipSortStrengthen(self.target_info.equip_type)

        if cur_cfg == nil then 
            self.progress:SetProgress( self.progress_rate[0])
        elseif cur_cfg ~= nil and cur_break == cur_cfg.grade then 
            self.progress:SetProgress( self.progress_rate[cur_cfg.stage])
        elseif cur_break == next_cfg.grade then 
            self.progress:SetProgress( self.progress_rate[0])
        end 
    end

end

UpgradeEquipItem = UpgradeEquipItem or DeclareMono("UpgradeEquipItem", UIWidgetBaseItem)
function UpgradeEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local with_data = data.equip.item_id > 0 

    self.Equip:SetActive(not with_data)
    self.Type:SetActive(with_data)
    self.Type:SetActive(with_data)
    self.Level.gameObject:SetActive(with_data)
    self.LevelShow:SetActive(with_data)
    self.Cell:SetData(data.equip)
    if with_data then 
        local str = data.equip:Level()..Language.Common.Level
        if data.equip:Color() == ItemColor.Star or data.equip:Color() == ItemColor.God then
            str = data.equip:StrengthenLevel()..Language.Common.StrengthenLevel
        end
        UH.SetText(self.EquipLevel, str)
    end 

    UH.SetText(self.Level,"+"..BagData.Instance:GetEquipSortUpgrade(data.equip_type))

    local cur_cfg,next_cfg = WorkshopData.Instance:GetEquipUpgradeCfgByType(data.equip_type)
    local cur_red = 0
    local is_ignore = WorkshopData.Instance:CheckIsIgnoreRemindInUpgrade(data.equip_type)

    local is_max = next_cfg == nil
    local cur_break = BagData.Instance:GetEquipSortStrengthen(data.equip_type)
    local is_need_break = not is_max and next_cfg.grade ~= cur_break and next_cfg.grade > 0
    local cur_break_cfg,new_break_cfg = WorkshopData.Instance:GetEquipUpgradeBreakCfgByType(data.equip_type)

    if is_need_break then 
        if new_break_cfg ~= nil and not data.is_empty then 
            local flag = true
            if data.equip:IsStarGod() then 
                flag = data.equip:StrengthenLevel() >= next_cfg.need_godequip_level
            else 
                flag = data.equip:LimitLv() >= next_cfg.need_equip_level
            end 

            for i,j in pairs(new_break_cfg.strengthen_consume_item) do
                if Item.GetNum(j.item_id) < j.num then 
                    flag = false
                end 
            end 
            cur_red = (flag and not is_ignore) and 1 or 0
        end 
    else 
        if next_cfg ~= nil and not data.is_empty then 
            local flag = true
            if data.equip:IsStarGod() then 
                flag = data.equip:StrengthenLevel() >= next_cfg.need_godequip_level
            else 
                flag = data.equip:LimitLv() >= next_cfg.need_equip_level
            end 

            for i,j in pairs(next_cfg.strengthen_consume_item) do
                if Item.GetNum(j.item_id) < j.num then 
                    flag = false
                end 
            end 
            cur_red = (flag and not is_ignore) and 1 or 0
        end 
    end 

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.red_pos,Vector3.New(40, 50))
    end
    self.red_point:SetNum(cur_red)

end

function UpgradeEquipItem:Click()
    if self.ItemClick then 
        self.ItemClick:Trigger()
    end 
end

function UpgradeEquipItem:RedDelete()
    if self.red_point ~= nil then
        self.red_point:SetNum(0)
    end 
end

UpgradeStuffItem = UpgradeStuffItem or DeclareMono("UpgradeStuffItem", UIWidgetBaseItem)
function UpgradeStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.lock:SetActive(data.is_lock)
    self.cell.gameObject:SetActive(not data.is_lock)
    self.num.gameObject:SetActive(not data.is_lock)

    UH.SetText(self.name,Item.GetQuaName(data.item.item_id,true))

    if not data.is_lock then 
        self.target_item = Item.New({item_id = data.item.item_id})
        self.cell:SetData(self.target_item)
        local flag = Item.GetNum(data.item.item_id) < data.item.num
        local color =  flag and COLORSTR.Red8 or COLORSTR.Green9
        UH.SetText(self.num,string.format(Language.Common.Cstr,color,Item.GetNum(data.item.item_id),data.item.num))
    end
end

function UpgradeStuffItem:IsEnough()
    if self.data.is_lock == true then return true,0 end 

    local lack_num = -1
    local is_enough = Item.GetNum(self.data.item.item_id) >= self.data.item.num
    if not is_enough then 
        lack_num = self.data.item.num - Item.GetNum(self.data.item.item_id)
    end 
    return is_enough,lack_num,self.data.item.num
end

function UpgradeStuffItem:OnClickItem()
    if self.data.is_lock == true then return end 

    -- ViewMgr:OpenView(GetWayView, {item = self.target_item})
    ItemInfoCtrl.Instance:ItemInfoView({item_list = {self.target_item},show_ways = true})
    BagData.Instance:SetGetWaySweepMark({need = self.data.item.num})
end

UpgradeSuitItem = UpgradeSuitItem or DeclareMono("UpgradeSuitItem", UIWidgetBaseItem)
function UpgradeSuitItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.text,data.str)
    local flag = WorkshopData.Instance:CheckEquipUpGradeMarkEqual(data.hole_level_all)
    UH.Color(self.text,flag and COLORS.Yellow13 or COLORS.Gray2 )
    UH.SpriteName(self.tips, flag and "str_tips1" or "str_tips2" )
end