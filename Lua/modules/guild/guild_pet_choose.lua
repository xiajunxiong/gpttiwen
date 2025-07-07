GuildPetChoose = GuildPetChoose or DeclareView("GuildPetChoose", "guild/guild_pet_choose")
function GuildPetChoose:GuildPetChoose()
    self.lang = Language.Guild
end
function GuildPetChoose:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab9, Vector2.New(1006, 520))
end

function GuildPetChoose:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

GuildPetChoosePanel = GuildPetChoosePanel or DeclareMono("GuildPetChoosePanel", UIWFlushPanel)
function GuildPetChoosePanel:GuildPetChoosePanel()
    self.data = GuildData.Instance
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function GuildPetChoosePanel:onFlush()
    local can_train_list = {}
    for k, v in pairs(PetData.Instance:GetPetList()) do
        if v:Level() < ROLE_MAX_LEVEL then
            table.insert(can_train_list, v)
        end
    end
    self.PetList:SetData(can_train_list)
    self.PetList:ClickItem(1)
end
function GuildPetChoosePanel:OnClickTrain()
    local cur_state = self.select_pet:State()
    if cur_state ~= PetState.Relax then
        PublicPopupCtrl.Instance:Error(Language.Guild.TrainPet[cur_state])
        return
    end
	local exp = RoleData.Instance:GetCurrency(CurrencyType.GuildCotribution)
	if exp < self.pet_gongxian_cost then
		PublicPopupCtrl.Instance:Error(ErrorText[92])
        return
	end
    --[[ local area = self.select_pet:ArenaState()
    if area then
        PublicPopupCtrl.Instance:Center(Language.Guild.TrainPet[PetState.Arena])
        return
    end ]]
    GuildCtrl.SendGuildPetTraining({pet_index = self.pet_index, places_index = self.data.places_index})
    ViewMgr:CloseView(GuildPetChoose)
end
function GuildPetChoosePanel:OnClickPet(data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self.pet_index = data.index
    self.select_pet = data
    local pet = data
    UH.SetText(self.Name, pet:Name())
    UH.SetText(self.Level, string.format(self.lang.LevelStr2, pet:Level()))

    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.Group, string.format(self.lang.GrowthStr, COLORSTR.Yellow13, COLORSTR.Yellow13, v1, v2))
    local need_exp = self.pet_data:GetLevelExp(pet:Level())
    self.ImgExp:SetProgress(pet:Exp() / need_exp)
    self.ImgExp:SetText(string.format(Language.Common.Progress2TxtColorDFormat, pet:Exp(), need_exp))--string.format("%d/%d", pet:Exp(), need_exp))
    local total_growth_list = pet:TotalGrowthList()
    local config_growth_list = pet:ConfigGrowthList()
    self.AttrList:SetPet(pet)
    local total_attr = self.pet_data:RealAttr(self.pet_index)
    if total_attr == nil then
        total_attr = pet:TotalAttr()
    end
    for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(pet:HP() / pet:HP())
            self.ProgMaxHp:SetText(string.format(Language.Common.Progress2TxtColorDFormat, pet:HP(), pet:HP()))--string.format("%d/%d", pet:HP(), pet:HP()))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(pet:MP() / pet:MP())
            self.ProgMaxMp:SetText(string.format(Language.Common.Progress2TxtColorDFormat, pet:MP(), pet:MP()))--string.format("%d/%d", pet:MP(), pet:MP()))
        else
            local obj = self["Lbl"..tostring(k)]
            if  obj ~= nil then
                UH.SetText(obj, v)
            end
        end
    end
    local tr_info = GuildData.Instance:GetTrainExpInfo(pet:Level())
    UH.SetText(self.GongXian, tr_info.gongxian_cost)
	self.pet_gongxian_cost = tr_info.gongxian_cost
end
GuildPetChooseItem = GuildPetChooseItem or DeclareMono("GuildPetChooseItem", UIWidgetBaseItem)
--PetHeadCell========[UIWSubstitute]
function GuildPetChooseItem:SetData(data)
    local state, is_follow = PetData.Instance:GetPetState(data.index)
    UH.SpriteName(self.State, PetData.State[state])

    -- UH.SetIcon(self.Icon, data:IconId())
    -- UH.SpriteName(self.QuaIcon, data:QuaIcon())
    self.PrayFlag:SetActive(data:PrayMainIndex() ~= nil)
    UIWidgetBaseItem.SetData(self, data)
    -- UH.SetText(self.Level, data.level)
    -- PetData.SetStrLevel(nil, self.StrLevel, data.str_level)
    self.PetHeadCell:SetDataByPet(data)
end

function GuildPetChooseItem:Click()
    self.ItemClick:Trigger()
end
