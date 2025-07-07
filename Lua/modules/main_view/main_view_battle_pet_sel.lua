----------------MainViewBattlePetSel 宠物选择框------------

MainViewBattlePetSel = MainViewBattlePetSel or DeclareMono("MainViewBattlePetSel", UIWFlushPanel)
---PetList====UIWidgetList

function MainViewBattlePetSel:MainViewBattlePetSel()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = self.battle_data:PetList(), func = self.FlushPetList}
    }

    GuideManager.Instance:RegisterGetGuideUi("BattlePetItem1", function()
        if self.PetList:SetupCompleted() then
            return self.PetList.item_list[1].EvenClick
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattlePetItem2", function()
        if self.PetList:SetupCompleted() then
            return self.PetList.item_list[2].EvenClick
        end
	end)
end

function MainViewBattlePetSel:FlushPetList()
    local data = self.battle_data:PetList()
    data = table.rearrange(data)
    local function sort_func(a,b)
        if a.status == b.status then
            if a.score == b.score then
                if a.pet_level == b.pet_level then
                    if a.pet_str_level == b.pet_str_level then
                        return a.bag_idx > b.bag_idx
                    else
                        return a.pet_str_level > b.pet_str_level
                    end
                else
                    return a.pet_level > b.pet_level
                end
            else
                return a.score > b.score
            end
        else
            return a.status < b.status
        end
    end
    table.sort(data, sort_func)
    self.PetList:SetData(data)
end

function MainViewBattlePetSel:OnSelPet(pet_data)
    if pet_data.status ~= BATTLE_PET_STATUS.Dai
    and pet_data.status ~= BATTLE_PET_STATUS.Wang then
        PublicPopupCtrl.Instance:DialogTips(
            {
                content = Language.Main.Battle.PetSelTip,
                cancel = {
                    func = function()
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                },
                confirm = {
                    func = function()
                        BattleCtrl.Instance:SetMove(self.battle_data:BaseData().command_i, BATTLE_MOVE_TYPE.CHANGE_PET, -1)
                        PublicPopupCtrl.Instance:CloseDialogTips()
                        if self.battle_data:ViewInfo().opened_sub == BattleSubView.Pet then
                            self.battle_data:ViewInfo().opened_sub = BattleSubView.None
                        end
                    end
                }
            }
        )
    elseif pet_data.status == BATTLE_PET_STATUS.Wang then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.PetWangTip)
    else
        BattleCtrl.Instance:SetMove(self.battle_data:BaseData().command_i, BATTLE_MOVE_TYPE.CHANGE_PET, pet_data.bag_idx)
        if self.battle_data:ViewInfo().opened_sub == BattleSubView.Pet then
            self.battle_data:ViewInfo().opened_sub = BattleSubView.None
        end
    end
end

function MainViewBattlePetSel:OnClickClose()
    self.battle_data:ViewInfo().opened_sub = BattleSubView.None
end

MainViewBattlePetSelItem = MainViewBattlePetSelItem or DeclareMono("MainViewBattlePetSelItem", UIWidgetBaseItem)
-------Name========Text
-------Hp==========Slider
-------Mp==========Slider
-------State=======UIImageSpriteSet
-------Lv==========Text
-------Ele=========UIWElementAttribute
-------HeadCell====UIWSubstitute[UIWPetHeadCell]
BATTLE_PET_STATUS = {
    Zhan = 0,
    Dai = 1,
    Wang = 2,
}

local function getPetStatusSpriteName(status)
    for k, v in pairs(BATTLE_PET_STATUS) do
        if status == v then
            return "_loc" .. k
        end
    end
end

function MainViewBattlePetSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local icon_id = Item.GetConfig(data.pet_id).icon_id
    if data.skin_id and data.skin_id ~= 0 then
        local co = Cfg.PetSkinAttr(data.skin_id)
        if co and co.head_portrait then
            icon_id = co.head_portrait
        end
    end
    self.HeadCell:SetData({
        pet_id = data.pet_id,
        lv = data.pet_level,
        fs_lv = data.fly_flag,
        str_lv = data.pet_str_level,
        skin_id = data.skin_id,
    })
    -- UH.SetIcon(self.Icon, icon_id)
    if data.lianyao_pet_id and data.lianyao_pet_id > 0 then
        if self.YaoObj then
            self.YaoObj:SetActive(true)
        end
        UH.SetText(self.Name, "")
        UH.SetText(self.Name2, Item.GetName(data.pet_id))
    else
        if self.YaoObj then
            self.YaoObj:SetActive(false)
        end
        UH.SetText(self.Name, Item.GetName(data.pet_id))
    end
    self.Ele:SetData(data.element)
    if data.status == BATTLE_PET_STATUS.Wang then
        self.Hp.value = 0
        self.Mp.value = 0
    else
        self.Hp.value = data.maxhp ~= 0 and data.curhp / data.maxhp or 1
        self.Mp.value = data.maxmp ~= 0 and data.curmp / data.maxmp or 1
    end
    
    -- self.State.SpriteName = data.status == 0 and "_LocZhan" or "_LocShi"
    self.State.SpriteName = getPetStatusSpriteName(data.status)
    self.ItHdl.Interactable = data.status ~= BATTLE_PET_STATUS.Wang
    self.Lv.text = string.format("Lv.%d", data.pet_level)
    -- local pet = PetData.Instance:GetPetInfo(data.bag_idx)
    -- LogError(pet:StrLevel())
    -- PetData.SetStrLevel(nil, self.Jie, pet:StrLevel())
    PetData.SetStrLevel(nil, self.Jie, data.pet_str_level)
    UH.SetText(self.BtnText, Language.Main.Battle.PetBattleBtnText[data.status])
    self.CallOut:SetActive(data.status ~= BATTLE_PET_STATUS.Zhan)
    self.CallBack:SetActive(data.status == BATTLE_PET_STATUS.Zhan)
end
