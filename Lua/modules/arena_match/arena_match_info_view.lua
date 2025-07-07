
ArenaMatchInfoView = ArenaMatchInfoView or DeclareView("ArenaMatchInfoView", ArenaMatchConfig.ResPath .. "arena_match_info")
VIEW_DECLARE_LEVEL(ArenaMatchInfoView,ViewLevel.L1)

function ArenaMatchInfoView:ArenaMatchInfoView()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Info
end

function ArenaMatchInfoView:LoadCallback()
	local info = self.data.cur_sel_info
	local info_type = info:InfoType()

	self.Board:SetData(self:GetType(), self.language.title[info_type], Vector2.New(1361, 672))
end

function ArenaMatchInfoView:CloseCallback()
	local info = self.data.cur_sel_info
	if ArenaMatchConfig.InfoType.own == info:InfoType() then
		self.data.cur_sel_info:RestorePP()
	end
end

function ArenaMatchInfoView:OnAutoSetClick()
	local cur_sel_info = self.data.cur_sel_info
	local pet_list, partner_list, show_pet, show_partner = self.data:GetFightList()
	local pet_index = 1
	local partner_index = 1
	local is_five_arena = cur_sel_info:IsFiveArena()
	local max_count = is_five_arena and 10 or 6
	for i = 1, max_count do
		local correlatio_pos
		if i % 2 == 0 then
			correlatio_pos = i - 1
		else
			correlatio_pos = i + 1
		end
		
		if not cur_sel_info.partner_list[correlatio_pos] and not cur_sel_info.pet_list[i] and not cur_sel_info.partner_list[i] then
			local partner = partner_list[partner_index]
			if partner then
				cur_sel_info:PartnerAdd(partner, i)
				partner_index = partner_index + 1
			end
		end
		if not cur_sel_info.pet_list[correlatio_pos] and not cur_sel_info.partner_list[i] and not cur_sel_info.pet_list[i] then
			local pet = pet_list[pet_index]
			if pet then
				cur_sel_info:PetAdd(pet, i)
				pet_index = pet_index + 1
			end
		end
	end
end

function ArenaMatchInfoView:OnResetClick()
	self.data.cur_sel_info:ClearAll()
end

function ArenaMatchInfoView:OnClickPetSkill()
	if next(self.data:GetMatchInfoPetList()) then
		ViewMgr:OpenView(ArenaMatchPetSkillView)
	else
		PublicPopupCtrl.Instance:Center(Language.ArenaMatch.Info.noPetYet)
	end
end
------------------------------------
ArenaMatchInfoViewDetail = ArenaMatchInfoViewDetail or DeclareMono("ArenaMatchInfoViewDetail", UIWFlushPanel)

function ArenaMatchInfoViewDetail:ArenaMatchInfoViewDetail()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Info

	self.data_cares = {
		{data = self.data.cur_sel_info, func = self.FlushRight},
	}
end

function ArenaMatchInfoViewDetail:FlushRight()
	local info = self.data.cur_sel_info
	
	local show_info =  info:PerInfo() or info
	local info_type = info:InfoType()
	self.BtnFight.text = self.language.fight[info_type]
	self.BtnWinnerInfoObj:SetActive(ArenaMatchConfig.InfoType.ready == info_type)

	self.Name.text = show_info:Name()
	self.Rank.text = show_info:RankL()
	-- self.Level.text = show_info:Level()
	if show_info.role_info.top_level and show_info.role_info.top_level > 0 then
        UH.SetText(self.Level, show_info.role_info.top_level)
		UH.Color(self.Level, COLORS.Red8)
		if self.PeakLevel then
			self.PeakLevel:SetActive(true)
		end
	else
		UH.Color(self.Level, COLORS.White)
		UH.SetText(self.Level, show_info:Level())
		if self.PeakLevel then
			self.PeakLevel:SetActive(false)
		end
	end
	UH.SetText(self.TxtHeScore, show_info:PetsCap() + show_info:PartnersCap())
	UH.SetText(self.TxtUsScore, info:GetPetListCap() + info:GetPartnerListCap())

	UH.SetAvatar(self.IconSp, show_info:AvatarType(),show_info:AvatarId(),show_info:AvatarQuality())
	AvatarEffect.SetImageEnabled(self.PeakImg, show_info.role_info.avatar_quality)

	self:FlushFight()
end

function ArenaMatchInfoViewDetail:FlushFight()
	local info = self.data.cur_sel_info
	local pet_list = info:PetList()
	local partner_list = info:PartnerList()
	local is_five_arena = info:IsFiveArena()

	for i = 1, self.BigShows:Length() do
		self.BigShows[i]:SetActive(is_five_arena)
	end

	for i = 1, self.FightItems:Length() do
		local pos = is_five_arena and i or i - 2
		self.FightItems[i]:SetData(pet_list[pos] or partner_list[pos], pos)
	end
end

function ArenaMatchInfoViewDetail:OnClickFight()
	TeamData.Instance:CheckSingle(function ()
		local info = self.data.cur_sel_info
		if ArenaMatchConfig.InfoType.own == info:InfoType() then
			ArenaMatchCtrl.Instance:SendPlatformBattleAdjust()
		elseif ArenaMatchConfig.InfoType.ready == info:InfoType() then
			if 0 == info:PetNum() and 0 == info:PartnerNum() then
				PublicPopupCtrl.Instance:Center(self.language.empty_tips)
				return
			end
			ArenaMatchCtrl.Instance:SendPlatformBattleChallenge()
		end
		ViewMgr:CloseView(ArenaMatchInfoView)
		ViewMgr:CloseView(ArenaMatchRankView)
		ViewMgr:CloseView(ArenaMatchView)	
	end)
end

function ArenaMatchInfoViewDetail:OnClickWinnerInfo()
	ViewMgr:OpenView(ArenaMatchWinnerView)
end

ArenaMatchInfoViewFightItem = DeclareMono("ArenaMatchInfoViewFightItem", ArenaMatchInfoViewFightItem)

function ArenaMatchInfoViewFightItem:ArenaMatchInfoViewFightItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language =  Language.ArenaMatch.Info
end

function ArenaMatchInfoViewFightItem:SetData(data, pos)
	UIWidgetBaseItem.SetData(self, data)
	self.pos = pos
	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	self.EmptyObj:SetActive(is_empty)
	if is_empty then return end
	local is_pet = nil ~= data.pet
	local is_partner = nil ~= data.partner
	self.IconPet:SetObjActive(is_pet)
	self.IconPartner:SetObjActive(is_partner)
	if is_pet then self:FlushPet(data) end
	if is_partner then self:FlushPartner(data) end
end

function ArenaMatchInfoViewFightItem:FlushPet(data)
	local pet = data.pet
	local vo = data.vo
	self.IconPet:SetData(data)
	self.Name.text = vo.name or pet:Name()
	self.Level.text = "Lv." .. (vo.level or pet:Level())
end

function ArenaMatchInfoViewFightItem:FlushPartner(data)
	local partner = data.partner
	local vo = data.vo
	self.IconPartner:SetData(data)
	self.Name.text = partner.name
	local level = vo.level
	if nil == level then
		local partner = PartnerData.Instance:GetPartnerInfoById(partner.partner_id)
		level = partner.vo.level
	end
	self.Level.text = "Lv." .. (level or RoleData.Instance:GetRoleLevel())
end

function ArenaMatchInfoViewFightItem:OnClickItem()
	if nil == self.data then
		self.arena_match_data.cur_sel_info_to_pos = self.pos
		ViewMgr:OpenView(ArenaMatchAddView)
	else
		PublicPopupCtrl.Instance:DialogTips{
			content = string.format(self.language.remove_tips, RichTextHelper.ColorStr(self.Name.text, COLORSTR.Yellow6)),
			confirm = {
				func = function()
					self.arena_match_data.cur_sel_info:RemoveByPos(self.pos)
					ViewMgr:CloseView(DialogTipsView)
					self.arena_match_data.cur_sel_info_to_pos = 0
				end
			}
		}
	end
end
------------------------------------------------
ArenaMatchPetSkillView = ArenaMatchPetSkillView or DeclareView("ArenaMatchPetSkillView","arena_match/arena_match_info_pet_skill")
VIEW_DECLARE_MASK(ArenaMatchPetSkillView,ViewMask.BlockClose)

function ArenaMatchPetSkillView:ArenaMatchPetSkillView()
	self.data = ArenaMatchData.Instance
end
function ArenaMatchPetSkillView:LoadCallback()
	self.Board3:SetData(ArenaMatchPetSkillView, Language.ActFormation.CopetSkillSetView, Vector2.New(896,528))
	self.PetList:SetCompleteCallBack(function ()
		self.PetList:ClickItem(1)
	end)
	self.PetList:SetData(self.data:GetMatchInfoPetList())
end

function ArenaMatchPetSkillView:CloseCallback()
	self.data.match_info_cur_pet_data = nil
	self.data.match_info_smart.cur_pet_index = -1
end
-----------------------------------------------
ArenaMatchPetSkillPanel = ArenaMatchPetSkillPanel or DeclareMono("ArenaMatchPetSkillPanel", UIWFlushPanel)
function ArenaMatchPetSkillPanel:ArenaMatchPetSkillPanel()
	self.data = ArenaMatchData.Instance
	self.pet_data = PetData.Instance
	self.data_cares = {
		{data = self.data.match_info_smart, func = self.FlushPetInfo, keys = {"cur_pet_index"}}
	}
end

function ArenaMatchPetSkillPanel:FlushPetInfo()
	if self.data.match_info_cur_pet_data == nil or self.data.match_info_cur_pet_data.pet == nil then
		return
	end
	local pet = self.data.match_info_cur_pet_data.pet
	local total_attr = self.pet_data:RealAttr(pet.index)
    if total_attr == nil then
        total_attr = pet:TotalAttr()
    end
    for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(pet:HP() / v)
            self.ProgMaxHp:SetText(string.format(Language.ActFormation.ProgressTxtColorFormat, pet:HP(), v))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(pet:MP() / v)
            self.ProgMaxMp:SetText(string.format(Language.ActFormation.ProgressTxtColorFormat, pet:MP(), v))
        elseif self["Lbl"..k] ~= nil then
            UH.SetText(self["Lbl" .. k], v)
        end
    end
	self.PetMainSkillList:SetData(self.data:GetMatchInfoCurPetMainSkillList())
end

function ArenaMatchPetSkillPanel:OnClickPet(mono)
	self.data:SetCurPetInfo(mono:GetData())
end
-------------------------------------------
ArenaMatchPetSkillPetItem = ArenaMatchPetSkillPetItem or DeclareMono("ArenaMatchPetSkillPetItem", UIWidgetBaseItem)
function ArenaMatchPetSkillPetItem:ArenaMatchPetSkillPetItem()
	self.formation_data = ActFormationData.Instance
	self.pet_data = PetData.Instance
end

function ArenaMatchPetSkillPetItem:SetData(data)
	if self.FlyBgSub then
        self.FlyBgSub:SetData({})
    end
	local pet = data.pet
    -- local state = self.formation_data:PetFormationStatus(pet.index)
    -- UH.SpriteName(self.PetState, state)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.PetName, pet:Name())
    UH.Color(self.PetName, pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)

    local score_num, score_level = pet:GetScore()
    UH.SetText(self.PetScore, score_num)
    UH.SpriteName(self.PetScoreLevel, "scoreLevel"..tostring(score_level))

    self.HeadCell:SetDataByPet(pet)
    local fly_flag = pet:FeiShengTimes()
    if self.FlyBgSub then
        self.FlyBgSub:SetData({fly_flag = fly_flag})
    end
    local is_lianyao = pet:IsLianYao()
    if self.YaoObj then
        self.YaoObj:SetActive(is_lianyao)
    end
    self.PetName.alignment = is_lianyao and 3 or 4
end

function ArenaMatchPetSkillPetItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
------------------------------------------------------------------
ArenaMatchPetSkillItem = ArenaMatchPetSkillItem or DeclareMono("ArenaMatchPetSkillItem", UIWidgetBaseItem)
function ArenaMatchPetSkillItem:ArenaMatchPetSkillItem()
	self.arena_match_data = ArenaMatchData.Instance
end

function ArenaMatchPetSkillItem:OnDestroy()
	self.GiftItem:SetGray(false)
end

function ArenaMatchPetSkillItem:SetData(data)
	self.GiftItem:SetData(data.gift_info)
	--1为被禁用，显示启用按钮
	self.ForbidBtn:SetActive(data.is_forbid == 0)
	self.PermitBtn:SetActive(data.is_forbid == 1)
	self.data = data
	self.GiftItem:SetGray(data.is_forbid == 1)
end

function ArenaMatchPetSkillItem:OnClickPermitBtn()
	self.ForbidBtn:SetActive(true)
	self.PermitBtn:SetActive(false)
	self.data.is_forbid = 0
	self.arena_match_data:SetPetSkillPermission(self.data)
	self.GiftItem:SetGray(false)
end

function ArenaMatchPetSkillItem:OnClickForbidBtn()
	self.ForbidBtn:SetActive(false)
	self.PermitBtn:SetActive(true)
	self.data.is_forbid = 1
	self.GiftItem:SetGray(true)
	self.arena_match_data:SetPetSkillPermission(self.data)
end

function ArenaMatchPetSkillItem:OnClickGift()
end