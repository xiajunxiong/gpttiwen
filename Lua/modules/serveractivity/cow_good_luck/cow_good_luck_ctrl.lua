VIEW_REQ("modules/serveractivity/cow_good_luck/cow_good_luck_view")

CowGoodLuckCtrl = CowGoodLuckCtrl or BaseClass(BaseCtrl)
 
function CowGoodLuckCtrl:__init()
    self.data = CowGoodLuckData.New()
    self:RegisterProtocol(SCRACowLuckyExchangeInfo,"OnCowLuckyExchangeInfo")
	self:RegisterProtocol(SCRACowStoreInfo,"OnCowStoreInfo")
end

function CowGoodLuckCtrl:__delete()
    self:UnRegisterProtocol(SCRACowLuckyExchangeInfo)
end

function CowGoodLuckCtrl:OnCowLuckyExchangeInfo(protocol)
	self.data:OnProtocolData(protocol)
end

function CowGoodLuckCtrl:OnCowStoreInfo(protocol)
	self.data:OnProtocolShopData(protocol)
end

CowGoodLuckData = CowGoodLuckData or BaseClass()

function CowGoodLuckData:__init()
	if CowGoodLuckData.Instance ~= nil then
		Debuger.LogError("[CowGoodLuckData] attempt to create singleton twice!")
		return
	end	
    CowGoodLuckData.Instance = self
    self.intimacy = SmartData.New({num = 0})
	self.show_all = SmartData.New({flag = false})
	self.buy_count_change = SmartData.New({flag = false})
	self.history_chongzhi_num = 0
end

function CowGoodLuckData:__delete()

end

function CowGoodLuckData:OnProtocolData(protocol)
	self.intimacy.num = protocol.initmacy
	self.history_chongzhi_num = protocol.history_chongzhi_num
end

function CowGoodLuckData:GetIntimacyRemind()
	if self.intimacy.num >= self:GetIntimacyCanGet() then
		return 1
	end
	return 0
end

function CowGoodLuckData:OnProtocolShopData(protocol)
	self.open_day = protocol.open_day
	self.buy_day_times = protocol.buy_day_times
	self.buy_sum_times = protocol.buy_sum_times
	self.buy_count_change.flag = not self.buy_count_change.flag
end

function CowGoodLuckData:GetIntimacyCanGet()
	local need_intimacy = 0
	local recharge_num = self.history_chongzhi_num
	local gift_configure = Config.cow_lucky_exchange_auto.gift_configure
	for k,v in pairs(gift_configure) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			need_intimacy = v.price
			break
		end
	end
	return need_intimacy
end

function CowGoodLuckData:GetShowItemList()
	local item_list = {}
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			item_list = v.reward_item
			break
		end
	end
	return item_list
end

function CowGoodLuckData:GetShowPetResId()
	local pet_id = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			pet_id = v.pet_show
			break
		end
	end
	local res_id = 0
	local pet_cfg = Config.pet_cfg_auto.pet_info_2
	for k1, v1 in pairs(pet_cfg) do
		if v1.id == pet_id then
			res_id = v1.res_id
			break
		end
	end
	return res_id
end

function CowGoodLuckData:GetShowPetName()
	local pet_id = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			pet_id = v.pet_show
			break
		end
	end
	local pet_name = ""
	local pet_cfg = Config.pet_cfg_auto.pet_info_2
	for k1, v1 in pairs(pet_cfg) do
		if v1.id == pet_id then
			pet_name = v1.name
			break
		end
	end
	return pet_name
end

function CowGoodLuckData:GetShowPetInfo()
	local petid = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			petid = v.pet_show
			break
		end
	end
	local pet = Pet.New({pet_id = petid})	
	local total_attr = pet:TotalAttr()
	local res = {}
    res.maxhp = total_attr[BattleAttriType.MaxHp]
    res.maxmp = total_attr[BattleAttriType.MaxMp]
    res.gongji = total_attr[BattleAttriType.Attack]
    res.fangyu = total_attr[BattleAttriType.Defense]
    res.minjie = total_attr[BattleAttriType.Agility]
    res.jingshen = total_attr[BattleAttriType.Mental]
    res.huifu = total_attr[BattleAttriType.Recovery]
	return res
end

function CowGoodLuckData:GetShowPetData()
	local petid = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			petid = v.pet_show
			break
		end
	end
	local pet_data = PetData.Instance:GetPetCfgByNormalId(petid)
	return pet_data
end

function CowGoodLuckData:GetSkillAndGiftGroupList()
	local pet_data = self:GetShowPetData()
	local gift_list = pet_data:GetSkillAndGiftGroupList()
	return gift_list
end

function CowGoodLuckData:GetShowPetSkillInfo()
	local petid = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			petid = v.pet_show
			break
		end
	end
	local pet = Pet.New({pet_id = petid})	
	local skill_list = pet:SkillList()
	return skill_list
end

function CowGoodLuckData:GetShowListInfo()
	local petid = 0
	local recharge_num = self.history_chongzhi_num
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and  recharge_num <= v.accumulated_up then
			petid = v.pet_show
			break
		end
	end
	local pet = Pet.New({pet_id = petid})	
	local skill_list = pet:SkillList()
	return skill_list
end

function CowGoodLuckData:GetGiftConfigByOpenDay()
	local gift_configure = Config.cow_store_auto.gift_configure
	local server_start = TimeCtrl.Instance:GetCurOpenServerDay()
	local info_list = {}
	for k,v in pairs(gift_configure) do
		if v.start_time <= server_start and v.end_time >= server_start then
			table.insert(info_list, v)
		elseif v.start_time <= server_start and v.end_time == 0 then
			table.insert(info_list, v)
		end
	end
	return info_list
end

function CowGoodLuckData:GetGiftList()
	local gift_list = {}
	local gift_configure = self:GetGiftConfigByOpenDay()
	for k,v in pairs(gift_configure) do 
		if v.section_start <= self.open_day and v.section_end >= self.open_day then
			table.insert(gift_list, v)
		end
	end
    return gift_list
end

function CowGoodLuckData:SortGiftList()
	local gift_configure = self:GetGiftList()
	for k,v in pairs(gift_configure) do 
		local have_count = self:GetGiftHaveCount(v.seq)
		if have_count > 0 then
			v.sort = 1
		else
			v.sort = 0
		end
	end

	table.sort(gift_configure,function(a,b)
		if a.sort ~= b.sort then
			return a.sort > b.sort 
		else
			return a.seq < b.seq 
		end
	end)	
	
    return gift_configure
end

function CowGoodLuckData:GetGiftCanBuyCount(seq)
	local gift_configure = self:GetGiftList()
	local all_count = 0
	for k,v in pairs(gift_configure) do 
		if v.seq == seq then
			if v.limit_type == 1 then
				all_count = v.buy_times
			else
				all_count = v.today_times
			end
			break
		end
	end
	return all_count
end

function CowGoodLuckData:GetGiftBuyCount(seq)
	local gift_configure = self:GetGiftList()
	local buy_count = 0
	for k,v in pairs(gift_configure) do 
		if v.seq == seq then
			if v.limit_type == 1 then
				buy_count = self.buy_sum_times[seq]
			else
				buy_count = self.buy_day_times[seq]
			end
			break
		end
	end
	return buy_count
end

function CowGoodLuckData:GetGiftHaveCount(seq)
	local have_count = 0
	local buy_count = self:GetGiftBuyCount(seq)
	local all_count = self:GetGiftCanBuyCount(seq)
	have_count  = all_count - buy_count
	return have_count
end

function CowGoodLuckData:GetShowRoleList()
	local gift_show = Config.cow_lucky_exchange_auto.gift_show
	local role_show = gift_show[1].role_show
	local recharge_num = self.history_chongzhi_num
	for k,v in pairs(gift_show) do 
		if recharge_num >= v.accumulated_down and recharge_num <= v.accumulated_up then
			role_show = v.role_show
			break
		end
	end
	local show_list = DataHelper.SplitList(role_show, "|")
	return show_list
end

