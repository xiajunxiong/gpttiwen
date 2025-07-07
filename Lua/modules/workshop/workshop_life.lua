WorkshopLife = WorkshopLife or DeclareMono("WorkshopLife", UIWFlushPanel)

function WorkshopLife:WorkshopLife()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        -- {data = self.data:ForgingData(), func = self.FlushLeftItem, keys = {"select_prof", "select_level"}}
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false}
    }
    self.stuff_num_list = {}
end

function WorkshopLife:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
end 
function WorkshopLife:onFlush()
    local data = self.data:LifeShowList()
    self.ItemList:SetData(data)
    local jump_key = self.data:GetLifeItemQuickJump()
    self.ItemList:ClickItem(jump_key)

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    if jump_key > 1 then 
        self.flush_left_item_handle = TimeHelper:AddDelayTimer(function()
            self.flush_left_item_handle = nil
            if jump_key <= #data -4 then 
                self.ItemList:JumpVerticalIndex(jump_key,104)
            else 
                self.ItemList:JumpVerticalIndex(#data -4,104)
            end 
        end,0.2)
    end 
end
function WorkshopLife:FlushRightItem()
    if self.target_info then
        self:OnClickItem(self.target_info)
    end
end
function WorkshopLife:OnClickItem(data)
    self.make_num = self.make_num or 1
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}
    self.target_info = data
    local item = nil
    local item_id = 0
    local flag_complete = true

    for i = 1,4 do 
        local vo = {}
        local _item = Item.New({item_id = self.target_info["stuff_id_"..i] or 0}) 
        vo.item_info = _item
        vo.active_seq = self.target_info.active_seq
        self.MaterialList[i]:SetData(vo)

        if data["stuff_id_" .. i] > 0 then 
            self.stuff_num_list.need[i] = data["stuff_num_" .. i]
            self.stuff_num_list.have[i] = _item:Num()
        end 

    end 
    
    local is_learned = data.active_seq == 0 or( data.active_seq > 0 and  self.data:GetAcitveInfo(data.active_seq))
    self.Active:SetActive(is_learned)
    self.NoActive:SetActive(not is_learned)
    if is_learned and flag_complete then 
        self.CanDoAnim:Play()
    end 
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopLife:OnClickGet()
    ItemInfoCtrl.Instance:ItemInfoView({show_ways = true,item_list = {Item.Init(self.target_info.activation)}})
end

function WorkshopLife:OnClickAdd(num)
    self.make_num = self.make_num + num
    if self.make_num <= 0 then
        self.make_num = 1
        -- PublicPopupCtrl.Instance:Center(ErrorText[19])
        return
    end
    local can_make = true
    for k, v in pairs(self.stuff_num_list.need) do
        local have = self.stuff_num_list.have[k]
        local need = v * self.make_num
        if have < need then
            can_make = false
        end
    end
    if not can_make then
        self.make_num = self.make_num - 1
        -- PublicPopupCtrl.Instance:Center(ErrorText[20])
        return
    end
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function WorkshopLife:OnClickMax()
    --计算最大个数
    local max_num = self.data:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function WorkshopLife:FlushResult()
    UH.SetText(self.LblNum, self.make_num)
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.make_num
    UH.SetText(self.LblCost, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need

    self.BtnMoney:SetActive(need > 0)
    self.BtnFree:SetActive(not (need > 0) )

    for i =1 ,4 do 
        local need_num = self.target_info["stuff_num_"..i]
        self.MaterialList[i]:FlushStuffNum(need_num,self.make_num,true)
    end 
end

function WorkshopLife:OnClickToMake()
    -- 钱不够 材料不足
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.data:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need) == 0 then
        -- PublicPopupCtrl.Instance:Center(ErrorText[22])
        local operate = nil
        for i = 1 ,5 do 
            if self.target_info["stuff_id_"..i] > 0 and Item.GetNum(self.target_info["stuff_id_"..i]) < self.target_info["stuff_num_"..i]  then 
                operate = Item.Create({item_id = self.target_info["stuff_id_"..i]})
                break
            end 
        end  
        if operate ~=nil then  MainOtherCtrl.Instance:GetWayView({item = operate}) end 
        return
    end

    self:ShowCreateEffectEffect()
   
end
function WorkshopLife:OnClickStuff(index)
    if self.target_info == nil then
        return
    end
    -- if self.target_info.active_seq > 0 and not self.data:GetAcitveInfo(self.target_info.active_seq) then
    --     return
    -- end
    local data = Item.New()
    data.item_id = self.target_info["stuff_id_" .. index]
    if data.item_id == 0 then
        return
    end
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopLife:OnClickTarget()
    local data = Item.New()
    data.item_id = self.target_info.target
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopLife:ShowCreateEffectEffect()
    self.WaitNabe:SetActive(false)
    self.NabingProg:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
    
    self.EffectTool:Play("10010285")
    self.prog_step = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateCreateProgEffect, self), 0.1)
    self.ProtectArea:SetActive(true)
end

function WorkshopLife:UpdateCreateProgEffect()
    if self.ProgSlider == nil then return end 
    local value_prog = self.ProgSlider.value
    local cur_prog = math.floor(value_prog * 100)
    if self.ProText~= nil then 
        UH.SetText(self.ProText,cur_prog.."%")
    end 
end

function WorkshopLife:CanelCreateEffect()
    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
    self.WaitNabe:SetActive(true)
    self.NabingProg:SetActive(false)

    self.ctrl:SendCompoundReq(self.target_info.seq, self.make_num, 0)
    
    self.ProtectArea:SetActive(false)
end