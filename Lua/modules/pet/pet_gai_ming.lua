PetGaiMing = PetGaiMing or DeclareView("PetGaiMing", "pet/pet_gai_ming", Mod.Pet.PetGaiMing)
function PetGaiMing:PetGaiMing()
    self.Board:SetData(self:GetType(), Language.PetGaiMing.Title)
end
function PetGaiMing:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end
function PetGaiMing:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

PetGaiMingPanel = PetGaiMingPanel or DeclareMono("PetGaiMingPanel", UIWFlushPanel)
function PetGaiMingPanel:PetGaiMingPanel()
    self.data = PetData.Instance
    self.data_cares = {
        {data  = self.data:GetPetSelect(), func = self.FlushBaseInfo},
    }
    GuideManager.Instance:RegisterGetGuideUi("GaiMingUpOne",function ()
        return self.BtnOne
    end)
    GuideManager.Instance:RegisterGetGuideUi("GaiMingKuang",function ()
        return self.GaiMingKuang, function ()
            
        end
    end)
    self.v2 = Vector2.zero
    -- self.pos = Vector3.zero
    self.can_play_fly_effct = false
end
function PetGaiMingPanel:FlushBaseInfo()
    local data = self.data:GetGaiMingAttrs()
    self.List:SetData(data)
    self:FlushGaiMingInfo()
    self:FlushPointEffect()
end
function PetGaiMingPanel:PlayFlyEffect(end_func)
    local startPos = self.StartPoint.localPosition
    local endPos = self.EndPoint.localPosition
    local crossPos = self.CrossPoint.localPosition
    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, 0.5)
    self.EffectTool:Play(3161023)
    UH.LocalPos(self.FlyPoint, startPos)
    local time = 0
    Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    self.handle_runner = Runner.Instance:RunUntilTrue(
        function()
            if shoot_curve then
                time = time + Time.deltaTime
                if time >= 1 then
                    UH.LocalPos(self.FlyPoint, endPos)
                    if end_func then
                        end_func()
                    end
                    return true
                end
                UH.LocalPos(self.FlyPoint, shoot_curve:GetPos(time))
            end
        end)
end
function PetGaiMingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
end
function PetGaiMingPanel:FlushPointEffect()
    local pet =  self.data:GetCurSelect()
    local point = pet:SurplusGaiMingPoint()
    if self.cur_point == nil then
        self.cur_point = pet:SurplusGaiMingPoint()
    end
    if point ~= self.cur_point then
        self.EffectTool:Play(10010003)
        self.cur_point = pet:SurplusGaiMingPoint()
    end
end
function PetGaiMingPanel:FlushEndPointEffect(per)
    -- length = 288
    --[[ self.EndPoint:SetParent(self.BarParent)
    per = per - 0.5
    self.v2:Set(per * 288, 0)
    UH.AnchoredPosition(self.EndPoint, self.v2)
    self.EndPoint:SetParent(self.EndPointParent)
    if self.can_play_fly_effct == true then
        self:PlayFlyEffect()
        self.can_play_fly_effct = false
    end ]]
end
function PetGaiMingPanel:FlushGaiMingInfo()
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local config = self.data:GaiMingLevelInfo(info.level)
    local is_tupo = self.data:GaiMingIsTuPo(pet)
    
    UH.SetText(self.Level, info.level)
    UH.SetText(self.Point, pet:SurplusGaiMingPoint())
    local is_max = info.level == self.data:GaiMingMaxLevel()
    self.BtnInter.Interactable = not is_max
    if is_max then
        self.BtnTuPo:SetActive(true)
        self.BtnUpLevel:SetActive(false)
        UH.SetText(self.Value, "")
        self.BarValue.fillAmount = 1
        UH.SetText(self.UpDesc, Language.PetGaiMing.MaxLevel)
        self:FlushEndPointEffect(1)
    else
        self.BtnTuPo:SetActive(is_tupo)
        self.BtnUpLevel:SetActive(not is_tupo)
        UH.SetText(self.Value, string.format(Language.PetGaiMing.Num, info.exp, config.up_exp))
        local per = info.exp / config.up_exp
        self.BarValue.fillAmount = per 
        self:FlushEndPointEffect(per)
        UH.SetText(self.UpDesc, is_tupo and Language.PetGaiMing.TuPo or Language.PetGaiMing.Up)
    end
    --根据当前配置以及经验判断是否为突破
    local item, num, need_num
    if is_tupo then
        item = Item.Create({item_id = config.break_item})
        num = Item.GetNum(config.break_item)
        need_num = config.break_item_num
        self.RedTuPo:SetNum(num>0 and 1 or 0)
    else
        local item_id = self.data:GaiMingUpItem()
        item = Item.Create({item_id = item_id})
        num = Item.GetNum(item_id)
        need_num = 1
        self.RedOne:SetNum(num>0 and 1 or 0)
        self.RedTen:SetNum(num>0 and 1 or 0)
    end
    UH.SetText(self.CostNum, string.format(Language.PetGaiMing.CostNum, num >= need_num and QualityColorStr[ItemColor.Green] or QualityColorStr[ItemColor.Red], num, need_num))
    UH.SetText(self.ItemName, item:QuaName())
    self.CostCell:SetData(item)
end

function PetGaiMingPanel:GaiMingUplevel()
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local item_id = self.data:GaiMingUpItem()
    local is_tupo = self.data:GaiMingIsTuPo(pet)
    local config = self.data:GaiMingLevelInfo(info.level)
    if Item.GetNum(item_id) <= 0 and not is_tupo then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        local item = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    if is_tupo and Item.GetNum(config.break_item) < config.break_item_num then
        local item = Item.Create({item_id = config.break_item})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.UpGeneral2)
    if is_tupo then
        PetCtrl.Instance:GaiMingUplevel(pet.index, 1, 1)--突破也是这个
        self.can_play_fly_effct = true
        --[[ self:PlayFlyEffect(function ()
        end) ]]
    else
        --根据数量和经验判断需要进行多少次提升
        local num = Item.GetNum(item_id)
        local exp = config.up_exp - info.exp
        local use_exp = self.data:GaiMingUpExp()
        local time = math.floor(exp / use_exp)
        time = time <= 0 and 1 or time
        PetCtrl.Instance:GaiMingUplevel(pet.index, time)--突破也是这个
        self.can_play_fly_effct = true
        --[[ self:PlayFlyEffect(function ()
        end) ]]
    end
end

function PetGaiMingPanel:GaiMingUplevelOne()
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local item_id = self.data:GaiMingUpItem()
    local is_tupo = self.data:GaiMingIsTuPo(pet)
    local config = self.data:GaiMingLevelInfo(info.level)
    if Item.GetNum(item_id) <= 0 and not is_tupo then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        local item = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    --[[ if is_tupo and Item.GetNum(config.break_item) < config.break_item_num then
        local item = Item.Create({item_id = config.break_item})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end ]]
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.UpGeneral2)
    --[[ if is_tupo then
        PetCtrl.Instance:GaiMingUplevel(pet.index, 1)--突破也是这个
    else ]]
        --根据数量和经验判断需要进行多少次提升
        --[[ local num = Item.GetNum(item_id)
        local exp = config.up_exp - info.exp
        local use_exp = self.data:GaiMingUpExp()
        local time = math.floor(exp / use_exp)
        time = time <= 0 and 1 or time ]]
        PetCtrl.Instance:GaiMingUplevel(pet.index, 1)--突破也是这个
        self.can_play_fly_effct = true
        --[[ self:PlayFlyEffect(function ()
        end) ]]
   --[[  end ]]
end
function PetGaiMingPanel:GaiMingUplevelTen()
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local item_id = self.data:GaiMingUpItem()
    local is_tupo = self.data:GaiMingIsTuPo(pet)
    local config = self.data:GaiMingLevelInfo(info.level)
    if Item.GetNum(item_id) <= 0 and not is_tupo then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        local item = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    --[[ if is_tupo and Item.GetNum(config.break_item) < config.break_item_num then
        local item = Item.Create({item_id = config.break_item})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end ]]
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.UpGeneral2)
    --[[ if is_tupo then
        PetCtrl.Instance:GaiMingUplevel(pet.index, 1)--突破也是这个
    else ]]
        --根据数量和经验判断需要进行多少次提升
        --[[ local num = Item.GetNum(item_id)
        local exp = config.up_exp - info.exp
        local use_exp = self.data:GaiMingUpExp()
        local time = math.floor(exp / use_exp)
        time = time <= 0 and 1 or time ]]
        PetCtrl.Instance:GaiMingUplevel(pet.index, 10)--突破也是这个
        self.can_play_fly_effct = true
        --[[ self:PlayFlyEffect(function ()
        end) ]]
   --[[  end ]]
end
function PetGaiMingPanel:GaiMingAttrReset()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    if info.level == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.LevelZero)
        return
    end
    --[[ if pet:SurplusGaiMingPoint() == self.data:GaiMingMaxLevel() then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.LevelZero)
        return
    end ]]
    -- PetCtrl.Instance:GaiMingAttrReset(pet.index)
    ViewMgr:OpenView(PetGaiMingReset)
end
function PetGaiMingPanel:OnClickTip()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    PublicPopupCtrl.Instance:HelpTip(365)
end




PetGaiMingItem = PetGaiMingItem or DeclareMono("PetGaiMingItem", UIWidgetBaseItem)
function PetGaiMingItem:PetGaiMingItem()
    self.pet_data = PetData.Instance
end

function PetGaiMingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pet =  self.pet_data:GetCurSelect()
    if pet == nil then--#1535734 LuaException
        return
    end
    local info = pet:GetGaiMingInfo()
    local level = info.attr_level[data.seq + 1]
    UH.SetText(self.AttrType, data.show_name)--Language.AttrItems.AttrNames[data.att_type])
    local value = self.pet_data:GetGaiMingAttrValue(data.seq, level)
    UH.SetText(self.AttrValue, value)
    UH.SetText(self.Point, level)
    local surp = pet:SurplusGaiMingPoint()
    local maxp = self.pet_data:GaiMingMax()
    self.RedPoint:SetNum((surp > 0 and level < maxp) and 1 or 0)
end
function PetGaiMingItem:PlayEffect()
    self.EffectTool:Play(3161024, self.Effect2)
end
function PetGaiMingItem:OnClickAdd()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    local pet =  self.pet_data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local point = pet:SurplusGaiMingPoint()
    if point == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.PointTip)
        return
    end
    if info.attr_level[self.data.seq + 1] == self.pet_data:GaiMingMax() then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.PointMax)
        return
    end
    PetCtrl.Instance:GaiMingAttrUplevel(pet.index, self.data.seq)
    self:PlayEffect()
end

PetGaiMingReset = PetGaiMingReset or DeclareView("PetGaiMingReset", "pet/pet_gai_ming_reset")
VIEW_DECLARE_MASK(PetGaiMingReset, ViewMask.BgBlockClose)
function PetGaiMingReset:PetGaiMingReset()
    self.data = PetData.Instance
    self.Board:SetData(self:GetType(), Language.PetGaiMing.Title2, Vector2.New(678.2499,443.0522))
    self.handle_lis = BagData.Instance:ListenItems({CommonItemId.PetGaiMingReset},BindTool.Bind(self.FlushItem, self))
end
function PetGaiMingReset:LoadCallback()
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    local config = self.data:GaiMingLevelInfo(info.level)
    local item = Item.Create({item_id = config.chongzhi_item, num = tostring(Item.GetNum(config.chongzhi_item))})
    UH.SetText(self.Desc, string.format(Language.PetGaiMing.Reset, config.chongzhi_item_num, item:QuaName(), pet:Name()))
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName())
end
function PetGaiMingReset:FlushItem()
    local item_id = CommonItemId.PetGaiMingReset
    local item = Item.Create({item_id = item_id, num = tostring(Item.GetNum(item_id))})
    self.Cell:SetData(item)
end
function PetGaiMingReset:CloseCallback()
    BagData.Instance:RemoveListenItems(self.handle_lis)
end
function PetGaiMingReset:OnClickReset()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    local pet =  self.data:GetCurSelect()
    local info = pet:GetGaiMingInfo()
    if info.level == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.NoPoint)
        ViewMgr:CloseView(PetGaiMingReset)
        return
    end
    if pet:SurplusGaiMingPoint() == info.level then
        PublicPopupCtrl.Instance:Center(Language.PetGaiMing.NoPoint)
        ViewMgr:CloseView(PetGaiMingReset)
        return
    end
    local config = self.data:GaiMingLevelInfo(info.level)
    if Item.GetNum(CommonItemId.PetGaiMingReset) <= 0 or Item.GetNum(CommonItemId.PetGaiMingReset) < config.chongzhi_item_num then
        local item = Item.Create({item_id = CommonItemId.PetGaiMingReset})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    PetCtrl.Instance:GaiMingAttrReset(pet.index)
    ViewMgr:CloseView(PetGaiMingReset)
end