CellFlushs = CellFlushs or {}

--物品品质框颜色全局
COLOR_FRAME_SPRITE = {
	[ItemColor.None] = "WuPinKuangDi",
	[ItemColor.White] = "WuPinKuangPinZhi_PuTong",
	[ItemColor.Green] = "WuPinKuangPinZhiLv",
	[ItemColor.Blue] = "WuPinKuangPinZhiLan",
	[ItemColor.Purple] = "WuPinKuangPinZhiZi",
	[ItemColor.Orange] = "WuPinKuangPinZhiCheng",
	[ItemColor.Red] = "WuPinKuangPinZhiHong",
	[ItemColor.Star] = "WuPinKuangPinZhiCheng",
    [ItemColor.God] = "WuPinKuangPinZhiHong",
	[ItemColor.Gold] = "WuPinKuangPinZhiJin",
	[ItemColor.Black] = "WuPinKuangPinZhiHei",
}

local rank_qua_sprite = {
	Rank3 = "cellRankS",
	Rank2 = "cellRankA",
	Rank1 = "cellRankB",
	Rank0 = "cellRankC",
}
local zhezhao_sprite = {
	petTong = "ChongWuKaTong",
	petYin = "ChongWuKaYin",
	petJin = "ChongWuKaJin",
	--petXiyou = "ChongWuKaXiyou",
	--petShenshou = "ChongWuKaShenshou",
	Partner = "HuoBan_ZheZhao",
	PetSoul_1 = "ChongWuHunPo_1",
	PetSoul_2 = "ChongWuHunPo_2",
	PetSoul_3 = "ChongWuHunPo_3",
	PetSoul_4 = "ChongWuHunPo_4",
	PetSoul_5 = "ChongWuHunPo_5",
	PetPiece_1 = "ChongWuSuiPian_1",
	PetPiece_2 = "ChongWuSuiPian_2",
	PetPiece_3 = "ChongWuSuiPian_3",
	PetPiece_4 = "ChongWuSuiPian_4",
	PetPiece_5 = "ChongWuSuiPian_5",
	pws = "PartnerWakeStone_",
	fls = "FuLingShi_ZheZhao",
	pb = "JiNengShu_ZheZhao",
	youming5 = "YouMingJiNengZheZhao_Cheng",
	youming6 = "YouMingJiNengZheZhao_Hong",
	youming9 = "YouMingJiNengZheZhao_Cai",
}
local medal_sp_bg_sprite = {
	[ItemColor.Blue] = "medal_sp_lan",
	[ItemColor.Purple] = "medal_sp_zi",
	[ItemColor.Orange] = "medal_sp_cheng",
	[ItemColor.Red] = "medal_sp_hong"
}

--用于CheckSomething中针对不同ShowType物品的处理接口
CellShowTypeFunc = {
	--伙伴碎片
	[ShowType.PartnerPieces] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite.Partner)
	end,
	--宠物技能书
	[ShowType.PetSkillBook] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite.pb)
	end,
	--宠物魂魄 兑换宠物 1:1
	[ShowType.PetSoul] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local co = Item.GetConfig(data.item_id)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite["PetSoul_"..co.pet_quality])		
	end,
	--宠物蛋
	[ShowType.PetEgg] = function (cell, data)
		if data.Config then
			local config = data:Config()
			if config.param1 and config.param1 >= 2 then
				if cell.EggJieImage then
					cell.EggJieImage:SetObjActive(true)
					UH.SpriteName(cell.EggJieImage, "PetNum"..tostring(config.param1))
				end
			end
		end
	end,
	--图鉴卡
	[ShowType.IllustrateCard] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local icon_id = data.IconId and data:IconId() or 0
		local pet_cfg = PetData.Instance:GetPetCfg(icon_id) or {} -- 策划保证图鉴卡icon_id==宠物id
		UH.SpriteName(cell.ZheZhaoTop, zhezhao_sprite.petYin)
	end,
	--宠物碎片 兑换宠物 多:1
	[ShowType.PetPieces] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local co = Item.GetConfig(data.item_id)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite["PetPiece_"..co.pet_quality])
	end,
	[ShowType.YouMingClip] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local co = Item.GetConfig(data.item_id)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite["PetPiece_"..co.color])
	end,
	--伙伴觉醒石
	[ShowType.PartnerWakeStone] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local co = Item.GetConfig(data.item_id)
		UH.SpriteName(cell.ZheZhaoTop, zhezhao_sprite.pws .. (co.color == 9 and 7 or co.color))
	end,
	--活动物品
	[ShowType.ActItem] = function (cell, data)
		cell.JiaoBiao:SetActive(true)
	end,
	--附灵石
	[ShowType.FuLingShi] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite.fls)		
		local cfg = MedalData.Instance:GetMedalSpCfg(data.item_id)
		if cell.FulingProf then 
		    cell.FulingProf:SetObjActive(cfg.role_id > 0)
		    if cfg.role_id > 0 then 
		        UH.SpriteName(cell.FulingProf,MedalConfig.MedalProfIcon[cfg.role_id])	
		    end 

			local item_cfg = MedalData.Instance:GetMedalSpItemCfg(data.item_id)
			local sp_cfg =  BagData.Instance:GetEquipEffectCfg(item_cfg.sp_id)
			cell.RankPic:SetObjActive(item_cfg.rank ~= nil)
			local level_ = sp_cfg.is_special == 1 and "n" or item_cfg.rank
			UH.SpriteName(cell.RankPic,MedalConfig.MedalRankIcon..level_)	
		end 
	end,
	--宠物魂器
	[ShowType.PetWeapon] = function (cell, data)
		--if cell.IsShowSpec then
			local point_text = data.QuaShowPoint and data:QuaShowPoint() or ""
			if cell.QuaPoints then
				cell.QuaPoints.text = point_text
			end
			-- if cell.WeaponLock then
			-- 	local is_lock = data.IsLock and data:IsLock() or false
			-- 	cell.WeaponLock:SetActive(is_lock)
			-- end

			if cell.EquipLevelObj then
				local level = data.Level and data:Level() or 1
				cell.EquipLevelObj:SetActive(true)
				if cell.EquipLevelText then
					UH.SetText(cell.EquipLevelText, string.format(Language.PetStamp.Level,level))
				end
			end

			if cell.LevelJieDi then
				cell.LevelJieDi:SetActive(true)
				local refine_level = data.RefineLevel and data:RefineLevel() or 0
				UH.SetText(cell.LevelJie, string.format(Language.PetStamp.Step,DataHelper.GetDaXie(refine_level)))
			end
		--end
	end,
	--符石
	[ShowType.FuShi] = function (cell, data)
		if cell.FuShiDi then
			cell.FuShiDi:SetActive(true)
			cell.FuShiDi.transform:SetParent(cell.QuaIcon.gameObject.transform)
		end

		if cell.EquipLevelObj then
			local level = data.Level and data:Level() or 1
			cell.EquipLevelObj:SetActive(true)
			if cell.EquipLevelText then
				UH.SetText(cell.EquipLevelText, string.format(Language.PetStamp.Level,level))
			end
		end
	end,
	--圣器
	[ShowType.ShengQi] = function (cell, data)
		if cell.EquipLevelObj then
			local level = data.Level and data:Level() or 1
			cell.EquipLevelObj:SetActive(true)
			if cell.EquipLevelText then
				UH.SetText(cell.EquipLevelText, string.format(Language.PetStamp.Level,level))
			end
		end
		if cell.FuShiFlag then
			local str = data:QuaShowPoint()
			cell.FuShiFlag.text = str
		end
	end,

	--灵骑技能卷轴
	[ShowType.BicycleSkillBook] = function (cell, data)
		if data.Config then
			local config = data:Config()
			local level = config.param3
			if cell.EquipLevelObj then
				cell.EquipLevelObj:SetActive(true)
			end
			if cell.BicycleSkillFrame then
				local color = data:Color()
				UH.SpriteName(cell.BicycleSkillFrame, "BicycleSkill" .. color)
			end
			if cell.EquipLevelText then
				UH.SetText(cell.EquipLevelText, string.format("Lv.%s",level))
			end
		end
	end,

	--幻兽技能书
	[ShowType.HuanShouSkillBook] = function (cell, data)
		if cell.BicycleSkillFrame then
			local color = data:Color()
			if color >= 4 and color <= 6 then
				UH.SpriteName(cell.BicycleSkillFrame, "HuanShouBook" .. color)
			end
		end
	end,

	--幻兽特殊蛋
	[ShowType.HuanShouSPEgg] = function (cell, data)
		if cell.BicycleSkillFrame then
			local color = data:Color()
			if color >= 3 and color <= 9 then
				UH.SpriteName(cell.BicycleSkillFrame, "HuanShouEgg" .. color)
			end
		end
	end,

	--幻兽灵魄
	[ShowType.HuanShouSoul] = function (cell, data)
		if cell.BicycleSkillFrame then
			local color = data:Color()
			if color >= 3 and color <= 9 then
				UH.SpriteName(cell.BicycleSkillFrame, "HuanShouSoul" .. color)
			end
		end

		local cfg = Cfg.HuanShouBaseCfgByItemId(data.item_id)
		if cfg and cfg.is_limit == 1 and cell.LimitFlag then
			cell.LimitFlag:SetActive(true)
		end
	end,

	[ShowType.PetItemShow] = function (cell, data)
		if data.Config then
			local config = data:Config()
			if config.param1 and config.param1 >= 2 then
				if cell.EggJieImage then
					cell.EggJieImage:SetObjActive(true)
					UH.SpriteName(cell.EggJieImage, "PetNum"..tostring(config.param1))
				end
			end
		end
	end,
	--龙魂
	[ShowType.FairyDragonSoul] = function (cell, data)
		if cell.DragonSoulType then
			if cell.QuaIcon then
				local qua = data:Color()
				local img = "DragonSoul" .. qua
				UH.SpriteName(cell.QuaIcon, img)
			end

			if data.SoulType and cell.DragonSoulType then --and not cell.NotFlag then
				local type = data:SoulType() or FairyDragonCfg.SOUL_TYPE.Gong
				local img = "_LocLongHunLeiXing" .. type
				UH.SpriteName(cell.DragonSoulType, img)
			end
		end
	end,
	-- 配饰
	[ShowType.Accrssory] = function(cell, data)
		-- 配饰套装类型角标
		if cell.AccrssoryTitleBg then
			cell.AccrssoryTitleBg:SetActive(true)
		end
		if cell.TxtAccrssoryTitle then
			UH.SetText(cell.TxtAccrssoryTitle, data:SuitName())
		end
		-- 配饰强化角标
		if data.StrengthLevel and data:StrengthLevel() > 0 then
			if cell.AccrssoryStrengthBg then
				cell.AccrssoryStrengthBg:SetActive(true)
			end
			if cell.TxtAccrssoryStrength then
				UH.SetText(cell.TxtAccrssoryStrength, Format(Language.Accrssory.Add, data:StrengthLevel()))
			end
		end
	end,
	-- 灵宝
	[ShowType.LingBao] = function(cell, data)
		-- 灵宝阶数
		if cell.ObjLingBaoState then
			cell.ObjLingBaoState:SetActive(true)
		end
		if cell.TxtLingBaoState then
			UH.SetText(cell.TxtLingBaoState, Format(Language.LingBao.ItemState, DataHelper.GetDaXie(data:StateLevel())))
		end
		-- 灵宝星星
		if cell.ObjLingBaoStar then
			cell.ObjLingBaoStar:SetActive(true)
			local mono = cell.ObjLingBaoStar:GetLuaComponent("UIWidgetList")
			mono:SetData(data:StarData())
		end
	end,
	-- 变身框
	[ShowType.LiveCard] = function(cell, data)
		if cell.LiveCard then
			cell.LiveCard:SetActive(true)
		end
	end,
	[ShowType.Halidom] = function (cell, data)
		if cell.HalidomFlag then
			cell.HalidomFlag:SetActive(true)
		end
	end,
	-- 宠物徽章
	[ShowType.PetBagde] = function (cell, data)
		if data.SkillId then
			local skill_id = data:SkillId() or 0
			if skill_id > 0 and cell.EffectFlag then
				cell.EffectFlag:SetActive(true)
			end
		end
	end,
	-- 宠物神印
	[ShowType.PetStamp] = function (cell, data)
		if data.item_id > 0 then
			-- 等级
			local level = data.Level and data:Level() or 1
			if cell.PetStampLevel then
				cell.PetStampLevel:SetActive(true)
			end
			if cell.PetStampLevelText then
				UH.SetText(cell.PetStampLevelText, string.format(Language.PetStamp.Level,level))
			end

			-- 神印专属品质框
			if cell.PetStampQua then
				cell.PetStampQua:SetObjActive(true)
				local qua_frame_name = ""
				if data.QuaFrameName then
					qua_frame_name = data:QuaFrameName()
				else
					local cfg = Item.GetConfig(data.item_id)
					local pos_index = cfg.param2
					local qua = cfg.color
					qua_frame_name = string.format("pet_stamp_%s",qua)
				end
				UH.SpriteName(cell.PetStampQua, qua_frame_name)
			end

			-- 神印位置角标
			local pos_index = data.PosIndex and data:PosIndex() or 0
			if pos_index > 0 and cell.PetStampPos then
				cell.PetStampPos:SetActive(true)
				UH.SetText(cell.PetStampPosText, DataHelper.GetDaXie(pos_index, true))
			end

			-- 品质点点点
			-- if cell.PetStampPoints then
			-- 	local qua_desc = ""
			-- 	if data.QuaShowPoint then
			-- 		qua_desc = data:QuaShowPoint()
			-- 	elseif cfg ~= nil then
			-- 		for i = 1,cfg.param3 do
			-- 			qua_desc = qua_desc .. "1"
			-- 		end
			-- 	end
			-- 	cell.PetStampPoints.text = qua_desc
			-- end

			if cell.PetStampIcon and data.Icon then
				UH.SetIcon(cell.PetStampIcon, data:Icon())
			end
			if cell.SwallowStar then
				if data.info and data.info.swallow_num and data.info.swallow_num > 0 then
					cell.SwallowStar:SetActive(true)
				else
					cell.SwallowStar:SetActive(false)
				end
			end
		end
		
		if data.item_id == 0 then
			cell.data = nil
			UH.SetIcon(cell.PetStampIcon, nil)
		end
	end,

	[ShowType.AttributeDan] = function (cell, data)
		local atributedan = AttributeDanData.Instance:GetAttibuteDanByItemId(data.item_id)
		if atributedan then
			if atributedan.attribute_pellet_type == 0 then
				if cell.AttributedanRole then
					cell.AttributedanRole:SetActive(true)
				end
			end
			if atributedan.attribute_pellet_type == 1 then
				if cell.AttributedanPet then
					cell.AttributedanPet:SetActive(true)
				end
			end
			if atributedan.attribute_pellet_type == 2 then
				if cell.AttributedanPartner then
					cell.AttributedanPartner:SetActive(true)
				end
			end
		end
	end,

	[ShowType.PartnerPact] = function (cell, data)
		if cell.PartnerPact then
			cell.PartnerPact:SetActive(data.show_stars)
			local mono = cell.PartnerPact:GetLuaComponent("PartnerPactCell")
			mono:SetData(data)
		end
	end,

	[ShowType.Neveal] = function (cell, data)
		if cell.Neveal then
			cell.Neveal:SetActive(data:IsNeveal())
			if data:IsNeveal() then 
			    local mono = cell.Neveal:GetLuaComponent("NevealCellShow")
			    mono:SetData(data) 
			end 
		end
	end,
	[ShowType.YouMingSkillBook] = function (cell, data)
		cell.ZheZhaoTop:SetObjActive(true)
		local qua = data:Color()
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite["youming"..qua])
	end,
}
function CellFlushs.SetIconQuality(cell, item_id)
	if item_id == 0 then
		UH.SpriteName(cell.QuaIcon, nil)
	else
		local color = Item.GetColor(item_id)
		local data = cell:GetData()
		if data.UsedTime then
			if data:UsedTime() == 0 then
				color = tonumber(1)
			end
		end

		if data.IsMedal and data:IsMedal() and data.param and data.param.sp_id then
			UH.SpriteName(cell.QuaIcon, COLOR_FRAME_SPRITE[data:GetMedalQua()])
		else 
			UH.SpriteName(cell.QuaIcon, COLOR_FRAME_SPRITE[color])
		end 

		--变异框层级要比品质框低，所以要提前设置（创建）
		if data.IsMutant and data:IsMutant() then
			cell.Mutant:SetActive(true)
		end
		if data.IsLianYao and data:IsLianYao() then
			cell.LianYao:SetActive(true)
			cell.LianYao.transform:SetParent(cell.Icon.gameObject.transform)
		end
		if data.QuaIcon then
			UH.SpriteName(cell.QuaIcon, nil)
			UH.SpriteName(cell.CatchIcon, data:QuaIcon())
		end
		--宠物强化信息
		if data.StrLevel and data:StrLevel() and not cell.BlockStrLevel then
			if data:StrLevel() ~= 0 then
				UH.SpriteName(cell.JieImage, PetData.PetStrLevel[data:StrLevel()])
			end
		end
	end
	UH.SpriteName(cell.Frame, cell.Dark and UIWItemCell.FrameName[3] or UIWItemCell.FrameName[1])
end

function CellFlushs.SetNum(cell, num, show_one)
	if not StringIsEmpty(cell.CellSize) then
		UH.LocalScale(cell.RbTxt.transform,Vector3.New(1/tonumber(cell.CellSize),1/tonumber(cell.CellSize), 1))
	else
		UH.LocalScale(cell.RbTxt.transform,Vector3.one)
	end
	if num and type(num) == "number" and (num > 1 or show_one) then
		cell.RbTxt.text = num >= 10000 and DataHelper.ConverMoney(num) or num
	elseif type(num) == "string" then
		cell.RbTxt.text = num
	end
	cell.Bind.transform:SetAsLastSibling()
	cell.RbTxt.transform:SetAsLastSibling()
end

function CellFlushs.SetIcon(cell, icon_id)
	-- 获取图片赋值
	local data = cell:GetData()
	if data.UsedTime then
		if data:UsedTime() == 0 then
			icon_id = tonumber(40040)
		end
	end
	-- if icon_id and icon_id ~= 0 then
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		local audit_seq = AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
		if Config.agent_adapt_auto.icon_replace[audit_seq][icon_id] == 1 then
			icon_id = icon_id.."audit"..audit_seq
		end		
	end
	UH.SetIcon(cell.Icon, icon_id)
	-- else
	-- 	UH.SpriteName(cell.Icon, nil)
	-- end
	-- 不管了先在这里加
end

--设置物品名称
--quaColor 是否适用品质色
--DarkBg 深色底板
function CellFlushs.SetName(cell, data, quaColor, DarkBg)
	local config = Item.GetConfig(data.item_id)
	if config == nil then
		Debuger.LogError("!!!!!!!!!!无配置id" .. tostring(data.item_id))
		return
	end
	local colorStr = DarkBg and QualityColorStr[data:Color()] or QualityColorStrShallow[data:Color()]
	local colorStr = quaColor and colorStr or COLORSTR.White
	UH.SetText(cell.ItemName, string.format(Language.Common.Xstr, colorStr, config.name))
end

function CellFlushs.SetNewTips(cell,is_new)
	local flag = is_new == nil and false or is_new
	if flag and cell.NewTips then
		cell.NewTips:SetActive(flag)
	end 
end

function CellFlushs.SetFormula(cell, data)
	if data.is_unique_formula then
		cell.FormulaUnique:SetActive(true)
		cell.IsGetObj:SetActive(WorkshopData.Instance:GetWorkShopActivation(data))
	end
end

function CellFlushs.SetQuaSpAnim(cell)
	local data = cell:GetData()

	if data.item_id ~= 0 and not data.is_grey then
		local color = Item.GetColor(data.item_id)
		if color >= ItemColor.Star then 
			cell.QuaAnimTool:SetObjActive(false)	
			if color == 7 then
				if cell.qua_eff == nil or cell.qua_eff ~= "GodEffectOrange" then
					cell.GodEffectOrange:SetActive(true)
					cell.qua_eff = "GodEffectOrange"
				elseif cell.qua_eff == "GodEffectOrange" then 
					cell.GodEffectOrange:SetActive(true)
			    end 
			elseif color == 8 then
				if cell.qua_eff == nil or cell.qua_eff ~= "GodEffectRed" then 
					cell.GodEffectRed:SetActive(true)
					cell.qua_eff = "GodEffectRed"
				elseif cell.qua_eff == "GodEffectRed" then 
					cell.GodEffectRed:SetActive(true)
			    end 
			elseif color == 9 then 
				if cell.qua_eff == nil or cell.qua_eff ~= "GoldEffect" then 
					cell.GoldEffect:SetActive(true)
					cell.qua_eff = "GoldEffect"
				elseif cell.qua_eff == "GoldEffect" then 
					cell.GoldEffect:SetActive(true)
			    end 

				UISpriteAnimTool.AnimName(cell.QuaAnimTool, string.format(UISpriteAnimToolConfig.ItemCell.AnimName, color))
				UH.SizeDelta(cell.QuaAnimTool, Vector2.New(105,105))
				cell.QuaAnimTool.PicNum = 8
				cell.QuaAnimTool:SetObjActive(true)
			elseif color == 10 then 
				-- if cell.qua_eff == nil or cell.qua_eff ~= "BlackEffect" then 
				-- 	cell.BlackEffect:SetActive(true)
				-- 	cell.qua_eff = "BlackEffect"
				-- elseif cell.qua_eff == "BlackEffect" then 
				-- 	cell.BlackEffect:SetActive(true)
			    -- end 

				UISpriteAnimTool.AnimName(cell.QuaAnimTool, string.format(UISpriteAnimToolConfig.ItemCell.AnimName, color))
				UH.SizeDelta(cell.QuaAnimTool, Vector2.New(150,150))
				cell.QuaAnimTool.PicNum = 24
				cell.QuaAnimTool:SetObjActive(true)
			end
		elseif data.FeiShengTimes ~= nil then	--宠物飞升
			local fs_lv = data:FeiShengTimes()
			UIWPetHeadCell.EffAnimName(fs_lv,cell.FSQuaAnim)
		else
			if color and tonumber(color) and color >= ItemColor.Purple then
				UISpriteAnimTool.AnimName(cell.QuaAnimTool, string.format(UISpriteAnimToolConfig.ItemCell.AnimName, color))
				UH.SizeDelta(cell.QuaAnimTool, Vector2.New(105,105))
				cell.QuaAnimTool.PicNum = 8
				cell.QuaAnimTool:SetObjActive(true)
			end
		end
	end
end

function CellFlushs.SetBind(cell, is_bind)
	if type(is_bind) == "number" then
		cell.Bind:SetActive(is_bind == 1)
	else
		cell.Bind:SetActive(is_bind)
	end
end

function CellFlushs.MakeGrey(cell, is_grey)
	cell:MakeGrey(is_grey)
end

function CellFlushs.SubName(cell)
	local data = cell:GetData()
	cell.SubTxt.text = data:QuaName()
end

function CellFlushs.SetEquipBetter(cell,is_better)
	cell.EquipBetter:SetActive(is_better)
end

function CellFlushs.SetEquipInvalid(cell,is_invalid)
	cell.EquipInvalid:SetActive(is_invalid)
end

function CellFlushs.SetNotTreasure(cell)
	cell.SkillQuaIcon:SetActive(false)
end

function CellFlushs.ChatQuickShow(cell, data)
	if data.chat_quick_show then
		cell.ChatQuickObj:SetActive(true)
		local ChatQuickText = cell.ChatQuickObj:GetComponentInChildren(typeof(UnityUI.Text))
		ChatQuickText.text = data.chat_quick_show_medal and Language.Chat.ChatQuick.Dressed or Language.Chat.ChatQuick.Equiped
	end
end

-----common functions end-------

-----Internal refresh--------
function CellFlushs.ReadyItem(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	CellFlushs.SetIconQuality(cell, data.item_id)
	local icon_id = data.IconId and data:IconId() or 0
	CellFlushs.SetIcon(cell, icon_id)
	CellFlushs.SetQuaSpAnim(cell)
	CellFlushs.CheckSomething(cell, data)
	CellFlushs.SetNewTips(cell, data.is_new)
	CellFlushs.SetFormula(cell, data)
end
function CellFlushs.FlushFuYu(cell, data)
	if cell.FuYuLevel ~= nil then
		cell.FuYuLevel:SetActive(true)
		local mono = cell.FuYuLevel:GetLuaComponent("FuYuCellPlus")
		mono:SetData(data)
	end
	if data.param and data.param.sp_id and data.param.sp_id > 0 then 
		local cfg = Cfg.MedalEffectById(data.param.sp_id)
		if cfg then 
			cell.MedalSpBg:SetActive(true)
			cell.MedalSpEff:SetActive(true)
			cell.MedalSpEff:GetComponent(typeof(UnityUI.Text)).text = cfg.name
			UH.SpriteName(cell.MedalSpBg:GetComponent(typeof(Game.UIImageSpriteSet)),medal_sp_bg_sprite[data:Color()])
		else
			cell.MedalSpBg:SetActive(true)
			cell.MedalSpEff:SetActive(true)
			cell.MedalSpEff:GetComponent(typeof(UnityUI.Text)).text = data.param.sp_id
		end
	end 
end
function CellFlushs.CheckSomething(cell, data)
	local icon_id = data.IconId and data:IconId() or 0
	if type(icon_id) == "string" and string.sub(icon_id, 1, 1) == "s" then
		cell.ZheZhaoTop:SetObjActive(true)
		UH.SpriteName(cell.ZheZhaoTop,zhezhao_sprite.Partner)
		cell.IconRect.sizeDelta = Vector2.New(80, 80)
		--[[ cell.SkillQuaIcon:SetActive(true)
		local skill_quality = data:Config().skill_quality
		UH.SpriteName(cell.SkillQuaIcon:GetComponent(typeof(Game.UIImageSpriteSet)),
		skill_quality == 1 and "" or skill_qua_sprite[skill_quality]) ]]
	end
	if data.IsEquip and data:IsEquip() and data.param and data.param.is_treasure then 
		local flag = BagData.Instance:CheckWithScore(data.item_id)
 		cell.CellRank:SetActive(flag)
		if flag then 
		    UH.SpriteName(cell.CellRank:GetComponent(typeof(Game.UIImageSpriteSet)), rank_qua_sprite["Rank"..data.param.is_treasure])
		end 
		local upgarde_flag = BagData.Instance:GetEquipSortUpgrade(data:EquipType()) > 0 and not data.in_bag 
		    and ((cell.WithUpgrade ~= nil and cell.WithUpgrade == true) or cell.WithUpgrade == nil) and not ViewMgr:IsOpen(EquipView)
		cell.UpgradeObj:SetActive(upgarde_flag)
		if upgarde_flag then 
			UH.SetText(cell.UpgradeText, "+"..BagData.Instance:GetEquipSortUpgrade(data:EquipType()))
		end 
	end
	if data.IsJewelry and data:IsJewelry() and data.param and data.param.is_treasure then
		local flag = BagData.Instance:CheckWithScore(data.item_id)
		cell.CellRank:SetActive(flag)
		if flag then 
		    UH.SpriteName(cell.CellRank:GetComponent(typeof(Game.UIImageSpriteSet)), rank_qua_sprite["Rank"..data.param.is_treasure])
		end 

		local upgarde_flag = BagData.Instance:GetEquipSortUpgrade(data:EquipType()) > 0 and not data.in_bag 
		    and ((cell.WithUpgrade ~= nil and cell.WithUpgrade == true)or cell.WithUpgrade == nil) and not ViewMgr:IsOpen(EquipView)
		cell.UpgradeObj:SetActive(upgarde_flag)
		if upgarde_flag then 
			UH.SetText(cell.UpgradeText, "+"..BagData.Instance:GetEquipSortUpgrade(data:EquipType()))
		end 
	end

	if data.IsBlazeStar and data:IsBlazeStar() then 
		cell.BlazeStar:SetActive(data:IsBlazeStar())
		local mono = cell.BlazeStar:GetLuaComponent("BlazeStarCellShow")
		mono:SetData(data) 
	end
	if data.IsFuYu and data:IsFuYu() then
		CellFlushs.FlushFuYu(cell, data)
	end

	--物品格子针对ShowType的处理处理接口集中写在CellShowTypeFunc中
	local show_type = data.ShowType and data:ShowType() or nil
	-- LogError("show_type ",show_type)
	if show_type then
		local show_type_func = CellShowTypeFunc[show_type]
		if show_type_func then
			show_type_func(cell, data)
		end
	end
	if data.PESuit and data.pe_suit then
		cell.ZheZhaoTop:SetObjActive(true)
		UH.SpriteName(cell.ZheZhaoTop,data:PESuit())
	end

	if data.ShowType and data:ShowType() == ShowType.HuanShouSkillBook then
		local color = data:Color()
		if color >= 4 and color <= 6 then
			cell.ZheZhaoTop:SetObjActive(true)
			UH.SpriteName(cell.ZheZhaoTop,"HuanShouBook" .. color)
		end
	end

	if data.ShowType and data:ShowType() == ShowType.HuanShouSoul then
		local color = data:Color()
		if color >= 3 and color <= 9 then
			cell.ZheZhaoTop:SetObjActive(true)
			UH.SpriteName(cell.ZheZhaoTop,"HuanShouSoul" .. color)
		end
	end

	if data.ShowType and data:ShowType() == ShowType.HuanShouSPEgg then
		local color = data:Color()
		if color >= 3 and color <= 9 then
			cell.ZheZhaoTop:SetObjActive(true)
			UH.SpriteName(cell.ZheZhaoTop,"HuanShouEgg" .. color)
		end
	end

	if data.ShowType and data:ShowType() == ShowType.BicycleSkillBook then
		local color = data:Color()
		if color >= 4 and color <= 6 then
			cell.ZheZhaoTop:SetObjActive(true)
			UH.SpriteName(cell.ZheZhaoTop,"BicycleSkill" .. color)
		end
	end

	if data.is_rare_dw then
		UH.SpriteName(cell.DungeonRare, data.is_rare_dw)
		cell.DungeonRare:SetObjActive(true)
	end
	if data.is_up_dw then
		cell.DungeonUp:SetActive(true)
	end
	if data.is_rate_dw or (data.vo and data.vo.is_rate_dw) then
		cell.DungeonRate:SetActive(true)
		local mono = cell.DungeonRate:GetLuaComponent("DungeonRateCell")
		mono:SetData(data)
	end
	if data.IsGem and data:IsGem() then 
		if cell.StoneRecommend ~= nil then 
		    cell.StoneRecommend:SetActive(true)
		    local mono = cell.StoneRecommend:GetLuaComponent("StoneCellRecommend")
		    mono:SetData(data)
		end 
	end 
	if data.IsMarbas and data:IsMarbas() then 
		if cell.WithRefine then 
		    if cell.MarbasRefine ~= nil then 
		        cell.MarbasRefine:SetActive(true)
		        local mono = cell.MarbasRefine:GetLuaComponent("MarbasRefineCellPlus")
		        mono:SetData(data)
		    end 
		else 
			if cell.MarbasRefine ~= nil then 
				cell.MarbasRefine:SetActive(false)
			end 
	    end 
	end 
	if data.IsMedal and data:IsMedal() and data.param then 
		if data.param.sp_id and data.param.sp_id > 0 then 
		    local cfg = Cfg.MedalEffectById(data.param.sp_id)
		    if cfg then 
			    cell.MedalSpBg:SetActive(true)
			    cell.MedalSpEff:SetActive(true)
			    cell.MedalSpEff:GetComponent(typeof(UnityUI.Text)).text = cfg.name
			    UH.SpriteName(cell.MedalSpBg:GetComponent(typeof(Game.UIImageSpriteSet)),medal_sp_bg_sprite[data:GetMedalQua()])
		    end 		
	    end 

		if data.param.grade then 
			cell.RankPic:SetObjActive(data.param.grade ~= nil)
			local level_ = data:IsSpecialSp() and (( data.param.special_medal_grade and data.param.special_medal_grade > 1) and "n2" or 
			((data.param.special_medal_grade == nil or data.param.special_medal_grade == 0) and "n" or "n1")) or data.param.grade
			UH.SpriteName(cell.RankPic,MedalConfig.MedalRankIcon..level_)	
		end 
	end 
	if data.item_id and data.item_id > 0 and data.LevelView and cell.LevelView then
		local force_flag = (data:IsEquip() or data:IsJewelry()) and not data:IsStarGod() and cell.ForceLevelView == true
		local level = force_flag and data:LevelShow() or data:LevelView()
		cell.LevelView.text = (not StringIsEmpty(level) and string.format( Language.Main.Battle.Ji, level) or nil)
	end
	if data.vo and data.vo.top_right_str then
		cell.RtText.text = data.vo.top_right_str
	end
	
	local co = Item.GetConfig(data.item_id)
	if co and co.attr_view and co.attr_view ~=-1 then	
		if co.attr_view == 0 then
			if cell.AttributedanRole then
				cell.AttributedanRole:SetActive(true)
			end
		end
		
		if co.attr_view == 1 then
			if cell.AttributedanPet then
				cell.AttributedanPet:SetActive(true)
			end
		end
		
		if co.attr_view == 2 then
			if cell.AttributedanPartner then
				cell.AttributedanPartner:SetActive(true)
			end
		end
	end
end
------end------

------------------Common cell flush funcs------------------------

--背包格子--
function CellFlushs.BagGrid(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data.num)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
end
--背包格子--
function CellFlushs.SupplyGrid(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data.num)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
	UH.AnchoredPosition(cell.Bind:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.New(-29.5, -15))
	UH.AnchoredPosition(cell.RbTxt:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.New(-10, 36))
end

--a/b--
function CellFlushs.NeedFlush(cell)
	local data = cell:GetData()
	local num = data.num > 0 and data.num or Item.GetNum(data.item_id)
	local need_num = cell.need_num ~= nil and cell.need_num or data.need_num
	cell.RbTxt.text = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or QualityColorStr[ItemColor.Red], num, need_num)
	UH.LocalScale(cell.RbTxt.transform,Vector3.one)
end

--背包格子 1也显示--
function CellFlushs.BagGridShowOne(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data.num, true)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
end

--背包格子 不显示数字
function CellFlushs.BagGridNoNum(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, 1)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
end

--背包格子 只显示正数 不包括0
function CellFlushs.BagGridShowPositive(cell)
	local data = cell:GetData()
	if data.num ~= 0 then
		CellFlushs.SetNum(cell, data.num, true)
	end
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
end

function CellFlushs.SubGrid(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data:Num(), true)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
	CellFlushs.SubName(cell)
end

function CellFlushs.BagGridEquipment(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data.num)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
	local limit_prof = Equip.GetLimitProf(data.item_id)
	local flag_invalid = limit_prof ~= -1 and limit_prof ~= RoleData.Instance:GetRoleProfType()
	CellFlushs.SetEquipInvalid(cell, flag_invalid)
	CellFlushs.SetNotTreasure(cell)
end

-- 背包格子 背包界面
function CellFlushs.BagGridBagView(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	CellFlushs.SetNum(cell, data.num)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
	CellFlushs.SetEquipBetter(cell, data.is_better or false)
	CellFlushs.CancelStoneRecommend(cell)
end

function CellFlushs.TradePetSell(cell)
	CellFlushs.BagGrid(cell)
	local data = cell:GetData()
	if data.IsBind and data:IsBind() then
		cell.TradeLock:SetActive(true)
		cell.MengBan:SetActive(true)
	end
	if data.lock then
		cell.PetBind:SetActive(data.lock == 1)
	end
end

function CellFlushs.BagGridChatQuickItem(cell)
	local data = cell:GetData()
	CellFlushs.SetNum(cell, data.num)
	CellFlushs.SetBind(cell, data.is_bind)
	CellFlushs.MakeGrey(cell, data.is_grey)
	CellFlushs.ChatQuickShow(cell, data)
end

function CellFlushs.CancelStoneRecommend(cell)
	if cell.StoneRecommend ~= nil then 
		cell.StoneRecommend:SetActive(false)
	end 
end

------------------Common cell click funcs------------------------

CellClicks = CellClicks or {}

function CellClicks.BagGrid(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	-- cell.Selected:SetActive(not cell.Selected.activeSelf)
	local info = {
		item_list = {data, data:Compare()},
		func = function()
			return get_func_table(data)
		end,
		pos = Vector3.New(-400, 50, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

-- 增加了点击删除新物品
function CellClicks.BagGridClick(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	-- cell.Selected:SetActive(not cell.Selected.activeSelf)
	local info = {
		item_list = {data, data:Compare()},
		func = function()
			return get_func_table(data)
		end,
		pos = Vector3.New(-400, 50, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
	if BagData.Instance:IsNewItem() then 
		BagData.Instance:ClearOnesNewItem(data)
	end 
end 

function CellClicks.AnotherBagGridClick(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end

	local info = {
		item_list = {data, data:AnotherCompare()},
		func = function()
			return get_func_table(data,true)
		end,
		pos = Vector3.New(-400, 50, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function get_func_table(data,is_another)
	local func_t = {}
	local config = Item.GetConfig(data.item_id)
	if config == nil then
		Debuger.LogError("!!!!!!!!!!无配置id" .. tostring(data.item_id))
		return func_t
	end
	local is_equip = false
	local is_bag_spirit = true
	local is_bag_accrssory = true
	if data.IsEquip and data:IsEquip() then
		if is_another then 
			func_t = { ItemInfoFunc.AnotherEquip}
		else 
			--[[ if data.in_bag then
				if data:HadStone() then
					func_t = {ItemInfoFunc.Equip, ItemInfoFunc.Stone, ItemInfoFunc.UnStone, ItemInfoFunc.Smelt}
				else
					func_t = {ItemInfoFunc.Equip, ItemInfoFunc.Stone, ItemInfoFunc.Smelt}
				end
			else ]]
			func_t = {ItemInfoFunc.Equip}
			if Equip.IsDivine(data.item_id) then 
				table.insert(func_t,ItemInfoFunc.Divine)
			end 
			local max_num = WorkshopData.Instance:GetLockLimited(data)
			if max_num > 0 then 
				table.insert(func_t,ItemInfoFunc.Smelt)
			end 
			--[[ end ]]
			if (data:IsBlazeStar() or Equip.CanBlazeStar(data.item_id)) and (data:BlazeStar()<4) then 
				table.insert(func_t, ItemInfoFunc.BlazeStar)
			end 
			if WorkshopData.Instance:GetNevealIsOpen() then 
				table.insert(func_t, ItemInfoFunc.Neveal)
			end
			if data.IsCanConversion and data:IsCanConversion() then
				table.insert(func_t, ItemInfoFunc.EquipConversion)
			end
			if config.is_donate ~= 1 then
				table.insert(func_t,ItemInfoFunc.ReputationDonate)
			end
			-- if data.HasScore and data:HasScore() and FunOpen.Instance:GetFunIsOpened(Mod.Equip.Ulti) 
			--     and data.IsElder and not data:IsElder() then
			-- 	table.insert(func_t, ItemInfoFunc.UltiWork)
			-- end
			if data.CanResolve and data:CanResolve() then
				table.insert(func_t, ItemInfoFunc.Resolve)
			end
		end
		is_equip = true
	elseif data.IsJewelry and data:IsJewelry() then
		-- func_t = {ItemInfoFunc.Equip}
		-- local equip_btn = is_another == true and ItemInfoFunc.AnotherEquip or ItemInfoFunc.Equip
		-- func_t = {equip_btn,ItemInfoFunc.Smelt}
		-- if config.is_donate ~= 1 and not is_another then
		-- 	table.insert(func_t,ItemInfoFunc.ReputationDonate)
		-- end
		if is_another then 
			func_t = { ItemInfoFunc.AnotherEquip}
		else 
			--[[ if data.in_bag then
				if data:HadStone() then
					func_t = {ItemInfoFunc.Equip, ItemInfoFunc.Stone, ItemInfoFunc.UnStone, ItemInfoFunc.Smelt}
				else
					func_t = {ItemInfoFunc.Equip, ItemInfoFunc.Stone, ItemInfoFunc.Smelt}
				end
			else ]]
			func_t = {ItemInfoFunc.Equip}
			if Equip.IsDivine(data.item_id) then 
				table.insert(func_t,ItemInfoFunc.Divine)
			end 
			local max_num = WorkshopData.Instance:GetLockLimited(data)
			if max_num > 0 then 
				table.insert(func_t,ItemInfoFunc.Smelt)
			end 
			--[[ end ]]
			if (data:IsBlazeStar() or Equip.CanBlazeStar(data.item_id)) and (data:BlazeStar()<4) then 
				table.insert(func_t, ItemInfoFunc.BlazeStar)
			end 
			if data.IsCanConversion and data:IsCanConversion() then
				table.insert(func_t, ItemInfoFunc.EquipConversion)
			end
			if config.is_donate ~= 1 then
				table.insert(func_t,ItemInfoFunc.ReputationDonate)
			end
			-- if data.HasScore and data:HasScore() and FunOpen.Instance:GetFunIsOpened(Mod.Equip.Ulti) 
			--     and data.IsElder and not data:IsElder() then
			-- 	table.insert(func_t, ItemInfoFunc.UltiWork)
			-- end

			if data.CanResolve and data:CanResolve() then
				table.insert(func_t, ItemInfoFunc.Resolve)
			end
		end
		is_equip = true
	elseif config.item_type == ItemType.Illustration then
		func_t = {ItemInfoFunc.Collection}
	elseif config.item_type == ItemType.Medal then
		local opera_pos,is_full = BagData.Instance:GetMedalEmpty()
		-- local equip_btn = is_another == true and ItemInfoFunc.AnotherEquip or ItemInfoFunc.MedalEquip
		local equip_btn = ItemInfoFunc.Equip
		if is_another then 
			local opera_pos,is_full = AnotherData.Instance:GetMedalEmpty()
			equip_btn = is_full and ItemInfoFunc.AnotherMedalEquip or ItemInfoFunc.AnotherEquip
			func_t = {equip_btn}
		else 
			equip_btn = is_full and ItemInfoFunc.MedalEquip or ItemInfoFunc.Equip
			if data:IsSpecialSp() then 
				if data.param.special_medal_grade < 2 then 
					func_t = {equip_btn,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalMix,ItemInfoFunc.MedalSmelt}
				else
					func_t = {equip_btn,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalSmelt}
				end 
			elseif data.param.grade < 24 then 
				if data:WithSp() then 
					func_t = {equip_btn,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalUpgrade,ItemInfoFunc.MedalSmelt}
				else 
					func_t = {equip_btn,ItemInfoFunc.MedalUpgrade,ItemInfoFunc.MedalSmelt}
				end 
			else 
				func_t = {equip_btn,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalSmelt}
			end 
			if data:CanAllUpgrade() then 
				table.insert( func_t, ItemInfoFunc.MedalAllUpgrade )
			end 
		end 
	elseif config.item_type == ItemType.MedalFuling then
		func_t = {ItemInfoFunc.MedalEffectItem}
	elseif config.item_type == ItemType.EleCrystal then
		if is_another then 
		    func_t = {ItemInfoFunc.AnotherEquip}
		else 
			func_t = {ItemInfoFunc.Equip,ItemInfoFunc.PartnerEquip}
		end 
	elseif config.item_type == ItemType.PartnerEquip then
		local is_can_use = data:IsCanUse()
		func_t = {}
		if is_can_use then
			table.insert(func_t,ItemInfoFunc.Use)
			if FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerLife) then 
			    table.insert(func_t,ItemInfoFunc.PartnerEquipBreak)
			end
		end
	elseif config.show_type == ShowType.PetPieces
		or config.show_type == ShowType.PetCommonPieces then
		func_t = {ItemInfoFunc.PetPieceCompose}
		if PetData.Instance:IsCanMultiPieces(data.item_id) then
			table.insert(func_t, ItemInfoFunc.PetMulitiPieces1)
			table.insert(func_t, ItemInfoFunc.PetMulitiPieces2)
		end
	elseif config.show_type == ShowType.HeartSkillChip then
		func_t = {ItemInfoFunc.HeartSkillChip}
	elseif config.show_type == ShowType.HeartSkill then
		func_t = {ItemInfoFunc.Use}
		if config.color <= 5 then
			table.insert(func_t,ItemInfoFunc.HeartSkillVolume)
		end
	elseif config.show_type == ShowType.HeartSkillVolume then
		table.insert(func_t,ItemInfoFunc.HeartSkillVolume)
	elseif config.show_type == ShowType.FlyUp then
		if data.item_id == (FlyUpData.Instance:GetOtherConfig()["shilian_cost" .. FlyUpData.Instance:GetNextStage()] or 0) then
			func_t = {ItemInfoFunc.Use}
		end
	elseif config.show_type == ShowType.JewelryMaterial then
		func_t = {ItemInfoFunc.Use}
	elseif config.show_type == ShowType.PartnerWakeStone then
		func_t = {ItemInfoFunc.PartnerQuality}
	elseif Item.GetBigType(data.item_id) == ITEM_BIG_TYPE.PET_EQUIPMENT then
		if not is_another then
			func_t = data:IsCanUse() ~= 0 and {ItemInfoFunc.Use} or {}
			table.insert(func_t,ItemInfoFunc.PetEquipAssembly)
			table.insert(func_t,ItemInfoFunc.PetEquipReset)
			table.insert(func_t,ItemInfoFunc.PetEquipBreak)
			if AmuletData.Instance:IsCanIntensify(data) then
				table.insert(func_t,ItemInfoFunc.PetEquipInstensify)
			end
		else
			func_t = {ItemInfoFunc.AnotherPetEquip}
		end
	elseif config.show_type == ShowType.CrystalEquip then
		func_t = {}
		local is_can_use = data:IsCanUse()
		if is_can_use then
			table.insert(func_t,ItemInfoFunc.CrystalEquipIdentify)
		end
	elseif config.show_type == ShowType.PartnerPage then
		func_t = {ItemInfoFunc.PartnerPageCompose}
	elseif config.show_type == ShowType.GiftBag then
		func_t = {}
		local is_can_use = data:IsCanUse()
		if is_can_use ~= 0 then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		if is_can_use == 2 and data:Num() > 1 then
			if BagData.GetGiftSelectNum(data.item_id) == 1 then
				table.insert(func_t, ItemInfoFunc.UseBatch)
			else
				table.insert(func_t, ItemInfoFunc.UseAll)
			end
		end
		table.insert(func_t, ItemInfoFunc.PreviewGift)
	elseif config.show_type == ShowType.PetWeapon then
		func_t = {}
		table.insert(func_t,ItemInfoFunc.PetWeaponOnUse)
		table.insert(func_t,ItemInfoFunc.PetWeaponIntensify)
		table.insert(func_t,ItemInfoFunc.PetWeaponRefine)
		if config.color > 4 then
			table.insert(func_t,ItemInfoFunc.PetWeaponChange)
		end
	elseif config.show_type == ShowType.LiveFood then
		func_t = {}
		local is_can_use = data:IsCanUse()
		if is_can_use ~= 0 then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		table.insert(func_t, ItemInfoFunc.LiveGive)
	elseif config.show_type == ShowType.EquipSpirit then
		func_t = {}
		if EquipData.Instance.now_select_equip_data then
			-- 在铸灵界面点到的铸灵不需要显示出售按钮
			is_bag_spirit = false
			-- 满级的铸灵不能升级
			if not EquipData.Instance:GetSpiritIsMaxLevel(data.item_id, data.param.level) then
				table.insert(func_t, ItemInfoFunc.SpiritStoneLevelUp)
			end
			-- 铸灵界面才有替换
			table.insert(func_t, ItemInfoFunc.SpiritStoneReplace)
			table.insert(func_t, ItemInfoFunc.SpiritStoneRemove)
		else
			table.insert(func_t, ItemInfoFunc.SpiritStoneUse)
			-- 满级的铸灵不能升级
			if not EquipData.Instance:GetSpiritIsMaxLevel(data.item_id, data.param.level) then
				table.insert(func_t, ItemInfoFunc.SpiritStoneLevelUp)
			end
			table.insert(func_t, ItemInfoFunc.SpiritStoneResolve)
		end
	elseif config.show_type == ShowType.Accrssory then
		func_t = {}
		if data.in_bag then
			table.insert(func_t, ItemInfoFunc.AccrssoryUse)
			table.insert(func_t, ItemInfoFunc.AccrssoryResolve)
		else
			is_bag_accrssory = false
			table.insert(func_t, ItemInfoFunc.AccrssoryRemove)
		end
		table.insert(func_t, ItemInfoFunc.AccrssoryChange)
	elseif config.show_type == ShowType.LingBao then
		func_t = {}
		if data.in_bag then
			table.insert(func_t, ItemInfoFunc.LingBaoUse)
		end
	elseif config.show_type == ShowType.PetSkillBook then
		func_t = {ItemInfoFunc.PetSkillBookUse}
		if config.color >= 5 then
			table.insert(func_t, ItemInfoFunc.PetBookChange)
		end
	elseif config.show_type == ShowType.GlassTripod then
		func_t = {ItemInfoFunc.GlassTripod}
	elseif config.show_type == ShowType.DungeonSealStone then
		func_t = {ItemInfoFunc.DungeonSealStone}
	elseif config.show_type == ShowType.Neveal then
		func_t = {ItemInfoFunc.Use}

		if data.param.has_param == 1 and data.param.effect_id > 0 then  
			-- ItemInfoFunc.NevealExtralPlus,
			if data.param.effect_level >= 10 and data.param.effect_level < 20 then 
				table.insert(func_t,ItemInfoFunc.NevealExtralPlus)
			end 
			
		    table.insert(func_t,ItemInfoFunc.NevealResolve)
		end
	elseif config.show_type == ShowType.PetBagdeClip then
		func_t = {ItemInfoFunc.PetBagdeCpd}
	elseif config.show_type == ShowType.PetBagde then
		func_t = {}
		local is_can_use = data:IsCanUse()
		if is_can_use ~= 0 then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		table.insert(func_t, ItemInfoFunc.PetBagdeResolve)
	elseif config.show_type == ShowType.ActItem then
		local is_can_use = data:IsCanUse()
		if 0 ~= is_can_use then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		if ConcentricKnotData.Instance:IsSelfItem(data.item_id) then
			table.insert(func_t, ItemInfoFunc.ConcentricKnotSend)
		end
		if 2 == is_can_use and data:Num() > 1 then
			table.insert(func_t, ItemInfoFunc.UseAll)
		end
	elseif config.show_type == ShowType.SpecialCompose then
		table.insert(func_t, ItemInfoFunc.SpecialCompose)
	elseif data.IsMarbas and data:IsMarbas() then
		table.insert(func_t, ItemInfoFunc.MarbasEquip)
	elseif data.IsMarbasGem and data:IsMarbasGem() then
		table.insert(func_t, ItemInfoFunc.MarbasBloodGemDecompose)
		table.insert(func_t, ItemInfoFunc.MarbasBloodMulitGemDecompose)
	elseif MarbasData.Instance:GetIsMarbasGemComposeItem(data.item_id) then
		table.insert(func_t, ItemInfoFunc.MarbasBloodGemCompose)
	elseif config.show_type == ShowType.ShengQiSkillBook then
		func_t = {}
		table.insert(func_t,ItemInfoFunc.Use)
		if data:Color() >= 5 then
			table.insert(func_t,ItemInfoFunc.ShengQiSkillChange)
		end
	elseif config.show_type == ShowType.HuanShouSoul then
		func_t = {}
		local is_can_use = data:IsCanUse()
		if is_can_use ~= 0 then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		table.insert(func_t, ItemInfoFunc.HuanShouResolve)
	elseif config.item_type == ItemType.FuYu then
		if data.wear_idx == -1 then
			-- table.insert(func_t, ItemInfoFunc.FuYuTaskOff)
			--[[ if ViewMgr:IsOpen(BagView) then
				table.insert(func_t, ItemInfoFunc.FuYuUse)
			else
				table.insert(func_t, ItemInfoFunc.FuYuChange)
			end ]]
			-- if ViewMgr:IsOpen(PartnerFuYuBag) then
				local item =  PartnerData.Instance:GetFuYuClickBodyItem()
				if item == nil then
					table.insert(func_t, ItemInfoFunc.FuYuUse)
				else
					if item.is_empty == true then
						table.insert(func_t, ItemInfoFunc.FuYuUse)
					else
						table.insert(func_t, ItemInfoFunc.FuYuChange)
					end
				end
			-- end
			table.insert(func_t, ItemInfoFunc.FuYuQiangHua)
			table.insert(func_t, ItemInfoFunc.FuYuFenJie)
			if data:SpId() > 0 and not data:IsExpired() then
				table.insert(func_t, ItemInfoFunc.FuYuSwitch)
			end
			if data:IsExc() and not data:IsGod() then
				table.insert(func_t, ItemInfoFunc.FuYuShengHua)
			end
		else
			table.insert(func_t, ItemInfoFunc.FuYuTakeOff)
			table.insert(func_t, ItemInfoFunc.FuYuChange)
			table.insert(func_t, ItemInfoFunc.FuYuQiangHua)
			table.insert(func_t, ItemInfoFunc.FuYuFenJie)
			if data:SpId() > 0 and not data:IsExpired() then
				table.insert(func_t, ItemInfoFunc.FuYuSwitch)
			end
			if data:IsExc() and not data:IsGod() then
				table.insert(func_t, ItemInfoFunc.FuYuShengHua)
			end
		end
	elseif config.show_type == ShowType.BicycleEquip then
		table.insert(func_t, ItemInfoFunc.Use)
		-- 灵骑装备，是否显示转换按钮
		local item_id = BicycleData.Instance:ChangeItemList(BicycleCfg.ChangeItem.Equip)
		if RoleLevel() >= Item.GetLimitLv(item_id) then
			table.insert(func_t, ItemInfoFunc.LingQiChangeEquip)
		end
	elseif config.show_type == ShowType.IllusionChange then
		table.insert(func_t, ItemInfoFunc.Use)
		-- 时装转换后道具分解按钮
		table.insert(func_t, ItemInfoFunc.IllusionChangeResolve)
	elseif config.show_type == ShowType.Halidom then
		table.insert(func_t, ItemInfoFunc.Use)
		-- 灵骑圣物，是否显示转换按钮
		local item_id = BicycleData.Instance:ChangeItemList(BicycleCfg.ChangeItem.Halidom)
		if RoleLevel() >= Item.GetLimitLv(item_id) then
			table.insert(func_t, ItemInfoFunc.LingQiChangeHalidom)
		end
	elseif config.show_type == ShowType.BicycleSkillBook then
		local is_can_use = data:IsCanUse()
		if 0 ~= is_can_use then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		local skill_id = config.param1
		local max_level = BicycleData.Instance:GetSkillMaxLevel(skill_id)
		local level = config.param3
		if level < max_level then
			table.insert(func_t, ItemInfoFunc.BicycleSkillCompose)
			table.insert(func_t, ItemInfoFunc.LingQiYiJianHeCheng)
		end
	elseif config.item_type == ItemType.YouMingEquip then
		if data.in_bag == true then
			if data:Compare() then
				table.insert(func_t, ItemInfoFunc.YouMingTiHuang2)
			else
				table.insert(func_t, ItemInfoFunc.Use)
			end
		else
			table.insert(func_t, ItemInfoFunc.YouMingEquipUnfix)
			table.insert(func_t, ItemInfoFunc.YouMingTiHuang)
		end
	else
		local is_can_use = data:IsCanUse()
		if 0 ~= is_can_use then
			table.insert(func_t, ItemInfoFunc.Use)
		end
		if 2 == is_can_use and data:Num() > 1 then
			table.insert(func_t, ItemInfoFunc.UseAll)
		end
		if config.show_type == ShowType.TreasureMap and config.id == CommonItemId.SuperMapItem then
			table.insert(func_t, ItemInfoFunc.BatchDig)
		end
		-- local is_medal = Item.IsMedalChange(data.item_id)
		-- if is_medal then
		-- 	table.insert(func_t, ItemInfoFunc.ExChange)
		-- end
	end
	if RewardData.Instance:CheckShowExchange(data.item_id) then 
		table.insert(func_t, ItemInfoFunc.Recycling)
	end 
	if not data.is_bind and data:CanTrade() and not is_another then
		table.insert(func_t, ItemInfoFunc.Trade)
	end
	local btn_list = data:BtnList()
	if #btn_list > 0 then
		for _, v in pairs(btn_list) do
			if ItemInfoFunc[v] then
				table.insert(func_t, ItemInfoFunc[v])
			end
		end
	end
	if data:Price() ~= 0 and not is_another and not is_equip and is_bag_spirit and is_bag_accrssory then
		table.insert(func_t, ItemInfoFunc.Sell)
	end
	if data:IsTreasure() then
		func_t[1] = ItemInfoFunc.UseMap
	end
	--[[ if data.item_id == CommonItemId.FabaoSP or ShowType.Fashion == data:ShowType() then
		table.insert(func_t, ItemInfoFunc.Resolve)
	end ]]
	return func_t
end

function get_other_func_table(data)
	local func_t = {}
	local config = Item.GetConfig(data.item_id)
	if config == nil then
		Debuger.LogError("!!!!!!!!!!无配置id" .. tostring(data.item_id))
		return func_t
	end
	if config.show_type == ShowType.GiftBag then
		table.insert(func_t, ItemInfoFunc.PreviewGift)
	end
	return func_t
end

function CellClicks.EquipGrid(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function(item)
			if data.IsMedal and data:IsMedal() then 
				return {ItemInfoFunc.UnFix}
			elseif data.IsMarbas and data:IsMarbas() then 
				return {ItemInfoFunc.MarbasChange}--,ItemInfoFunc.MarbasUnFix}
			else
				if data:IsEquip() or data:IsJewelry() then
					local temp
					if data:HadStone() then
						-- temp = {ItemInfoFunc.UnFix,ItemInfoFunc.Stone, ItemInfoFunc.UnStone, ItemInfoFunc.Smelt}
						temp = {ItemInfoFunc.UnFix}
					else 
						-- temp = {ItemInfoFunc.UnFix,ItemInfoFunc.Stone, ItemInfoFunc.Smelt}
						temp = {ItemInfoFunc.UnFix}
					end 
					if Equip.IsDivine(data.item_id) then 
						table.insert(temp,ItemInfoFunc.Divine)
					end 
					local max_num = WorkshopData.Instance:GetLockLimited(data)
					if max_num > 0 then 
						table.insert(temp, ItemInfoFunc.Smelt)
					end 
					-- if data.HasScore and data:HasScore() and FunOpen.Instance:GetFunIsOpened(Mod.Equip.Ulti) 
					--     and data.IsElder and not data:IsElder() then
					-- 	table.insert(temp, ItemInfoFunc.UltiWork)
					-- end
					if (data:IsBlazeStar() or Equip.CanBlazeStar(data.item_id)) and (data:BlazeStar()<4) then 
						table.insert(temp, ItemInfoFunc.BlazeStar)
					end 
					if data:IsEquip() and WorkshopData.Instance:GetNevealIsOpen() then 
						table.insert(temp, ItemInfoFunc.Neveal)
					end 
					if data.IsCanConversion and data:IsCanConversion() then
						table.insert(temp, ItemInfoFunc.EquipConversion)
					end
					return temp
				else 
					local temp = {ItemInfoFunc.UnFix}
					local max_num = WorkshopData.Instance:GetLockLimited(data)
					if max_num > 0 then 
						table.insert(temp, ItemInfoFunc.Smelt)
					end 
					return temp
				end
			end 
		end,
		pos = Vector3.New(0, 140, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.AnotherBagGridEquip(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function(item)
			if data.IsMedal and data:IsMedal() then 
				return {ItemInfoFunc.AnotherUnFix}
			else
				return {ItemInfoFunc.AnotherUnFix}
			end 
		end,
		pos = Vector3.New(0, 140, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.EquipGridNoButton(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local equip_type = data:EquipType()
	local compare_data
	if equip_type then
		compare_data = BagData.Instance:GetEquipList(equip_type) or BagData.Instance:GetJewelryList(equip_type-5)
		if compare_data then
			if compare_data.item_unique_id_1 == data.item_unique_id_1 and compare_data.item_unique_id_2 == data.item_unique_id_2 then
				compare_data = nil
			end
		end
	end
	local info = {
		item_list = {data, compare_data},
		pos = Vector3.New(0, 140, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.BagGridNoButton(cell)
	if cell == nil then
		return
	end
	if cell.GetData == nil then
		return
	end
	local data = cell:GetData()
	if data == nil then
		return
	end

	if data.pet_id then
		--ViewMgr:OpenView(PetDetailView,data)
		PetCtrl.Instance:OpenActPetDetailView(data)
		return
	end

	local co = Item.GetConfig(data.item_id)
	if co and co.show_type and co.show_type == ShowType.Pet then
		local pet = Pet.New({item_id = data.item_id})
		PetCtrl.Instance:OpenActPetDetailView(pet)
		return
	end

	if co and co.show_type and co.show_type == ShowType.YouMing then
		ViewMgr:OpenView(YouMingInfoView, {youming = data})
		return
	end

	local info = {
		item_list = {data},
		func = function ()
			return get_other_func_table(data)
		end,
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.BagGridNoButtonGrey(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	data.is_grey = false
	local info = {
		item_list = {data}
	}
	if data.pet_id then
		ViewMgr:OpenView(PetDetailView,data)
		return
	end
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.StorageSGrid(cell)
	local data = cell:GetData()
	local info = {
		item_list = {data},
		func = function(item)
			return {ItemInfoFunc.Fetch}
		end
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.StorageBGrid(cell)
	local data = cell:GetData()
	local info = {
		item_list = {data},
		func = function(item)
			return {ItemInfoFunc.Store}
		end
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.BagGridChatQuickItem(cell)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	if cell == nil then return end
	local data = cell:GetData()
	if data == nil then return end
	local chat_data = ChatCtrl.Instance:Data()
	local chat_item = nil
	if data.chat_quick_show_equip then
		chat_item = ChatItemMonoEquip.New(data)
	elseif data.chat_quick_show_jewelry then
		chat_item = ChatItemMonoJewelry.New(data)
	elseif data.chat_quick_show_medal then
		chat_item = ChatItemMonoMedal.New(data)
	else
		chat_item = ChatItemMono.New(data)
	end
	chat_data.chat_quick_input:Set(chat_item)
	ViewMgr:CloseView(ChatQuickView)
end

--交易行出售
function CellClicks.OnClickSell(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	if data.item_id == 0 then
		return
	end
	local param = {item = data,cur_status = 0}
	if param.item.num == 1 and param.item:GroupNum() == 1 then
		if param.item:IsEquip() or param.item:IsJewelry() or param.item:IsMedal() then
			ViewMgr:OpenView(EquipSellView,param)
			return
		end
		ViewMgr:OpenView(MallSellView,param)
	else
		ViewMgr:OpenView(SellView,param)
	end
end

--宠物出售的
function CellClicks.OnClickPetSell(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	if data.item_id == 0 then
		return
	end
	if data.lock and data.lock == 1 then
		PublicPopupCtrl.Instance:AlertTip(Language.Trade.CanNotTradeLockPet,function()
			PetCtrl.SendPetOp(PET_OP_TYPE.Lock, data.index)
		end)
		return
	end
	if data.IsBind and data:IsBind() then
		PublicPopupCtrl.Instance:Center(Language.Trade.CanNotTradeBindPet)
		return
	end
	ViewMgr:OpenView(PetSellView,{pet = data,cur_status = 0})
end

--打开小地图的
function CellClicks.OnClickMapMini(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	if data.pet_id or data.monster_id then
		ViewMgr:OpenView(MapInfoView,{item = data,root_pos = UH.GetRootPosition(cell.gameObject),
		funcs = {{name = Language.Activity.RemindTips[1]},{name = Language.Pet.SeedTitle,enter_click = function()
			ViewMgr:OpenViewByKey(Mod.Pet.PetBook)
		end}}})
	else
		CellClicks.BagGridNoButton(cell)
	end
end

--只有出售按钮的
function CellClicks.BagGridSell(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function()
			return {ItemInfoFunc.Sell}
		end,
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

--宠物弹出框 只有百科
function CellClicks.OnClickPetPopup(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	if data.pet_id or data.monster_id then
		ViewMgr:OpenView(MapInfoView,{item = data,root_pos = UH.GetRootPosition(cell.gameObject),
		funcs = {{name = Language.Pet.SeedTitle,enter_click = function()
			ViewMgr:OpenViewByKey(Mod.Pet.PetBook)
		end}}})
	else
		CellClicks.BagGridNoButton(cell)
	end
end

function CellClicks.PetEquiped(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
				return { ItemInfoFunc.PetEquipedInstead, ItemInfoFunc.PetEquipReset, ItemInfoFunc.PetEquipAssembly, ItemInfoFunc.PetEquipedRemove }
			end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

-- 宠物徽章
function CellClicks.PetBadged(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
				return { ItemInfoFunc.PetBagdeReplace, ItemInfoFunc.PetBagdeUnfix}
			end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.AnotherPetEquiped(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
				return { ItemInfoFunc.AnotherPetEquipedInstead, ItemInfoFunc.AnotherPetEquipedRemove }
			end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.PetWeaponBag(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
			local func = {}
			if data:ShowType() ~= ShowType.PetWeapon then
				return func
			end
			if PetWeaponData.Instance:CurWeapon() then
				func = { ItemInfoFunc.PetWeaponReplace }
			else
				func = { ItemInfoFunc.PetWeaponOnUse}
			end
			return func
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.ShengQiBag(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	ShengQiCtrl.ClickShengQi(data)
end

function CellClicks.FuShiUse(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	ShengQiCtrl.ClickFuShi(data)
end

function CellClicks.PetWeapon(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
			return { ItemInfoFunc.PetWeaponUnfix}
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.PetStamp(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
			local buleprint = PetStampData.Instance:GetCurBuleprint()
			if buleprint then
				local pet_index = buleprint:PetIndex()
				if pet_index then
					return { ItemInfoFunc.PetStampUnfix, ItemInfoFunc.PetStampIntensify}
				else
					return { ItemInfoFunc.PetStampBlueprintUnfix}
				end
			end
			return {}
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end


function CellClicks.PetStampBag(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	local buleprint = PetStampData.Instance:GetCurBuleprint()
	local pet_stamp
	if buleprint then
		local pos_index = data:PosIndex()
		pet_stamp = buleprint:GetStampByPos(pos_index)
	end
	local info = {
		item_list = {data,pet_stamp},
		func = function ()
			local func = {}
			if data:ShowType() ~= ShowType.PetStamp then
				return func
			end
			if buleprint then
				local is_open_blutprint = ViewMgr:IsOpen(PetStampBlueprintView)
				local pet_index = buleprint:PetIndex()	-- 不为空时则是宠物界面，否则是方案
				local index = buleprint:Index()
				-- 有神印且有宠物, 替换，分解
				if pet_index and (not is_open_blutprint or index < 0) then
					if pet_stamp then
						func = { ItemInfoFunc.PetStampReplace, ItemInfoFunc.PetStampResolve, ItemInfoFunc.PetStampIntensify}
					else
						func = { ItemInfoFunc.PetStampOnUse, ItemInfoFunc.PetStampResolve, ItemInfoFunc.PetStampIntensify}
					end
					if data:QuaLevel() >= 5 then
						table.insert(func, ItemInfoFunc.PetStampChange)
						if data:Level() >= 20 and data:QuaLevel() >= 6 and not data:Swallowed() then
							table.insert(func, 2, ItemInfoFunc.PetStampSwallow)
						end
					end
					if DEBUG_MODE then
						table.insert(func, ItemInfoFunc.PetStampDrop)
					end
				else
					if pet_stamp then
						func = { ItemInfoFunc.PetStampBlueprintReplace,}
					else
						func = { ItemInfoFunc.PetStampBlueprintPut,}
					end
				end
			end
			return func
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
	ItemInfoData.Instance:SetClickDataPos(Vector2.New(0,75))
end

function CellClicks.BagGridSocietySend(cell)
	if SocietyData.Instance:IsSendGiftBlockItemInfo() then return end
	local data = cell:GetData()
	if data == nil then
		return
	end
	local info = {
		item_list = {data},
		func = function ()
			return get_other_func_table(data)
		end,
		show_send = SocietyConfig.ShareType.GodLandTrace == SocietyData.Instance.send_info_sm.type
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.GetWayView(cell)
	local item = cell:GetData()
	MainOtherCtrl.Instance:GetWayView({item = item})
end

function CellClicks.BagGridMayDayRewardGet(cell)
	-- local data = cell:GetData()
	-- if data == nil then
	-- 	return
	-- end

	-- if data.pet_id then
	-- 	ViewMgr:OpenView(PetDetailView,data)
	-- 	return
	-- end

	-- local info = {
	-- 	item_list = {data},
	-- 	func = function ()
	-- 		return get_other_func_table(data)
	-- 	end,
	-- }
	-- ItemInfoCtrl.Instance:ItemInfoView(info)

	MayDayCtrl.Instance:SendMainDayRewardGet()
end

function CellClicks.MarbasGemBagClick(cell)
	local data = cell:GetData()
	if data == nil then
		return
	end
	-- cell.Selected:SetActive(not cell.Selected.activeSelf)
	local info = {
		item_list = {data },
		func = function()
			local next_item = MarbasData.Instance:GetGemNextLevel(data.item_id)     
			local temp = {ItemInfoFunc.MarbasBloodGemEquip}
			if next_item.item_id ~= data.item_id then 
				table.insert(temp,ItemInfoFunc.MarbasBloodGemPlus)
			end 
			return temp
		end,
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function CellClicks.QQBigPlayerWeekGift(cell)
	local data = cell:GetData()
	QQBigPlayerCtrl.Instance:OnClickViewItem(data)
end
function CellClicks.DaZhanHongTuClick(cell)
	if DaZhanHongTuData.Instance:DaZhanHongTuRewardCanGet() then
		DaZhanHongTuCtrl.SendGetReward()
	else
		CellClicks.BagGridNoButton(cell)
	end
end
---------------------
CellSelects = CellSelects or {}
function CellSelects.BagGrid(cell, selected)
	cell.Selected:SetActive(selected)
end

function CellSelects.ReputationBatchedDonation(cell, selected)
	cell.Selected:SetActive(selected)
end

function CellSelects.ItemsSub(cell, selected)
	cell.Selected:SetActive(true)
end

function CellSelects.BagGridSocietySend(cell, selected)
	cell.Selected:SetActive(selected)
	local send_num = ""
	if selected then
		local data = cell:GetData()
		if nil ~= data then
			SocietyData.Instance:SetSendInfoSelItem(data.item_id)
		end
		send_num = SocietyData.Instance:GetSendInfoSendNum()
		send_num = send_num > 0 and send_num or ""
	end
	cell.SocietySend.text = send_num
end


PartnerPactCell = PartnerPactCell or DeclareMono("PartnerPactCell")

function PartnerPactCell:SetData(data)
	local star_level = 0
	if data.StarLevel then
		star_level = data:StarLevel()
	else
		local item = Item.Init(data.item_id)
		star_level = item:StarLevel()
	end
	UH.LocalPos(self.Root,Vector3.New(0,data.StarLevel and -32 or -50,0))
	for i = 1, self.Stars:Length() do
		self.Stars[i].SpriteName = star_level >= i and "xingxinghong" or "xingxinghui"
	end
end

NevealCellShow = NevealCellShow or DeclareMono("NevealCellShow")
function NevealCellShow:SetData(data)
	local info = data:Neveal()
	local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
	UH.SetText(self.eff_name, cfg.effect_name)
	UH.SetText(self.eff_level, cfg.effect_level..Language.Common.Level)
end 

BlazeStarCellShow = BlazeStarCellShow or DeclareMono("BlazeStarCellShow")
function BlazeStarCellShow:SetData(data)
	local star_num = data:BlazeStar() - 1
	for i = 1,5 do 
		self.stars[i]:SetActive(star_num >= i)
	end 
end

DungeonRateCell = DungeonRateCell or DeclareMono("DungeonRateCell")

function DungeonRateCell:SetData(data)
	self.RateShow.text = (data.is_rate_dw or data.vo.is_rate_dw) .. "%"
end

StoneCellRecommend = StoneCellRecommend or DeclareMono("StoneCellRecommend")
function StoneCellRecommend:SetData(data)
	local yes_flag = WorkshopData.Instance:GetProfRecommendAllPart(data.item_id)
	local no_flag = WorkshopData.Instance:GetProfNotRecommendAllPart(data.item_id)
	self.yes:SetActive(yes_flag)
	self.no:SetActive(no_flag)

	self.bg:SetActive(yes_flag or no_flag)
end 

MarbasRefineCellPlus = MarbasRefineCellPlus or DeclareMono("MarbasRefineCellPlus")
function MarbasRefineCellPlus:SetData(data)
	local level = MarbasData.Instance:GetRefineInfo(data:EquipType())
	UH.SetText(self.plus_num, "+"..level)
	-- local flag = not data.in_bag 
	-- self.bg:SetActive(level > 0 and flag)
end 