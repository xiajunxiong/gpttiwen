RewardPetIntensify = RewardPetIntensify or DeclareMono("RewardPetIntensify", UIWFlushPanel)

function RewardPetIntensify:RewardPetIntensify()
	self.data = RewardData.Instance
	self.data_cares = {
		{data = self.data.pet_intensify_data,  func = self.FlushPanel, init_call = false}
	}
end

function RewardPetIntensify:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function RewardPetIntensify:FlushPanel()
    local list_data = self.data:GetPetIntensifyList()
    list_data = TableCopy(list_data)
    table.sort(list_data, function (a,b)
        local a_flag = self.data:PetIntensifyRewardFlag(a)
        local b_flag = self.data:PetIntensifyRewardFlag(b)
        if a_flag == b_flag then
            return a.seq < b.seq
        end
        return a_flag > b_flag
    end)
	self.RewardList:SetData(list_data)
end



RewardPetIntensifyItem = RewardPetIntensifyItem or DeclareMono("RewardPetIntensifyItem", UIWidgetBaseItem)
function RewardPetIntensifyItem:RewardPetIntensifyItem()
    self.language = Language.Reward.RewardPetIntensify
end
-- data ==== cfg:pet_strengthen 
function RewardPetIntensifyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Condition, self:GetConditionText())
    local reward_list_data = DataHelper.FormatItemList(data.item_list_1)
    self.RewardGrid:SetData(reward_list_data)

    local flag = RewardData.Instance:PetIntensifyRewardFlag(data)
    self.BtnGetInter.Interactable = flag == 1
    UH.SetText(self.BtnGetText, flag == -1 and self.language.text1 or self.language.text2)
    self.BtnGetEffect:SetActive(flag == 1)
end

function RewardPetIntensifyItem:GetConditionText()
    local data = self.data
    local need_level = data.pet_strengthen
    local text
    local pet_qua_list = Split(data.quality, "|")
    local pet_text = ""
    for i,v in pairs(pet_qua_list) do
        local qua = tonumber(v)
        pet_text = pet_text .. Language.Pet.ColorLevelT[qua]
        if i ~= #pet_qua_list then
            pet_text = pet_text .. self.language.text3
        end
    end
    if need_level == 1 then
        text = Language.Reward.RewardPetIntensify.condition1
        text = string.format(text, pet_text)
    else
        text = Language.Reward.RewardPetIntensify.condition2
        text = string.format(text, pet_text, need_level)
    end
    return text
end

function RewardPetIntensifyItem:OnGetClick()
    local flag = RewardData.Instance:PetIntensifyRewardFlag(self.data)
    if flag == -1 then
        PublicPopupCtrl.Instance:Error(self.language.text4)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(self.language.text5)
    elseif flag > 0 then
        RewardCtrl.Instance:SendPetIntensifyRewardReq(self.data.seq)
    end
end