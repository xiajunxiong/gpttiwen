-- =============== 神印强化 ===============
PetStampIntensifyView = PetStampIntensifyView or DeclareView("PetStampIntensifyView", "pet/pet_stamp/pet_stamp_intensify")
function PetStampIntensifyView:PetStampIntensifyView()
    self.BG:SetData(self:GetType(),Language.PetStampIntensify.Title,Vector2.New(914, 640))
    self.Currency:DefaultSet()
end


PetStampIntensifyPanel = PetStampIntensifyPanel or DeclareMono("PetStampIntensifyPanel", UIWFlushPanel)
function PetStampIntensifyPanel:PetStampIntensifyPanel()
    self.data = PetStampData.Instance
    self.data_cares = {
        {data = self.data.intensify_put_exp_data, func = self.FlushInfo, init_call = false},
        {data = self.data.pet_stamp_list, func = self.FlushPanel, init_call = false},
    }

    self.sel_type = 1
    self.sort_type = 2

    self.max_stuff_count = PetStampCfg.IntensifyStuffMaxCount    -- 最大可选择的狗粮的数量
    self.cur_put_count = 0
    self.sel_stuff_list = {}
    self.sel_exp_stuff_list = {}
end

function PetStampIntensifyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.sel_pet_stamp = self.data:GetIntensifyStamp()
    self.FirstSeletor.isOn = true
    self:FlushPanel()
end

function PetStampIntensifyPanel:OnDestroy()
    self.data:SetIntensifyStamp(nil)
    self.data.intensify_put_exp_data:Set({all_exp = 0, exp = 0, item_exp = 0, stuff_exp = 0})
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.touch_ht)
end

function PetStampIntensifyPanel:FlushPanel()
    self:FlushInfo()
    self:FlushGridList()
end

function PetStampIntensifyPanel:FlushGridList()
    local grid_list = self.data:GetIntensifyList(self.sel_pet_stamp,self.sel_type, self.sort_type)
    table.insert(grid_list,1,{is_exp = true})
    self.GridList:SetData(grid_list)
end

function PetStampIntensifyPanel:FlushInfo()
    if not self.sel_pet_stamp then
        return
    else
        local index = self.sel_pet_stamp:Index()
        self.sel_pet_stamp = self.data:GetPetStampByIndex(index)
    end
    self.StampCell:SetData(self.sel_pet_stamp)
    UH.SetText(self.Name, self.sel_pet_stamp:Name())
    local cur_level = self.sel_pet_stamp:Level()
    local put_exp = self.data.intensify_put_exp_data.all_exp or 0
    local up_level = self.data:GetExpToLevel(self.sel_pet_stamp, put_exp)
    up_level = up_level > self.sel_pet_stamp:MaxLevel() and self.sel_pet_stamp:MaxLevel() or up_level
    UH.SetText(self.Level, string.format("+%s",cur_level))
    UH.SetText(self.CurLv, string.format("+%s",cur_level))
    UH.SetText(self.UpLv, string.format("+%s",up_level))
    local intensify_cfg = Cfg.PetStampIntensifyCfg(self.sel_pet_stamp:QuaLevel(), cur_level)

    local all_exp = self.data.intensify_put_exp_data.all_exp or 0
    local stamp_exp = self.sel_pet_stamp:Exp() or 0
    local cfg_exp = intensify_cfg.exp or 1
    
    --满级状态
    if self.sel_pet_stamp:IsFullLevel() then
        self.ProgressEffectObj:SetLocalPosition(Vector3.New(-157,1,0))
        self.Interactor.Interactable = false
        self.Progress:SetText(Language.PetStamp.FullLevel)
    else
        local x = -157 - (cfg_exp - stamp_exp)/cfg_exp * 314
        self.ProgressEffectObj:SetLocalPosition(Vector3.New(x,1,0))
        self.Interactor.Interactable = true
        self.Progress:SetText(string.format("%s/%s",(all_exp + stamp_exp),cfg_exp))
    end

    local progress_num
    if all_exp > 0 then
        progress_num = (stamp_exp + all_exp) / cfg_exp
    else
        progress_num = self.sel_pet_stamp:IsFullLevel() and 1 or stamp_exp / cfg_exp
    end
    if self.last_exp and self.last_exp ~= stamp_exp then
        if self.last_level and cur_level > self.last_level then
            self.Progress:AddToProgress(1,function ()
                self.Progress:AddToProgress(progress_num)
            end)
        else
            self.Progress:AddToProgress(progress_num)
        end
    else
        self.Progress:SetProgress(progress_num)
    end
    self.last_exp = self.sel_pet_stamp:Exp()
    self.last_level = cur_level


    -- local coin_num = self.data.intensify_put_exp_data.exp * self.data.stamp_cfg.other[1].need_coin_100_exp / 100
    -- coin_num = math.floor(coin_num)
    -- UH.SetText(self.CoinNum, coin_num)

    local attri_list = self.data:GetIntensifyAddition(self.sel_pet_stamp, up_level)
    self.AttriList:SetData(attri_list)
    local max_exp = self.data:GetPetStampNeedMaxExp(self.sel_pet_stamp)
    local use_exp = put_exp > max_exp and max_exp or put_exp
    self.need_coin = use_exp * self.data.stamp_cfg.other[1].need_coin_100_exp / 100
    UH.SetText(self.CoinNum, self.need_coin)

    local old_attri
    if cur_level == up_level then
        old_attri = attri_list
    else
        old_attri = self.data:GetIntensifyAddition(self.sel_pet_stamp, cur_level)
    end
    if self.old_attri and old_attri then
        for i,v in pairs(old_attri) do
            if self.old_attri[i] == nil or self.old_attri[i].attri_type == nil or old_attri[i].attri_value > self.old_attri[i].attri_value then
                local index = self.AttriList:GetSelectedIndex(v)
                if index then
                    local mono = self.AttriList:GetItem(index)
                    mono:PlayChangeEffect(true)
                end
            end
        end
    end
    if old_attri then
        self.old_attri = old_attri
    end

    --local item_attri_info = self.sel_pet_stamp:AttriListInfo()
    local add_attri_list = self.sel_pet_stamp:AddAttri() or {}
    local add_count = 0
    for i,v in pairs(add_attri_list) do
        add_count = add_count + 1
    end
    local is_show_add_tip = add_count >= PetStampCfg.MaxAddAttriCount and not self.sel_pet_stamp:IsFullLevel()
    self.AdditionTip:SetObjActive(is_show_add_tip)
    if is_show_add_tip then
        local next_add_level = self.sel_pet_stamp:NextAdditionLevel()
        self.AdditionTip:SetObjActive(next_add_level ~= nil)
        if next_add_level then
            UH.SetText(self.AdditionTip, string.format(Language.PetStamp.Tip8,next_add_level))
        end
    end
end

function PetStampIntensifyPanel:PutPetStamp(pet_stamp,value)
    if value and self.cur_put_count >= self.max_stuff_count then
        return
    elseif not value and self.cur_put_count <= 0 then
        return
    end
    if value then
        self.cur_put_count = self.cur_put_count + 1
        self.sel_stuff_list[pet_stamp] = value
    else
        self.cur_put_count = self.cur_put_count - 1
        self.sel_stuff_list[pet_stamp] = nil
    end
end

function PetStampIntensifyPanel:PutExpStuff(item,num)
    if num and num < 0 then
        num = nil
    end
    self.sel_exp_stuff_list[item] = num
end

function PetStampIntensifyPanel:GetStuffPrice(pet_stamp_list)
    local exp = 0
    for i,v in pairs(pet_stamp_list) do
        exp = exp + self.data:GetStampPriceExp(i)
    end
    return exp
end

function PetStampIntensifyPanel:GetExpStuffPrice(exp_stuff_list)
    local exp = 0
    for i,v in pairs(exp_stuff_list) do
        exp = exp + self.data:GetStuffPriceExp(i, v)
    end
    return exp
end

function PetStampIntensifyPanel:OnExpClick()
    if self.sel_pet_stamp:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetStampIntensify.Error1)
        return
    end
    ViewMgr:OpenView(PetStampIntensifyDialog,self.sel_pet_stamp)
end

function PetStampIntensifyPanel:OnItemSelClick(mono)
    local data = mono:GetData()
    if data:Type() == ItemType.PetStamp then
        self:OnStampItemEvent(mono)
    else
        self:OnExpItemEvent(mono)
    end
end

-- 点击到神印事件
function PetStampIntensifyPanel:OnStampItemEvent(mono)
    local new_sel = not mono.sel
    local data = mono:GetData()
    -- 检查下能不能选
    if new_sel then
        local max_exp = self.data:GetPetStampNeedMaxExp(self.sel_pet_stamp)
        if self.sel_pet_stamp:IsFullLevel() then
            new_sel = false
            PublicPopupCtrl.Instance:Error(Language.PetStampIntensify.Error1)
            return
        elseif self.cur_put_count >= self.max_stuff_count then -- 数量是否太多
            new_sel = false
            PublicPopupCtrl.Instance:Error(string.format(Language.PetStampIntensify.Error2,self.max_stuff_count))
            return
        elseif self.data.intensify_put_exp_data.all_exp >= max_exp then -- 经验是否太多
            new_sel = false
            PublicPopupCtrl.Instance:Error(string.format(Language.PetStampIntensify.Error3,self.data.intensify_put_exp_data.all_exp))
            return
        end
    end
    mono:SetSel(new_sel)
    self:PutPetStamp(data, new_sel)
    local price_exp = self:GetStuffPrice(self.sel_stuff_list)
    self.data.intensify_put_exp_data.item_exp = price_exp
    self.data.intensify_put_exp_data.all_exp = self.data.intensify_put_exp_data.exp + self.data.intensify_put_exp_data.stuff_exp + price_exp
    self:FlushInfo()
end

-- 点击到经验材料事件
function PetStampIntensifyPanel:OnExpItemEvent(mono)
    local data = mono:GetData()
    local max_exp = self.data:GetPetStampNeedMaxExp(self.sel_pet_stamp)

    if self.sel_pet_stamp:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetStampIntensify.Error1)
        return
    elseif self.data.intensify_put_exp_data.all_exp >= max_exp then -- 经验是否太多
        PublicPopupCtrl.Instance:Error(string.format(Language.PetStampIntensify.Error3,self.data.intensify_put_exp_data.all_exp))
        TimeHelper:CancelTimer(self.touch_ht)
        return
    end
    -- 看他可放几个材料
    -- local num = 0
    -- if max_exp then
    --     local can_exp = max_exp - self.data.intensify_put_exp_data.all_exp
    --     local item_price_exp = self.data:GetStuffPriceExp(data, 1)  -- 一个道具值多少经验
    --     num = math.ceil(can_exp / item_price_exp)
    --     num = num > data.num and data.num or num
    -- end

    -- if num <= 0 then
    --     TimeHelper:CancelTimer(self.touch_ht)
    --     PublicPopupCtrl.Instance:Center("填入的经验已满")
    --     return
    -- end

    local cur_num = self.sel_exp_stuff_list[data] or 0

    if cur_num == data:Num() then
        PublicPopupCtrl.Instance:Center(Language.PetStampIntensify.Tip5)
        TimeHelper:CancelTimer(self.touch_ht)
        return
    end
    self:PutExpStuff(data, cur_num + 1)
    local price_exp = self:GetExpStuffPrice(self.sel_exp_stuff_list)
    self.data.intensify_put_exp_data.stuff_exp = price_exp
    self.data.intensify_put_exp_data.all_exp = self.data.intensify_put_exp_data.exp + self.data.intensify_put_exp_data.item_exp + price_exp
    self:FlushInfo()

    mono:SetSel(true, self.sel_exp_stuff_list[data])
end

function PetStampIntensifyPanel:OnPutStuffDown(mono)
    TimeHelper:CancelTimer(self.touch_ht)
	self.touch_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnExpItemEvent,self,mono),0.2)
end

function PetStampIntensifyPanel:OnUnPutStuffClick(mono)
    local data = mono:GetData()
    local cur_num = self.sel_exp_stuff_list[data] or 0
    cur_num = cur_num - 1 < 0 and 0 or cur_num - 1
    self:PutExpStuff(data, cur_num)
    local price_exp = self:GetExpStuffPrice(self.sel_exp_stuff_list)
    self.data.intensify_put_exp_data.stuff_exp = price_exp
    self.data.intensify_put_exp_data.all_exp = self.data.intensify_put_exp_data.exp + self.data.intensify_put_exp_data.item_exp + price_exp
    self:FlushInfo()

    mono:SetSel(cur_num > 0, cur_num)
end

function PetStampIntensifyPanel:OnPutStuffUp()
    TimeHelper:CancelTimer(self.touch_ht)
end


function PetStampIntensifyPanel:OnSortSelClick(sel_type, sort_type)
    --if self.sel_type ~= sel_type or self.sort_type ~= sort_type then
        self.sel_type = sel_type
        self.sort_type = sort_type
        self:FlushGridList()
    --end
end

function PetStampIntensifyPanel:OnIntensifyClick()
    if self.sel_pet_stamp:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetStampIntensify.Error1)
        return
    end
    if self.data.intensify_put_exp_data.all_exp < 1 then
        PublicPopupCtrl.Instance:Error(Language.PetStampIntensify.Error4)
        return
    end
    if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self.need_coin) then
        return
    end
    if self.data.intensify_put_exp_data.exp > 0 then
        PetStampCtrl.Instance:IntensifyByExp(self.sel_pet_stamp, self.data.intensify_put_exp_data.exp)
    end
    if self.data.intensify_put_exp_data.stuff_exp > 0 then
        PetStampCtrl.Instance:IntensifyByExpItem(self.sel_pet_stamp, self.sel_exp_stuff_list)
    end
    if self.data.intensify_put_exp_data.item_exp > 0 then
        PetStampCtrl.Instance:IntensifyByItem(self.sel_pet_stamp, self.sel_stuff_list)
    end
    self.cur_put_count = 0
    self.sel_stuff_list = {}
    self.sel_exp_stuff_list = {}
    self.data.intensify_put_exp_data:Set({all_exp = 0, exp = 0, item_exp = 0, stuff_exp = 0})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    --self:PlayAttiChangeEffect()
end

function PetStampIntensifyPanel:PlayAttiChangeEffect()
    local attri_item_list = self.AttriList.item_list
    for i,v in pairs(attri_item_list) do
        v:PlayChangeEffect()
    end
end

PetStampIntensifyItem = PetStampIntensifyItem or DeclareMono("PetStampIntensifyItem", UIWidgetBaseItem)
function PetStampIntensifyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    if self.ItemType then
        self.ItemType:SetActive(not data.is_exp)
    end
    if self.ExpType then
        self.ExpType:SetActive(data.is_exp == true)
    end

    if not data.is_exp then
        self.Cell:SetData(data)
        self.StampSel:SetActive(data:Type() == ItemType.PetStamp)
        self.StuffSel:SetActive(data:Type() ~= ItemType.PetStamp)
        if data:Type() == ItemType.PetStamp then
            self.sel = self.BasePanel.sel_stuff_list[data] or false
            self:SetSel(self.sel)
        else
            local num = self.BasePanel.sel_exp_stuff_list[data] or 0
            self.sel = num > 0
            self:SetSel(self.sel, num)
        end
    end
end


function PetStampIntensifyItem:SetSel(value, sel_num)
    if self.data.is_exp == true then
        return
    end
    self.sel = value
    self.SelImg:SetActive(value)

    local num = self.data:Num()
    if value and sel_num and sel_num > 0 and self.data:Type() ~= ItemType.PetStamp then
        CellFlushs.SetNum(self.Cell, string.format("%s/%s",sel_num,num))
        self.RemoveBtn:SetActive(true)
    else
        CellFlushs.SetNum(self.Cell, num)
        self.RemoveBtn:SetActive(false)
    end
end

-- 神印强化经验选择
PetStampIntensifyDialog = PetStampIntensifyDialog or DeclareView("PetStampIntensifyDialog", "pet/pet_stamp/pet_stamp_intensify_dialog")
function PetStampIntensifyDialog:PetStampIntensifyDialog()
    self.data = PetStampData.Instance
    self.exp = 0
end

function PetStampIntensifyDialog:LoadCallback(pet_stamp)
    self.data.intensify_put_exp_data.all_exp = self.data.intensify_put_exp_data.all_exp - self.data.intensify_put_exp_data.exp
    self.data.intensify_put_exp_data.exp = 0
    UH.SetText(self.Title, Language.PetStampIntensify.Title2)
    self.pet_stamp = pet_stamp
    self.max_exp = self.data:GetPetStampNeedMaxExp(pet_stamp)
    local level_list = self:GetLevelList(pet_stamp)
    self.LevelList:SetData(level_list)
    self.Slider:SetChangedEvent(BindTool.Bind(self.SliderEvent,self))
    self.Slider:SetOffEvent(BindTool.Bind(self.SliderOffEvent,self))
    self:FlushProgress(true)
end

function PetStampIntensifyDialog:GetLevelList(pet_stamp)
    -- local max_level = pet_stamp:MaxLevel()
    -- local level = self.data:GetExpToLevel(self.pet_stamp, self.data.intensify_put_exp_data.item_exp) 
    -- local max_count = 5
    -- local num = math.floor(max_level / max_count)
    -- num = num < 1 and 1 or num
    -- local list = {}
    -- for i = 1,max_count do
    --     local data = {}
    --     if i == max_count then
    --         data.level = max_level
    --     else
    --         data.level = i*num
    --     end
    --     data.is_gray = data.level <= level
    --     table.insert(list, data)
    -- end
    -- return list

    -- ========== 第二版 固定数值 ==========
    local level = self.data:GetExpToLevel(self.pet_stamp, self.data.intensify_put_exp_data.item_exp)
    local list = {}
    for i = 1, 5 do
        local data = {}
        data.level = i * 4
        data.is_gray = data.level <= level
        table.insert(list, data)
    end
    return list
end

function PetStampIntensifyDialog:OnLevelClick(data)
    local level, limit_type = self.data:GetExpToLevel(self.pet_stamp, self.data:GetExp())
    if data.level > level then
        if limit_type == 1 then
            PublicPopupCtrl.Instance:Center(string.format(Language.PetStampIntensify.Tip1,level))
        else
            PublicPopupCtrl.Instance:Center(string.format(Language.PetStampIntensify.Tip2,level))
        end
        self.exp = self.data:GetExp()
    else
        local exp = self.data:GetIntensifyNeedExp(self.pet_stamp, data.level)
        self.exp = exp
    end
    self:FlushProgress(true)
    ViewMgr:CloseView(PetStampIntensifyDialog)
end

function PetStampIntensifyDialog:OnCloseClick()
    ViewMgr:CloseView(PetStampIntensifyDialog)
end

function PetStampIntensifyDialog:SliderEvent()
    if self.is_set then
        self.is_set = false
        return
    end
    local slider_value = self.Slider:GetValue()
    local bag_exp = self.data:GetExp()
    bag_exp = bag_exp > self.max_exp and self.max_exp or bag_exp
    self.exp = math.ceil(bag_exp * slider_value)
    self:FlushProgress()
end

function PetStampIntensifyDialog:SliderOffEvent()
    self:FlushProgress(true)
end

function PetStampIntensifyDialog:FlushProgress(is_flush_slider)
    local bag_exp = self.data:GetExp()
    --bag_exp = bag_exp > self.max_exp and self.max_exp or bag_exp
    local limit_exp = bag_exp > self.max_exp and self.max_exp or bag_exp
    -- 最大经验限制
    if self.exp + self.data.intensify_put_exp_data.all_exp > self.max_exp then
        self.exp = self.max_exp - self.data.intensify_put_exp_data.all_exp
    end
    UH.SetText(self.ProgressText, string.format(Language.PetStampIntensify.Tip4, self.exp,bag_exp))
    if not is_flush_slider then
        return
    end
    if bag_exp <= 0 then
        self.Slider:SetValue(0)
    else
        self.is_set = true  -- 是手动设置的滑动值，不是拖动
        self.Slider:SetValue(self.exp / limit_exp)
    end
    
end

function PetStampIntensifyDialog:CloseCallback()
    self.data.intensify_put_exp_data.exp = self.exp
    self.data.intensify_put_exp_data.all_exp = self.exp + self.data.intensify_put_exp_data.item_exp + self.data.intensify_put_exp_data.stuff_exp
end

PetStampIntensifyLevelItem = PetStampIntensifyLevelItem or DeclareMono("PetStampIntensifyLevelItem", UIWidgetBaseItem)
function PetStampIntensifyLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Text, data.level)
    self.Interactor.Interactable = not data.is_gray
end

PetStampIntensifyAttriItem = PetStampIntensifyAttriItem or DeclareMono("PetStampIntensifyAttriItem", UIWidgetBaseItem)
--data.type = 1 是主属性, 2是副属性
function PetStampIntensifyAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local title_str = data.type == 1 and Language.PetStampIntensify.Attri1 or Language.PetStampIntensify.Attri2
    UH.SetText(self.Title, title_str)
    
    if data.attri_type then
        local name_str = Language.Common.AttrList[data.attri_type] or ""
        UH.SetText(self.Name, name_str)

        local value = data.attri_value
        if DataHelper.IsPercent(data.attri_type) then
            value = string.format("+%s%%",data.attri_value/10)
            UH.SetText(self.AddValue, string.format("+%s%%",(data.add_value.max or 0) / 10))
        else
            UH.SetText(self.AddValue, string.format("+%s",(data.add_value.max or 0)))
        end
        UH.SetText(self.Value, value)
        data.add_value = data.add_value or {}
        --UH.SetText(self.AddValue, string.format("+%s",(data.add_value.max or 0)))
        self.AddTipObj:SetActive(false)
        self.UpTip:SetActive(data.add_value.min ~= nil and data.add_value.min > 0)
        self.last_attri_type = true
    else
        UH.SetText(self.Name, "")
        UH.SetText(self.Value, "")
        UH.SetText(self.AddValue, "")
        self.AddTipObj:SetActive(true)
        self.UpTip:SetActive(false)
        self.last_attri_type = false
    end
end

function PetStampIntensifyAttriItem:PlayChangeEffect(is_play)
    if is_play == true or (self.data.add_value and self.data.add_value.max and self.data.add_value.max > 0) then
        self.EffectTool:Play(3161024)
    end
end