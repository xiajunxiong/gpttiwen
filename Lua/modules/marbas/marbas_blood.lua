MarbasBlood  = MarbasBlood or DeclareMono("MarbasBlood", UIWFlushPanel)
function MarbasBlood:MarbasBlood()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.item_grid,func = self.EquipFlush, init_call = false},
        {data = self.data.equip_list,func = self.EquipFlush, init_call = false},
        {data = BagData.Instance.item_grid, func = self.EquipFlush, init_call = false},
        -- {data = self.data.blood_view_data,func = self.MainFlush, init_call = false},
        -- {data = self.data.equip_list,func = self.MainFlush, init_call = false},
        -- {data = self.data.blood_view_data,func = self.GemEff, keys = {"GemEff"}, init_call = false},
    }
    self.wait_num = -1
    self.unique = {}


    self.EffectSelect:SetData({
        mono = self,
        start_effect = 3165130,
        speed_time = 3,
        end_func = BindTool.Bind2(self.FlyEnd, self,1),
    })

    for i = 1,4 do 
        self.Insets[i]:SetData({
            mono = self,
            start_effect = 3165130,
            complete_effect = 3165132,
            speed_time = 3,
            pre_pause = 1,
            end_func = BindTool.Bind2(self.FlyEnd, self,2),
        })
    end 

    self.effect_param = {}
end 

function MarbasBlood:FlyEnd(type)
    if type == 1 then 
        self.EffectSelect:CancelFlyStarEffect()
    else
        for i = 1,4 do 
            self.Insets[i]:CancelFlyStarEffect()
        end 
    end 
end

function MarbasBlood:Awake()
    UIWFlushPanel.Awake(self)
    self:EquipFlush({init = true})
end

function MarbasBlood:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.init_timer)

    TimeHelper:CancelTimer(self.blood_timer)

    self.EffectSelect:AllHandleCancel()
    for i = 1,4 do 
        self.Insets[i]:AllHandleCancel()
    end 

    self.data:SetBloodSetData()
    self.data.blood_view_data.need_pick = false

    self.center.lock_mark = nil
end


function MarbasBlood:EquipFlush(param)
    local list = self.data:GetCanBloodEquipList()
    self.EquipList:SetData(list)

    local need_mark = self.data:CheckNeedMark()
    if param.init or need_mark then 
        if need_mark then 
            self.EquipList:JumpVertIndex(1)
        end 
        self.init_timer = TimeHelper:AddDelayFrameTimer(function ()
            self.EquipList:ClickItem(1)
		end,5) 
    end

    self.center.gameObject:SetActive(#list > 0)
    self.right:SetActive(#list > 0)
    self.left:SetActive(#list > 0)
    self.empty_part:SetActive(#list == 0)
end

function MarbasBlood:OnClickEquipCell(data)
    self.data:SetBloodSetData(data)

    -- self.EquipList.index_item[self.EquipList:GetByData(data)]:ShowSelect()
end 

function MarbasBlood:AfterComplete()
    
end

function MarbasBlood:OnEffectComplete()
    if self.effect_param.type == 1 then 
        local mark = self.data.blood_view_data.select 

        if self.effect_param.sub_type == 1 then 
            MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.Force, 
                 mark.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped, 
                 mark:GridIndex(), 0)

            self.block:SetActive(false)
            self.data:SetMasterBlock(false)
        else 
            self.effect_param.sub_type = 1
            UH.LocalPosG(self.EffectSelect.CompletePos, Vector3.New(0,-100,0))
            self.EffectTool:Play("3165129",self.EffectSelect.CompletePos) 
            self.block:SetActive(true)
            self.data:SetMasterBlock(true)
        end 
    else 
        local param_send = self.data.blood_view_data.param_send 
        MarbasCtrl.Instance:SendBloodGemOper(param_send)

        self.data.blood_view_data.param_send  = nil
        self.block:SetActive(false)
        self.data:SetMasterBlock(false)
    end 
end

function MarbasBlood:PlayEffect(param)
    if self.inset_delay then
        TimeHelper:CancelTimer(self.inset_delay)
    end

    self.effect_param = param
    -- 装备血祭行为
    if param.type == 1 then 
        local pos = param.sub_type == 1 and Vector3.New(0,-100,0) or Vector3.New(100,-83,0)
        UH.LocalPosG(self.EffectSelect.CompletePos, pos)
        
        self.EffectSelect:SetCompleteEffect(param.sub_type == 1 and "3165129" or "3165155")
        
        self.EffectSelect:Play("3165131")
    -- 镶嵌元魂行为
    else 
        self.Insets[param.sub_type]:Play("3165131")
        self.handle_delay = TimeHelper:AddDelayTimer(function()
            self.center:OnClickCloseSelect()
        end,1)
    end     

    self.block:SetActive(true)
    self.data:SetMasterBlock(true)
end

function MarbasBlood:EffectTest(index)
    self:PlayEffect({type = 2,sub_type =index})
end

function MarbasBlood:OnClickWayBuild()
    ViewMgr:OpenViewByKey(Mod.Marbas.Build)
end

function MarbasBlood:OnClickWayFB()
    ViewMgr:OpenViewByKey(Mod.HarmonyHeaven.Main)

    ViewMgr:CloseView(MarbasDemonsView) 
end

MarbasBloodCenterShow = MarbasBloodCenterShow or DeclareMono("MarbasBloodCenterShow",UIWFlushPanel)
function MarbasBloodCenterShow:MarbasBloodCenterShow()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance
      
    self.data_cares = {
        {data = self.data.blood_view_data,func = self.MainFlush, init_call = false},
        {data = self.data.equip_list,func = self.MainFlush, init_call = false},
        {data = self.data.item_grid,func = self.MainFlush, init_call = false},
        {data = BagData.Instance.item_grid, func = self.MainFlush, init_call = false},
        {data = self.data.blood_view_data,func = self.GemEff, init_call = false},
        {data = self.data.blood_view_data,func = self.GemChange,keys = {"need_pick"}, init_call = false},
    }

end

function MarbasBloodCenterShow:InitGemPos()
    for i = 1,4 do  
        self.cell_list[i].gameObject:SetParent(self.gem_pos[i],false)        
        UH.LocalPosG(self.cell_list[i].gameObject,Vector3.New(0,50,0))
        self.cell_list[i].gameObject:SetParent(self.gem_part,true) 

        self.panel.Insets[i].EndPos:SetParent(self.gem_pos[i],false)  
        UH.LocalPosG(self.panel.Insets[i].EndPos,Vector3.New(0,50,0))      
        self.panel.Insets[i].EndPos:SetParent(self.panel.Insets[i].gameObject,true) 

        self.panel.Insets[i].CompletePos:SetParent(self.gem_pos[i],false)  
        UH.LocalPosG(self.panel.Insets[i].CompletePos,Vector3.New(0,50,0))      
        self.panel.Insets[i].CompletePos:SetParent(self.panel.Insets[i].gameObject,true) 
    end 
end

function MarbasBloodCenterShow:MainFlush(param_t)

    if not self.init then 
        self:InitGemPos()
        self.init = true
    end 

    local mark = self.data:TypeBloodGetBloodSelect()
    if mark == nil then return end 

    UH.SetText(self.point_max, mark:CheckForce())
    UH.SetText(self.point_cur, mark:CheckForce())
    local plus = mark:CheckForce() + self.data:GetNextForce(mark:CheckMana())
    UH.SetText(self.point_next, plus > self.data.config.other[1].max_soul_power and self.data.config.other[1].max_soul_power or plus)

    self.point_call:SetDataByItemIdWithNeed(Config.zaohua_equip_up_auto.other[1].blood_sacrifice_consume,
        Config.zaohua_equip_up_auto.other[1].consume_num)
    
    UH.SetText(self.btn_text, mark:CheckForce() > 0 and Language.Marbas.BloodAdd or Language.Marbas.BloodInit)
    
    if self.lock_mark ~= nil then 
        for i = 1,4 do 
            local cur_lock = self.data:GetBloodSoltLock(mark,i)
            if self.lock_mark[i].is_lock ~= cur_lock and cur_lock == true then 
                self.EffectTool:Play(3165159,self.cell_list[i].eff_pos2)
                self.panel.block:SetActive(true)
                self.data:SetMasterBlock(true)
                self.eff_mark = i
                break
            end 
        end 
        if self.eff_mark ~= nil then 
            return 
        elseif self.data.unlock_mark then 
            self.lock_mark = nil
            self.data.unlock_mark = false
        end 
    end 

    for i = 1,4 do 
        local act_flag = self.data:GetBloodSoltLock(mark,i)
        self.cell_list[i]:SetData(Item.Create(mark:GetStone(i)))
        self.cell_list[i]:ShowLock(not self.data:GetBloodSoltLock(mark,i))
    end 

    self.add_status:SetActive(not self.data:IsMaxForce(mark))
    self.max_status:SetActive(self.data:IsMaxForce(mark))
    self.max_show:SetActive(self.data:IsMaxForce(mark))
    self.btn_pointin:SetActive(not self.data:IsMaxForce(mark))
    self.matrial:SetActive(not self.data:IsMaxForce(mark))

    self.point_part:SetActive(mark:CheckMana() > 0)
    self.gem_part:SetActive(mark:CheckForce() > 0)


    if mark ~= nil then 
        local bg_path = self.data:BigIconPath(mark:EquipGrade(), mark:EquipType())
        ResMgr:LoadObject(bg_path, function(tex)
            if not self:Destroyed() then
                if self.Icon then
                    self.Icon.texture = tex
                    self.Icon.gameObject:SetActive(true)
                end
            end
        end, false)
    else 
        self.Icon.gameObject:SetActive(false)
    end 

    UH.SetText(self.mana,mark:ManaDesc())

    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.red_pos)
	end

    local item_num = Item.GetNum(Config.zaohua_equip_up_auto.other[1].blood_sacrifice_consume)
    self.red_point:SetNum(item_num > Config.zaohua_equip_up_auto.other[1].consume_num and 1 or 0)
end

function MarbasBloodCenterShow:GemChange()
    if not self.data.blood_view_data.need_pick then 
       return  
    end 
    self.select_part:SetActive(self.data.blood_view_data.need_pick)

    self.data.blood_view_data.need_pick = false
end

function MarbasBloodCenterShow:OnClickPlus(index)
    local mark = self.data:TypeBloodGetBloodSelect()
    local slot = mark:GetStone(index+1)

    local is_lock = MarbasData.Instance:GetBloodSoltLock(mark,index+1)
    if not is_lock then 
        local str = self.data:GetBloodSoltLockStr(mark,index+1)
        PublicPopupCtrl.Instance:Center(str)    
        return 
    end 

    local list = self.data:GetMarbasGemList()

    self.data:SetBloodSetGemSlot(index)
    if slot.item_id == 0 and #list > 0 then 
		self.select_part:SetActive(true)
        return 
    elseif slot.item_id == 0 and #list == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.BloodGemEmpty)    
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = MarbasConfig.DefluatGem})})
        return 
    end 

    local gem_item = Item.Init(slot.item_id)
    gem_item.param = slot
    gem_item.src_type = mark.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped
    local info = {
		item_list = {gem_item},
        func = function()
            local temp = {}
            local next_item = self.data:GetGemNextLevel(slot.item_id)     
            if next_item.item_id ~= slot.item_id then 
                table.insert(temp,ItemInfoFunc.MarbasBloodGemPlus)
            end 
            table.insert(temp,ItemInfoFunc.MarbasBloodGemChange)
            table.insert(temp,ItemInfoFunc.MarbasBloodGemUnfix)
            return temp
        end
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function MarbasBloodCenterShow:OnClickPointIn()
    local mark = self.data:TypeBloodGetBloodSelect()
    local cost = self.point_call:GetData()
    
    if Item.GetNum(cost.item_id) <= cost.need_num then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.BloodPointInLackCost)    
        ViewMgr:OpenView(GetWayView, {item = cost})
        return 
    end 
    
    self.panel:PlayEffect({type = 1 ,sub_type = mark:CheckForce() > 0 and 1 or 0})

    self.lock_mark = {}
    for i = 1,4 do 
        local vo = {}
        vo.is_lock = true
        if i < 4 then 
            vo.is_lock = self.data:GetBloodSoltLock(mark,i)
        end 
         
        table.insert(self.lock_mark,vo)
    end 
end

function MarbasBloodCenterShow:OnLockEffectComplete()
    if not self.unlock_effshow.is_playing and self.eff_mark ~= nil then 
        self.unlock_effshow:PlayUnLockEff()
        self.eff_mark = nil
    elseif self.unlock_effshow.is_playing and self.eff_mark == nil then 
        self.unlock_effshow:BackingShow()
    end 
end

function MarbasBloodCenterShow:OnClickCloseSelect()
    self.select_part:SetActive(false)
end

function MarbasBloodCenterShow:GemEff()
    if self.data.blood_view_data.param_send ~= nil then 
        local obj = self.select_panel:CatchItemByIndex(self.data.blood_view_data.select_gem:GridIndex())
        local operate_cell = self.panel.Insets[self.data.blood_view_data.select_index + 1]
        operate_cell.StartPos:SetParent(obj,false)
        operate_cell.StartPos:SetLocalPosition(Vector3.zero)
        operate_cell.StartPos:SetParent(operate_cell.gameObject,true)

        self.panel:PlayEffect({type =2 ,sub_type = self.data.blood_view_data.select_index+1})
    end 
end

MarbasBloodUnlockEffShow = MarbasBloodUnlockEffShow or DeclareMono("MarbasBloodUnlockEffShow",UIWFlushPanel)
function MarbasBloodUnlockEffShow:MarbasBloodUnlockEffShow()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.is_playing = false
    self.frame_setp = 30
end 

function MarbasBloodUnlockEffShow:OnDestroy()
    TimeHelper:CancelTimer(self.delay_timer)
    TimeHelper:CancelTimer(self.frame_runner)
    TimeHelper:CancelTimer(self.frame_delayer)
end

function MarbasBloodUnlockEffShow:PlayUnLockEff()
    TimeHelper:CancelTimer(self.delay_timer)
    TimeHelper:CancelTimer(self.frame_runner)
    TimeHelper:CancelTimer(self.frame_delayer)
    self.delay_timer = TimeHelper:AddDelayTimer(function ()
        self:EndComplete()
    end, 8)

    self.is_playing = true
    self.gem_index = self.center.eff_mark
    self.mark_gem_pos = self.total.transform:InverseTransformPoint(self.center.cell_list[self.gem_index].gameObject.transform.position)
    self.mark_icon_pos = self.total.transform:InverseTransformPoint(self.center.Icon.gameObject.transform.position)

    -- 开始演出
    self.total:SetActive(true)

    -- 移动至正中
    UH.LocalPosG(self.center_pos, self.mark_icon_pos)
    UH.LocalPosG(self.gem_obj, self.mark_gem_pos)
    self.gem_lock_obj:SetActive(true)
    self.gem_plus_obj:SetActive(false)
    self.is_moving = true
    self.frame_runner = TimeHelper:AddRunFrameTimer(function ()
		local step_x = self.gem_obj.transform.localPosition.x + (self.mark_icon_pos.x -  self.mark_gem_pos.x) / self.frame_setp
		local step_y = self.gem_obj.transform.localPosition.y + (self.mark_icon_pos.y -  self.mark_gem_pos.y) / self.frame_setp

        UH.LocalPosG(self.gem_obj,  Vector3.New(step_x,step_y,0))
    end,1 , self.frame_setp)

    -- 移动完毕，播特效
    self.frame_delayer = TimeHelper:AddDelayFrameTimer(function ()
        self.center.EffectTool:Play(3165158,self.center_pos)
        self.is_moving = false
    end,self.frame_setp)
end

function MarbasBloodUnlockEffShow:BackingShow()
    if self.is_moving then 
        return
    end 
    TimeHelper:CancelTimer(self.frame_runner)
    TimeHelper:CancelTimer(self.frame_delayer)

    UH.LocalPosG(self.gem_obj, self.mark_icon_pos)

    --归位
    self.gem_lock_obj:SetActive(false)
    self.gem_plus_obj:SetActive(true)
    self.frame_runner = TimeHelper:AddRunFrameTimer(function ()
		local step_x = self.gem_obj.transform.localPosition.x + (self.mark_gem_pos.x -  self.mark_icon_pos.x) / self.frame_setp
		local step_y = self.gem_obj.transform.localPosition.y + (self.mark_gem_pos.y -  self.mark_icon_pos.y) / self.frame_setp

        UH.LocalPosG(self.gem_obj, Vector3.New(step_x,step_y,0))
    end,1 , self.frame_setp)

    -- 移动完毕,结束演出
    self.frame_delayer = TimeHelper:AddDelayFrameTimer(function ()
        self:EndComplete()
    end,self.frame_setp)
end

function MarbasBloodUnlockEffShow:EndComplete()
	if not ViewMgr:IsOpen(MarbasView) then 
        return 
    end 
    
    for i = 1,4 do 
        self.center.cell_list[i].gameObject:SetActive(true)
    end 

    self.is_playing = false
    self.total:SetActive(false)
    self.center.lock_mark = nil
    self.data.unlock_mark = false
    self.panel.block:SetActive(false)
    self.data:SetMasterBlock(false)
    self.center:MainFlush()
end

MarbasBloodShowGemCell = MarbasBloodShowGemCell or DeclareMono("MarbasBloodShowGemCell",UIWidgetBaseItem)
function MarbasBloodShowGemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name,data.item_id > 0 and ColorStr(data:GemLevel()..Language.Common.Level,data:ColorStr()) 
        or "")
    self.name_show:SetActive(data.item_id >0)
    self.call:SetData(data)

    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.red_pos)
	end
end

function MarbasBloodShowGemCell:ShowLock(flag)
    self.lock:SetActive(flag)
    self.plus:SetActive(self.data.item_id == 0 and not flag )

    self.red_point:SetNum(((not flag) and self.data.item_id == 0 and #MarbasData.Instance:GetMarbasGemList() > 0) and 1 or 0)
end

MarbasBloodPlusPart = MarbasBloodPlusPart or DeclareMono("MarbasBloodPlusPart",UIWFlushPanel)
function MarbasBloodPlusPart:MarbasBloodPlusPart()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.blood_view_data,func = self.MainFlush, init_call = false},
        {data = self.data.item_grid,func = self.MainFlush, init_call = false},
        {data = self.data.equip_list,func = self.MainFlush, init_call = false},
    }
end

function MarbasBloodPlusPart:MainFlush()
    local mark = self.data:TypeBloodGetBloodSelect()
    if mark == nil then return end 

    local max_flag = mark:CheckForce() >= Config.zaohua_equip_up_auto.other[1].max_soul_power
    local str_1 = max_flag and Config.zaohua_equip_up_auto.other[1].max_soul_power or mark:CheckForce()
    local str_2 = max_flag and Language.Marbas.BloodMaxShow or Config.zaohua_equip_up_auto.other[1].max_soul_power
    UH.SetText(self.tama_num,string.format( "%s(%s)",str_1,str_2)) 

    self.tama_max:SetActive(max_flag)
    local param = self.data:GetBloodPlusAttrs(mark)

    self.TitleStatusName.gameObject:SetActive(param.is_act)
    self.TitleStatusUnAct:SetActive(not param.is_act)
    if param.is_act then 
        UH.SetText(self.TitleStatusName,param.name)
    end 

    for i = 1,self.plus_attrs:Length() do 
        local flag = param.list[i] ~= nil
        self.plus_attrs[i].gameObject:SetActive(flag)
        if flag then 
            self.plus_attrs[i]:SetData(param.list[i])
        end 
    end
    
end

function MarbasBloodPlusPart:OnClickTips()
    ViewMgr:OpenView(MarbasBloodTips)
end

MarbasBloodPlusAttrCell= MarbasBloodPlusAttrCell or DeclareMono("MarbasBloodPlusAttrCell",UIWidgetBaseItem)
function MarbasBloodPlusAttrCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_type,Language.Common.AttrList[data.attr_type])
    local is_percent = DataHelper.IsPercent(data.attr_type)
    UH.SetText(self.attr_value,is_percent and "+"..Percent(data.attr_value) or "+"..data.attr_value)
    self.arrow:SetActive(data.is_next)
    
    local next = data.is_next and ( data.o_next == 1 and data.attr_next_attr or data.attr_next_attr - data.attr_value) or 0
    UH.SetText(self.attr_plus,data.is_next and (is_percent and "+"..Percent(next) or "+"..next) or "" )

    if self.inter then self.inter.Interactable = data.is_act end 
end 


MarbasBloodGemPart = MarbasBloodGemPart or DeclareMono("MarbasBloodGemPart",UIWFlushPanel)
function MarbasBloodGemPart:MarbasBloodGemPart()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.blood_view_data,func = self.MainFlush, init_call = false},
        {data = self.data.item_grid,func = self.MainFlush, init_call = false},
        {data = self.data.equip_list,func = self.MainFlush, init_call = false},
    }
end

function MarbasBloodGemPart:MainFlush()
    local mark = self.data:TypeBloodGetBloodSelect()
    if mark == nil then return end 

    local param = self.data:GetBloodGemNeoAttrs(mark)
    self.plus_list:SetData(param.list)
    -- for i = 1,self.plus_attrs:Length() do 
    --     local flag = param.list[i] ~= nil 
        
    --     self.plus_attrs[i].gameObject:SetActive(flag)
    --     if flag then 
    --         self.plus_attrs[i]:SetData(param.list[i])
    --     end 
    -- end
    
    -- self.gem_plus_name.gameObject:SetActive(param.with_sp)
    -- self.gem_plis_empty.gameObject:SetActive(not param.with_sp)
    
    -- UH.SetText(self.gem_plus_name,param.name )
    -- self.gem_unact_plus:SetActive(not param.with_sp)

    -- UH.SetText(self.gem_plis_empty,param.sp_desc)
    -- UH.SetText(self.gem_plis_desc,param.with_sp and param.sp_desc or "")
    -- self.gem_plis_inter.Interactable = param.with_sp

    self.gem_empty:SetActive(#param.list == 0)
end

MarbasBloodGemRePart = MarbasBloodGemRePart or DeclareMono("MarbasBloodGemRePart",UIWFlushPanel)
function MarbasBloodGemRePart:MarbasBloodGemRePart()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.blood_view_data,func = self.MainFlush, init_call = false},
        {data = self.data.item_grid,func = self.MainFlush, init_call = false},
        {data = self.data.equip_list,func = self.MainFlush, init_call = false},
    }
end

function MarbasBloodGemRePart:MainFlush()
    local mark = self.data:TypeBloodGetBloodSelect()
    if mark == nil then return end 

    local param = self.data:GetBloodGemNeoAttrs(mark)
    self.gem_plus_name.gameObject:SetActive(param.with_sp)
    self.gem_empty.gameObject:SetActive(not param.with_sp)
    self.gem_unact_plus:SetActive(not param.with_sp)

    UH.SetText(self.gem_plus_name,param.name )

    for i = 1,self.plus_attrs:Length() do 
        local flag = param.sp_list[i] ~= nil 
        self.plus_attrs[i].gameObject:SetActive(flag)
        if flag then 
            self.plus_attrs[i]:SetData( param.sp_list[i] )
        end 
    end 
end 

MarbasBloodPlusReCell = MarbasBloodPlusReCell or DeclareMono("MarbasBloodPlusReCell",UIWidgetBaseItem)
function MarbasBloodPlusReCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_type,data.type ~= nil and data.type or "")
    UH.SetText(self.attr_value,data.value ~= nil and data.value or "")
    UH.SetText(self.attr_desc,data.desc ~= nil and data.desc or "")
end

MarbasBloodEquipCell = MarbasBloodEquipCell or DeclareMono("MarbasBloodEquipCell",UIWidgetBaseItem)
function MarbasBloodEquipCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Call:SetData(data)
    UH.SetText(self.Name, data:QuaName())
    self.is_equip:SetActive(not data.in_bag)

    local with_empty = false
    for i = 1,self.gem_list:Length() do 
        self.gem_list[i]:ShowLock(not MarbasData.Instance:GetBloodSoltLock(data,i))    
        self.gem_list[i]:SetData(Item.Create(data:GetStone(i)))
        with_empty = with_empty or (MarbasData.Instance:GetBloodSoltLock(data,i) and data:GetStone(i).item_id == 0 and  #MarbasData.Instance:GetMarbasGemList() > 0)
    end 

    UH.RenameGame(self,Format(data.in_bag and "bag_%s" or "equiped_%s",data.index))


    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.red_pos)
	end
    self.red_point:SetNum(with_empty and 1 or 0)

    self:ShowSelect()
end 

function MarbasBloodEquipCell:ShowSelect()

    local flag =  BagData.Instance:JudgeEquipIsEqual(self.data, MarbasData.Instance.blood_view_data.select) 
    
    if flag then 
        self:Click()
    end 
    
    for i =1,self.selects:Length() do 
        self.selects[i]:SetActive(flag)
    end 
end

function MarbasBloodEquipCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

MarbasBloodGemCell = MarbasBloodGemCell or DeclareMono("MarbasBloodGemCell",UIWidgetBaseItem)
function MarbasBloodGemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    if data == nil or data.item_id == 0 then 
        self.icon.gameObject:SetActive(false) 
        self.fr_red:SetActive(false)
        self.fr_aya:SetActive(false)
        return 
    end

    self.icon.gameObject:SetActive(true) 
    UH.SetIcon(self.icon,data:IconId())
    self.fr_red:SetActive(data:Color() == ItemColor.Red)
    self.fr_aya:SetActive(data:Color() == ItemColor.Gold)
end

function MarbasBloodGemCell:ShowLock(flag)
    self.lock:SetActive(flag)
end

