PetRefining = PetRefining or DeclareView("PetRefining", "pet_refining/pet_refining", Mod.PetRefining.Main)
VIEW_DECLARE_LEVEL(PetRefining, ViewLevel.L1)
function PetRefining:PetRefining()
    self.data = PetRefiningData.Instance
end
function PetRefining:LoadCallback()
    if UnityPlayerPrefs.GetInt(PrefKeys.LianYaoFirstOpen())== 0 then
        GuideManager.Instance:Start(ConstGuideId.LianYaoFirstOpen)
        UnityPlayerPrefs.SetInt(PrefKeys.LianYaoFirstOpen(), 1)
    end
end
function PetRefining:CloseCallback()
    self.data:Clear()
end

PetRefiningPanel = PetRefiningPanel or DeclareMono("PetRefiningPanel", UIWFlushPanel)
function PetRefiningPanel:PetRefiningPanel()
    self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.StoveChange, keys = {"stove_idx"}},
        {data = self.data:ViewData(), func = self.PetChange, keys = {"pet_1"}},
        {data = self.data:ViewData(), func = self.PetChange, keys = {"pet_2"}},
        {data = self.data:ViewData(), func = self.RefiningIng, keys = {"refining_ing"}, init_call = false},
        {data = self.data:ViewData(), func = self.RefiningStart, keys = {"has_result"}, init_call = false},
    }
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Default,
        })
    end
    GuideManager.Instance:RegisterGetGuideUi("ClickFrame", function ()
        return self.ClickFrame, function ()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BtnStart", function ()
        return self.BtnStart
    end)
    GuideManager.Instance:RegisterGetGuideUi("PreviewBtn", function ()
        return self.PreviewBtn,function ()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("LianYaoZhu", function ()
        return self.LianYaoZhu,function ()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("LianYaoFu", function ()
        return self.LianYaoFu,function ()
        end
    end)
end
function PetRefiningPanel:RefiningStart()
    ViewMgr:CloseView(PetRefiningLeft)
    ViewMgr:CloseView(PetRefiningRight)
    self.EffectTool:Play(3175103)
    self.data:ViewData().refining_ing = true
end
function PetRefiningPanel:RefiningIng()
    if self.data:ViewData().refining_ing == false then
        self.data:SetPet1(nil)
        self.data:SetPet2(nil)
        self.ui_obj:PlayAnim(ChAnimType.Idle)
    end
end
function PetRefiningPanel:PetChange()
    local pet1 = self.data:Pet1()
    local pet2 = self.data:Pet2()
    if pet1 ~= nil and pet2 ~= nil then
        self.BtnRef:SetActive(true)
        self.MoneyObj:SetActive(true)
        self.TipObj:SetActive(false)
    else
        self.TipObj:SetActive(true)
        self.BtnRef:SetActive(false)
        self.MoneyObj:SetActive(false)
    end
end
function PetRefiningPanel:onFlush()
    local data = self.data:LuZiList()
    local stove = self.data:Stove()
    self.List:SetData(data)
    if stove > 0 then
        self.List:ClickItem(stove)
    else
        self.List:ClickItem(1)
    end
    local unlock, total = self.data:UnlockCount()
    UH.SetText(self.LockTex, string.format(Language.PetRefining.UnlockTip, unlock, total))
    self.BtnPre:SetActive(false)
end
function PetRefiningPanel:OnClickStart()
    self.Anima:Play("clickstart")
    self.BtnStart:SetActive(false)
    self.BtnPre:SetActive(true)
    self.BtnTip:SetActive(true)
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        self.EffectTool:Play(3165111)
        self.EffectTool:Play(3165112)
    end,1.8)
end
function PetRefiningPanel:StoveChange()
    local stove = self.data:Stove()
    if stove == 0 then
        return
    end
   
    local list = self.data:LuZiList()
    local data  = list[stove]
    --正常默认选第一个
    --切换时清空选择的情况
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(data.res_id))
    self.ui_obj:PlayAnim(ChAnimType.Idle)
    --主副宠条件
    UH.SetText(self.CondTex1, string.format(Language.PetRefining.CondTex[data.main_max_quality], data.main_min_strengthen))
    UH.SetText(self.CondTex2, string.format(Language.PetRefining.CondTex2[data.second_max_quality], data.second_max_strengthen))
    UH.SetText(self.Desc1, data.liucheng_dec)
    UH.SetText(self.Desc2, data.result_dec)
    UH.SetText(self.Money, data.coin_cost)
    UH.SetText(self.TipTex, data.furnace_dec)
    UH.SetText(self.DescTitle, data.furnace_name)
end
function PetRefiningPanel:OnClickTip()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    ViewMgr:OpenView(PetRefiningTip)
end
function PetRefiningPanel:EffectEvent(handle, time, key)
    if self.ui_obj and key == "end" then
        self.ui_obj:PlayAnim(ChAnimType.BattleRun)
    end
    self.EffectTool:Stop(handle)
    self.handle_delay1 = TimeHelper:AddDelayTimer(function ()
        -- ViewMgr:OpenView(PetRefiningResult)
        -- self.ui_obj:PlayAnim(ChAnimType.Idle)
    end, 5.8)
end
function PetRefiningPanel:OnClickStove(cell)
    if self.data:ViewData().refining_ing == true then
        -- PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        self.List:ClickItem(self.data:Stove())
        return
    end
    --判断炉子是否开启
    --设置炉子的idx
    local data = cell:GetData()
    local unlock = self.data:IsUnlock(data)
    if not unlock then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.LockTip)
        self.List:ClickItem(self.data:Stove())
        return
    end
    self.data:ViewData().refining_ing = false
    self.data:SetStove(data.furnace_id)
end
function PetRefiningPanel:OnClickClose()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    if GuideManager.Instance:IsGuide() then
        return
    end
    ViewMgr:CloseView(PetRefining)
end
function PetRefiningPanel:OnClickPreview()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end    
    local pet = self.data:ViewData().pet_1
    local pet2 = self.data:ViewData().pet_2
    if pet == nil or pet2 == nil then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.SetPetTip)
        return
    end
    ViewMgr:OpenView(PetRefiningPreview)
end
function PetRefiningPanel:OnClickPet1()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    local list = self.data:MainPetList()
    if list == nil or table.nums(list) == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.NoCanSelect)
        return
    end
    if ViewMgr:IsOpen(PetRefiningRight) then
        if self.data:Pet2() ~= nil then
            ViewMgr:OpenView(PetRefiningLeft)
            self.data:ViewData().select_left = not self.data:ViewData().select_left
        else
            PublicPopupCtrl.Instance:Center(Language.PetRefining.SelecTip[2])
        end
    else
        ViewMgr:OpenView(PetRefiningLeft)
        self.data:ViewData().select_left = not self.data:ViewData().select_left
    end
end
function PetRefiningPanel:OnClickPet2()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    local list = self.data:SecPetList()
    if list == nil or table.nums(list) == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.NoCanSelect)
        return
    end
    --加入主宠界面打开了，那么就只能是设置了pet1才能打开
    if ViewMgr:IsOpen(PetRefiningLeft) then
        if self.data:Pet1() ~= nil then
            ViewMgr:OpenView(PetRefiningRight)
            self.data:ViewData().select_right = not self.data:ViewData().select_right
        else
            PublicPopupCtrl.Instance:Center(Language.PetRefining.SelecTip[1])
        end
    else
        ViewMgr:OpenView(PetRefiningRight)
        self.data:ViewData().select_right = not self.data:ViewData().select_right
    end
end
function PetRefiningPanel:OnClickRefining()
    if self.data:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    local pet1 = self.data:Pet1()
    local pet2 = self.data:Pet2()
    if pet1 == nil or pet2 == nil then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.SetPetTip)
        return
    end
    local func = function ()
        --判断钱够不够
        local info = self.data:LuZiList()[self.data:Stove()]
        if info.coin_cost > RoleData.Instance:GetCurrency(CurrencyType.CoinBind) then
            PetCtrl.SendPetOp(PET_OP_TYPE.PetLianYao, pet1:Index(), pet2:Index(), self.data:Stove())
            return
        end
        -- PetCtrl.SendPetOp(PET_OP_TYPE.PetLianYao, pet1:Index(), pet2:Index(), self.data:Stove())
    local info = {
        content = Language.PetRefining.Tip3,
        confirm = {
            func = function()
                -- LogDG(pet1:Index(), pet2:Index(), self.data:Stove())
                PetCtrl.SendPetOp(PET_OP_TYPE.PetLianYao, pet1:Index(), pet2:Index(), self.data:Stove())
                PublicPopupCtrl.Instance:CloseDialogTips()
            end,
            wait_time = 5
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    end
    if pet2.lock == 1 then
        ViewMgr:OpenView(PetRefiningDialog, {
            context = Language.PetRefining.Tip4,
            pet = pet2,
            ok_func = function ()
                PetCtrl.SendPetOp(PET_OP_TYPE.Lock, pet2.index)
                if pet2:State() == PetState.StandBy or pet2:State() == PetState.Fight then
                    PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet2.index,PET_STATE_OP.RELAX)
                end
                ViewMgr:CloseView(PetRefiningDialog)
                func()
            end
        })
        return
    end
    if pet2:State() == PetState.StandBy then
        ViewMgr:OpenView(PetRefiningDialog, {
            context = Language.PetRefining.Tip5,
            pet = pet2,
            ok_func = function ()
                PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet2.index,PET_STATE_OP.RELAX)
                ViewMgr:CloseView(PetRefiningDialog)
                func()
            end
        })
        return
    end
    if pet2:State() == PetState.Fight then
        ViewMgr:OpenView(PetRefiningDialog, {
            context = Language.PetRefining.Tip8,
            pet = pet2,
            ok_func = function ()
                PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet2.index,PET_STATE_OP.RELAX)
                ViewMgr:CloseView(PetRefiningDialog)
                func()
            end
        })
        return
    end
    if pet2:State() == PetState.Train then
        ViewMgr:OpenView(PetRefiningDialog, {
            context = Language.PetRefining.Tip7,
            pet = pet2,
            ok_func = function ()
                local data
                local list = GuildData.Instance:GetGuildPetList()
                for i,v in pairs(list) do
                    if v.pet_index == pet2.index then
                        data = v
                        break
                    end
                end
                if data then
                    GuildCtrl.SendGuildPetTraining({pet_index = pet2.index, places_index = data.index - 1})
                    ViewMgr:CloseView(PetRefiningDialog)
                    func()
                end
            end
        })
        return
    end
    func()
end
function PetRefiningPanel:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    self.data:ViewData().refining_ing = false
end

PetRefiningLuZiItem = PetRefiningLuZiItem or DeclareMono("PetRefiningLuZiItem", UIWidgetBaseItem)

function PetRefiningLuZiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local role_level = RoleData.Instance:GetRoleLevel()
    local fly_flag = RoleData.Instance:GetRoleAvatarQuality()
    UH.SpriteName(self.Icon, "LuZi".. data.furnace_id)
    UH.SetText(self.Name, data.furnace_name)
    self.Lock:SetActive(role_level < data.open_level)
    UH.SetText(self.LockTex, string.format(Language.PetRefining.LevelLock, data.open_level))
    --炉子4特殊判断
    self.Inter.Interactable = role_level >= data.open_level
    self.SelectObj:SetActive(role_level >= data.open_level)
    local unlock_flag = PetRefiningData.Instance:GetUnlockInfo(data.furnace_id)
    if data.pet_quality > 0 and role_level >= data.open_level then
       UH.SetText(self.LockTex, Format(Language.PetRefining.QuaLock, Language.Pet.QualityName[data.pet_quality], data.pet_strengthen)) 
       self.Lock:SetActive(unlock_flag == 0)
       self.Inter.Interactable = unlock_flag ~= 0
    end
    if data.feisheng_level > 0 then
        self.gameObject:SetActive(role_level >= data.open_level)
        self.Lock:SetActive(fly_flag < data.feisheng_level)
        UH.SetText(self.LockTex, Language.PetRefining.FlyupLock)
        self.Inter.Interactable = fly_flag >= data.feisheng_level
        self.SelectObj:SetActive(fly_flag >= data.feisheng_level)
    end
end

function PetRefiningLuZiItem:Click()
    self.Selector.isOn = true
    if PetRefiningData.Instance:ViewData().refining_ing == true then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.Refininging)
        return
    end
    self.EventClick:Trigger()
end

PetRefiningPet1Panel = PetRefiningPet1Panel or DeclareMono("PetRefiningPet1Panel", UIWFlushPanel)
function PetRefiningPet1Panel:PetRefiningPet1Panel()
    self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.FlushPetMsg, keys = {"pet_1"}}
    }
end
function PetRefiningPet1Panel:FlushPetMsg()
    local pet = self.data:Pet1()
    local flag = (pet ~= nil)
    self.PetCell:Clean()
    self.ClickTip:SetActive(flag)
    self.AddTip:SetActive(not flag)
    self.IconAdd:SetActive(not flag)
    if flag then
        self.PetCell:SetDataByPet(pet)
    end
end
function PetRefiningPet1Panel:FlushPetMsg1()
    local pet = self.data:Pet1()
    --有宠物的情况播一个动画然后显示
    --没有就复原清空
    if pet ~= nil then
        local main_skill = pet:GetMainSkillList({})
        local skills = pet:GetLearnedGiftList({})
        skills = pet:LianYaoSkillList(skills)
        if #skills <= 4 then
            self.List2:SetData(skills)
            self.List3:SetData({})
        else
            local list = {}
            local list2 = {}--每5个一组
            local l2idx = 1
            for i = 1, #skills do
                if i <= 4 then
                    table.insert(list, skills[i])
                else
                    l2idx = math.max(math.floor((i - 4) / 5), l2idx)
                    list2[l2idx] = list2[l2idx] or {}
                    table.insert(list2[l2idx], skills[i])
                end
            end
            self.List2:SetData(list)
            self.List3:SetData(list2)
        end
        self.List1:SetData(main_skill)
        local config = pet:ConfigGrowthList()
        local total = pet:TotalGrowthList()
        local index = 1
        for key, value in pairs(config) do
            index = PetData.GetZZIndex(key)
            UH.SetText(self.ValueTexts[index], string.format("%s/%s", total[key], value))
        end
    end
end
function PetRefiningPet1Panel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end

PetRefiningPet2Panel = PetRefiningPet2Panel or DeclareMono("PetRefiningPet2Panel", UIWFlushPanel)
function PetRefiningPet2Panel:PetRefiningPet2Panel()
    self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.FlushPetMsg, keys = {"pet_2"}}
    }
end
function PetRefiningPet2Panel:FlushPetMsg()
    local pet = self.data:Pet2()
    local flag = (pet ~= nil)
    self.PetCell:Clean()
    self.ClickTip:SetActive(flag)
    self.AddTip:SetActive(not flag)
    self.IconAdd:SetActive(not flag)
    if flag then
        self.PetCell:SetDataByPet(pet)
    end
end
function PetRefiningPet2Panel:FlushPetMsg1()
    local pet = self.data:Pet2()
    if pet ~= nil then
        local main_skill = pet:GetMainSkillList({})
        local skills = pet:GetLearnedGiftList({})
        skills = pet:LianYaoSkillList(skills)
        if #skills <= 4 then
            self.List2:SetData(skills)
            self.List3:SetData({})
        else
            local list = {}
            local list2 = {}--每5个一组
            local l2idx = 1
            for i = 1, #skills do
                if i <= 4 then
                    table.insert(list, skills[i])
                else
                    l2idx = math.max(math.floor((i - 4) / 5), l2idx)
                    list2[l2idx] = list2[l2idx] or {}
                    table.insert(list2[l2idx], skills[i])
                end
            end
            self.List2:SetData(list)
            self.List3:SetData(list2)
        end
        self.List1:SetData(main_skill)
        local config = pet:ConfigGrowthList()
        local total = pet:TotalGrowthList()
        local index = 1
        for key, value in pairs(config) do
            index = PetData.GetZZIndex(key)
            UH.SetText(self.ValueTexts[index], string.format("%s/%s", total[key], value))
        end
    end
end
function PetRefiningPet2Panel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end

PetRefiningTip = PetRefiningTip or DeclareView("PetRefiningTip", "pet_refining/pet_refining_tip")
function PetRefiningTip:PetRefiningTip()
    self.data = PetRefiningData.Instance
end
function PetRefiningTip:LoadCallback()
    local data = self.data:LuZiList()[self.data:Stove()]
    UH.SetText(self.Desc1, data.liucheng_dec)
    UH.SetText(self.Desc2, data.result_dec)
    UH.SetText(self.DescTitle, data.furnace_name) 
end
function PetRefiningTip:OnClickClose()
    ViewMgr:CloseView(PetRefiningTip)
end

--===============================LianYaoSub===============================
LianYaoSub = LianYaoSub or DeclareMono("LianYaoSub", UIWFlushPanel)

function LianYaoSub:LianYaoSub()
	self.data = PetRefiningData.Instance
    self.data_cares = {
	    {data = RoleData.Instance:GetBaseData(), func = self.FlushShowTip, init_call = false, keys = {"level"}},
	    {data = RoleData.Instance:GetBaseData(), func = self.FlushShowTip, init_call = false, keys = {"avatar_quality"}},
	    {data = self.data.pet_unlock_info, func = self.FlushShowTip, init_call = false},
        {data = LoginData.Instance.base_data, func = self.FlushShow, keys = {"login_state"}}
    }
    self.stove_id = 1
end
function LianYaoSub:FlushShow()
    self.Sub:SetActive(false)
end
function LianYaoSub:OnPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end
function LianYaoSub:FlushShowTip()
    local list = self.data:LuZiList()
    --当前level是开启等级时打开
    local is_open = false
    local level = RoleData.Instance:GetRoleLevel()
    local fly_flag = FlyUpData.Instance:GetStage()
    local idx = 1
    for i, v in ipairs(list) do
        if level and v.feisheng_level == 0 and v.open_level == level then
            if v.pet_quality > 0 then
            else
                is_open = true
                idx = i
            end
        end
        if fly_flag and level and level >= v.open_level and v.feisheng_level > 0 and fly_flag >= v.feisheng_level then
            is_open = true
            idx = i
        end
    end
    if is_open == false then
        for i, v in ipairs(list) do
            if level and v.feisheng_level == 0 and v.open_level <= level then
                if v.pet_quality > 0 then
                    if self.data:GetUnlockInfo(v.furnace_id) == 1 then
                        is_open = true
                        idx = i
                    end
                end
            end
        end
    end
    --[[ if is_open then
        MainViewData.Instance:ShowRbButton()
    end ]]
    self.stove_id = idx
    if is_open then
        if UnityPlayerPrefs.GetInt(PrefKeys.LianYaoLuziOpen(idx)) == 0 then
            MainViewData.Instance:ShowRbButton()
            self.Sub:SetActive(is_open)
            UnityPlayerPrefs.SetInt(PrefKeys.LianYaoLuziOpen(idx), 1)
        end
    else
        self.Sub:SetActive(is_open)
    end
    if idx > 0 then
        UH.SetText(self.TipText, list[idx].guide_dec)
    else
        UH.SetText(self.TipText,"")
    end
end

function LianYaoSub:Awake()
	UIWFlushPanel.Awake(self)
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.CourageChallenge.GoToTip,COLORSTR.Green3),"_ul",COLORSTR.Green3))
end

function LianYaoSub:OnClickEnter()
    self.data:SetStove(self.stove_id)
	ViewMgr:OpenView(PetRefining)
    self.Sub:SetActive(false)
end

function LianYaoSub:OnClickClose()
	self.Sub:SetActive(false)
end