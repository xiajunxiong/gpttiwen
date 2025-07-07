
local cell_full = nil
PreloadCtrl.RegisterPreload(
    function ()
    	PreloadCtrl.RegisterLoading()
        ResMgr:LoadGameObject("ui/widgets/item_cell_full.prefab",
        	function (obj)
        		cell_full = obj:GetLuaComponent("LuaMonoBehaviour")
        		GameObjectPool:RegisterPool("ui/widgets/item_cell.prefab",function(error)PreloadCtrl.UnregisterLoading()end,true)
        		
        	end
        	,false,false)
    end)


-- local SelType = {
-- 	None = nil,	--无交互 取消点击事件
-- 	Click = 0,	--点击
-- 	Toggle = 1,	--选中类型
-- }

UIWItemCell = UIWItemCell or DeclareMono("UIWItemCell")
UIWItemCell.FrameName = {
	"WuPinKuangDi",
	"WuPinKuangDi_Kong",
	"JiNengKuang",
}
-----Interactable==Bool----------是否有交互
-----FlushFunc===String

-----ClickEventTab====LuaMonoBehaviour----为空则从CellClicks里面取公共函数
-----ClickFunc===String----点击事件函数名

-----SelectEventTab====LuaMonoBehaviour----为空则从CellSelects里面取公共函数
-----SelectFunc==String----选择事件函数名

-----SelectorGroup========ToggleGroup
-----Dark==Bool-----------是否是深色底

-----CellSize==string-----格子大小


-----Icon========UIImageSpriteSet
-----Frame=======UIImageSpriteSet
-----IconClick===LuaUIEventClik
-----Selector======Toggle
-----QuaIcon=======UIImageSpriteSet\

-----IsLongClick===bool
-----LongClickEventTab====LuaMonoBehaviour----为空则从CellClicks里面取公共函数
-----LongClickFunc===String----点击事件函数名

--DynamicSerializeField=====String
local dynamic_ch_names = {
	RbTxt = typeof(UnityUI.Text),		--右下角的文本		Text
	Selected = typeof(UnityGameObj),	--选中框				GameObject
	Bind = typeof(UnityGameObj),		--绑定图标
	MengBan = typeof(UnityGameObj),		--蒙版
	SkillLock = typeof(UnityGameObj),	--技能锁
	AddIcon = typeof(UnityGameObj),		--添加图片
	CatchIcon = typeof(Game.UIImageSpriteSet), --宠物品质框
	ZheZhaoTop = typeof(Game.UIImageSpriteSet),	--遮罩(技能书、魂魄等物品格子的贴膜)
	SkillQuaIcon = typeof(UnityGameObj), --技能书品质
	DungeonRare = typeof(Game.UIImageSpriteSet), --副本奖励品质
	DungeonRate = typeof(UnityGameObj), --副本奖励概率
	SubTxt = typeof(UnityUI.Text), 		--物品提交界面物品名
	Mutant = typeof(UnityGameObj),		--宠物变异框
	LianYao = typeof(UnityGameObj),		--宠物炼妖框
	JieImage = typeof(Game.UIImageSpriteSet),	--宠物强化等级
	EggJieImage = typeof(Game.UIImageSpriteSet), --宠物蛋强化等级
	EquipBetter = typeof(UnityGameObj), --装备优良
	EquipInvalid = typeof(UnityGameObj), --装备未生效
	TradeLock = typeof(UnityGameObj), 	--不可交易锁
	JiaoBiao = typeof(UnityGameObj),  	--活动角标
	MedalSpEff = typeof(UnityGameObj),	--勋章的特效字	
	MedalSpBg = typeof(UnityGameObj), --勋章的特效底
	CellRank = typeof(UnityGameObj), -- 物品评分
	CellRankMenban = typeof(UnityGameObj), -- 物品评分
	IsGetObj = typeof(UnityGameObj), -- 已获取标签(已激活配方角标)
	FormulaUnique = typeof(UnityGameObj), -- 配方唯一角标
	NewTips = typeof(UnityGameObj), 		--背包中新物品的“新”标记 预设上尽量保持这个的层级最高
	RankPic = typeof(Game.UIImageSpriteSet), -- 令牌阶级
	FulingProf = typeof(Game.UIImageSpriteSet), -- 附灵石角标
	PetBind = typeof(UnityGameObj),		--宠物绑定图标
	PetLock = typeof(UnityGameObj),		--宠物锁的图标
	LevelView = typeof(Game.SpriteText),		--level_view
	GodEffectOrange = typeof(UnityGameObj),
	GodEffectRed = typeof(UnityGameObj),
	BlackEffect = typeof(UnityGameObj),
	DungeonUp = typeof(UnityGameObj), --副本奖励概率提升
	QuaPoints = typeof(Game.SpriteText), -- 品质圆点
	WeaponLock = typeof(UnityGameObj),
	PetStampQua = typeof(Game.UIImageSpriteSet),
	--PetStampPoints = typeof(Game.SpriteText),
	PetStampIcon = typeof(Game.UIImageSpriteSet),
	EquipLevelObj = typeof(UnityGameObj),
	EquipLevelText = typeof(UnityUI.Text),
	LevelJieDi = typeof(UnityGameObj),
	LevelJie = typeof(UnityUI.Text),
	UpgradeObj = typeof(UnityGameObj),
	UpgradeText = typeof(UnityUI.Text),
	PetStampPos = typeof(UnityGameObj),
	PetStampPosText = typeof(UnityUI.Text),
	PetStampLevel = typeof(UnityGameObj),
	PetStampLevelText = typeof(UnityUI.Text),
	RtText = typeof(UnityUI.Text),
	ChatQuickObj = typeof(UnityGameObj),	--聊天物品穿戴中的装备、灵饰、令牌角标
	SocietySend = typeof(UnityUI.Text),		--社交赠送界面物品角标
	AttributedanRole = typeof(UnityGameObj),
	AttributedanPet = typeof(UnityGameObj),
	AttributedanPartner = typeof(UnityGameObj),
	PartnerPact = typeof(UnityGameObj),		--伙伴元神契约
	AccrssoryTitleBg = typeof(UnityGameObj),	--配饰角标
	TxtAccrssoryTitle = typeof(UnityUI.Text),	--配饰角标
	Neveal = typeof(UnityGameObj),		--熔铸信息
	BlazeStar = typeof(UnityGameObj),		--熔铸信息
	LiveCard = typeof(UnityGameObj),	-- 变身框
	GoldEffect = typeof(UnityGameObj), -- 彩色品質
	HalidomFlag = typeof(UnityGameObj),-- 圣物标记
	FSQuaAnim = typeof(Game.UISpriteAnimTool),	--飞升特效
	EffectFlag = typeof(UnityGameObj),-- 特效标记
	AccrssoryStrengthBg = typeof(UnityGameObj),		--配饰等级
	TxtAccrssoryStrength = typeof(UnityUI.Text),	--配饰等级
	StoneRecommend = typeof(UnityGameObj), --宝石推荐
	MarbasRefine = typeof(UnityGameObj),-- 造化装备·精炼功能
	FuShiDi = typeof(UnityGameObj),-- 符石底
	FuShiFlag = typeof(Game.SpriteText),	-- 符石标记
	FuYuLevel = typeof(UnityGameObj),--符玉等级
	SwallowStar = typeof(UnityGameObj),	--神印吞噬星
	BicycleSkillFrame = typeof(Game.UIImageSpriteSet), -- 灵骑技能框
	LimitFlag = typeof(UnityGameObj),	-- 限定标记
	ObjLingBaoState = typeof(UnityGameObj),	--灵宝阶数
	TxtLingBaoState = typeof(UnityUI.Text),	--灵宝阶数
	ObjLingBaoStar = typeof(UnityGameObj),	--灵宝星星
	DragonSoulType = typeof(Game.UIImageSpriteSet),
}

--将已创建的动态控件对象还原到创建之前的状态，优化控件初始刷新时的新控件创建数量
local type_clear_flush_funcs = {
	[typeof(UnityUI.Text)] = function (text) text.text = nil end,
	[typeof(UnityGameObj)] = function (obj) obj:SetActive(false) end,
	[typeof(Game.UIImageSpriteSet)] = function (obj) obj.SpriteName = nil end,
	[typeof(Game.SpriteText)] = function (obj) obj.text = nil end,
	[typeof(Game.UISpriteAnimTool)] = function (anim) anim:SetObjActive(false) end
}

function UIWItemCell:UIWItemCell()
	self.select_group = nil
	self.on_click = nil
	self.on_seleted = nil
	self.dynamic_chs = {}	--所有动态结点	key:ch_name,value:unity object
	self.dynamic_sibling_cache = {}
	self.used_chs ={}	--标识正在使用的动态结点 key:ch_name,value:true or false or nil
	self.is_grey = false
	self.size = Vector3.one
end

function UIWItemCell:Awake()
	self.IsLongClick = self.IsLongClick or false
	if self.IsLongClick == false then
		if self.PressDown and self.PressUp then
			self.PressDown.enabled = false
			self.PressUp.enabled = false
		end
	end
	self.Interactable = self.Interactable or false
	if self.SelectorGroup ~= nil then
		self.Selector.group = self.SelectorGroup
	end
	if self.Interactable then
		-- self.Selector.interactable = true
		self.Selector.enabled = true
		self.IconClick.enabled = true
	end
	self.Dark = self.Dark or false
	--[[ if self.Dark then
		UH.SpriteName(self.Frame, UIWItemCell.FrameName[3])
	end ]]
	--[[ if not StringIsEmpty(self.CellSize) then
		self.size:Set(tonumber(self.CellSize), tonumber(self.CellSize), 1)
	end
	self.Frame.gameObject:SetLocalScale(self.size) ]]
	self.QuaAnimTool:SetObjActive(false)
end

function UIWItemCell:getClickDel()
	if self.on_click == nil then
		if StringIsEmpty(self.ClickFunc) then
			self.on_click = false
		else
			if self.ClickEventTab == nil then
				self.on_click = CellClicks[self.ClickFunc]
			else
				local tab_func = self.ClickEventTab[self.ClickFunc]
				self.on_click = tab_func and BindTool.Bind(tab_func,self.ClickEventTab) or CellClicks[self.ClickFunc]
			end
		end
	end
	return self.on_click
end

function UIWItemCell:getSelectDel()
	if self.on_seleted == nil then
		if StringIsEmpty(self.SelectFunc) then
			self.on_seleted = false
		else
			if self.SelectEventTab == nil then
				self.on_seleted = CellSelects[self.SelectFunc]
			else			
				self.on_seleted = BindTool.Bind(self.SelectEventTab[self.SelectFunc],self.SelectEventTab)
			end
		end
	end
	return self.on_seleted
end

function UIWItemCell:Click()
	-- self:OnClick()
	self.IconClick:Trigger()
end

function UIWItemCell:SetSelected(selected)
	self.Selector.isOn = selected
end

function UIWItemCell:IsSelected()
	return self.Selector.isOn
end

function UIWItemCell:OnSelect(selected)
	local sel_func = self:getSelectDel()
	if sel_func then
		sel_func(self,selected)
	end
	UIWidgetBaseItem.OnSelect(self,selected)
end

function UIWItemCell:OnClick()
	local click_func = self:getClickDel()
	ItemInfoData.Instance:SetClickDataPos(UH.GetRootPosition(self.gameObject))
	if click_func then
		click_func(self)
	end
end

--data == Item.New
function UIWItemCell:SetData(data)
	self.data = data
	self:ClearCare()
	self:ClearFlush()
	
	CellFlushs.ReadyItem(self)
	if not StringIsEmpty(self.FlushFunc) and nil ~= self.data then
		CellFlushs[self.FlushFunc](self)
	end
	self.size:Set(1, 1, 1)
	if not StringIsEmpty(self.CellSize) then
		self.size:Set(tonumber(self.CellSize), tonumber(self.CellSize), 1)
	end
	self.Frame.gameObject:SetLocalScale(self.size)
end

function UIWItemCell:SetDataByItemId(item_id)
	local item_data = Item.Create({item_id = 0})
	item_data.item_id = item_id
	self:SetData(item_data)
end

function UIWItemCell:SetDataByItemIdWithNeed(item_id, need_num)
	local item_data = Item.Create({item_id})
	item_data.item_id = item_id
	self.need_num = need_num
	self.FlushFunc = "NeedFlush"
	self:SetData(item_data)
end

function UIWItemCell:SetDataByItemIdWithZeroNum(item_id, num)
	local num = num or Item.GetNum(item_id)
	local item_data = Item.New()
	item_data.item_id = item_id
	item_data.num = tostring(num)
	self:SetData(item_data)
end
function UIWItemCell:SetDataByItemIdWithoutZeroNum(item_id)
	local num = Item.GetNum(item_id)
	local item_data = Item.New()
	item_data.item_id = item_id
	item_data.num = num == 0 and "" or tostring(num)
	self:SetData(item_data)
end

function UIWItemCell:ClearFlush()
	if self.is_grey then self:MakeGrey(false) end
	for ch_name,used in pairs(self.used_chs) do
		if used then
			type_clear_flush_funcs[dynamic_ch_names[ch_name]](self.dynamic_chs[ch_name])
		end
	end
	self.used_chs = {}
	if self:IsSelected() == true then
		local sel_func = self:getSelectDel()
		if sel_func then
			sel_func(self,true)
		end	
	end
	
	self.QuaAnimTool:SetObjActive(false)
end
function UIWItemCell:ClearCare()
	TimeHelper:CancelTimer(self.press_runner)
	if self.handle_care then
		BattleData.Instance:BaseData():Uncare(self.handle_care)
		self.handle_care = nil
	end
end
function UIWItemCell:GetData()
	return self.data
end
function UIWItemCell:SetInteractable(flag)
	if self.Interactable ~= flag then
		self.Interactable = flag
		-- self.Selector.interactable = flag
		self.Selector.enabled = flag
		self.IconClick.enabled = flag
	end
end
function UIWItemCell:CloseLongClick()
	self.IsLongClick = false
	if self.PressDown and self.PressUp then
		self.PressDown.enabled = false
		self.PressUp.enabled = false
	end
end

function UIWItemCell:OnSpawn()
	self:Awake()
end

function UIWItemCell:MakeGrey(value)
	self.is_grey = value
	self.Interactor.Interactable = not value
end
function UIWItemCell:NoneMask()
	UH.SpriteName(self.Icon, "none")
end
function UIWItemCell:OnRelease()
	self:ClearCare()
	if self.Interactable then
		self:SetInteractable(false)
		self.Selector.isOn = false
	end
	self:CloseLongClick()
	self.Dark = false
	self.Selector.group = nil
	self.on_click = nil
	self.on_seleted = nil
	self.ClickEventTab = nil
	self.ClickFunc = nil
	self.SelectEventTab = nil
	self.SelectFunc = nil
	self.CellSize = nil
	self.WithUpgrade = nil
	self.ForceLevelView = nil
	self.need_num = nil
	self.WithRefine = nil
	
	if self.GodEffectRed and self.GodEffectRed.activeSelf then 
		self.GodEffectRed:SetActive(false) 
		self.qua_eff = nil
	end 
	if self.GodEffectOrange and self.GodEffectOrange.activeSelf then 
		self.GodEffectOrange:SetActive(false) 
		self.qua_eff = nil
	end 

	if self.GoldEffect and self.GoldEffect.activeSelf then 
		self.GoldEffect:SetActive(false) 
		self.qua_eff = nil
	end 

	if self.BlackEffect and self.BlackEffect.activeSelf then 
		self.BlackEffect:SetActive(false) 
		self.qua_eff = nil
	end 


	UH.SpriteName(self.Icon,nil)
	-- self:ClearFlush()

	UIWidgetBaseItem.OnRelease(self)
end
--同类不同预设的使用方法
function UIWItemCell:SupplyMask()
	self.Mask:SetObjActive(false)
	if self.ValueText then
		self.ValueText.text = ""
	end
	if self.LimitObj then
		self.LimitObj:SetActive(false)
	end
	if self.SupplyName then
		UH.SetText(self.SupplyName, Item.GetName(self.data.item_id))
		self.SupplyName:SetObjActive(true)
	end
	--修改为次数限制
	local use_type = Item.GetUseType(self.data.item_id)
	if use_type == ItemUseType.MPMedicine or use_type == ItemUseType.MPMedicineRate then
		self.handle_care = BattleData.Instance:BaseData():Care(BindTool.Bind(self.MpItemCheck, self), "mp_item_can_use_tiems")
		if self.ValueText then
			local p_str = self.data:Config().param2
			if use_type == ItemUseType.MPMedicineRate then
				p_str = p_str .. "%"
			end
			local str = string.format(Language.Common.Xstr, COLORSTR.Blue11, p_str)
			UH.SetText(self.ValueText, str)
		end
		self:MpItemCheck()
		self:SupplyLimit()
	end
	if use_type == ItemUseType.HPMedicine or use_type == ItemUseType.HPMedicineRate then
		if BattleData.Instance:PetUseItemInBattle(self.data) == false then
			--主角操作使用了药品
			UH.SetIconToImage(self.Mask, self.data:IconId(), ICON_TYPE.ITEM)
			UH.SetText(self.MaskText, ErrorText[103])
			self.Mask:SetObjActive(true)
			self.Mask.transform:SetAsLastSibling()
		end
		if self.ValueText and self.data.item_id ~= CommonItemId.QingXinDan then
			local p_str = self.data:Config().param2
			if use_type == ItemUseType.HPMedicineRate then
				p_str = p_str .. "%"
			end
			self.ValueText.text = string.format(Language.Common.Xstr, COLORSTR.Red12, p_str)
		end
		self:SupplyLimit()
	end
	-- LogError(BattleData.Instance:CurCmdIdx(),BattleData.Instance:PlayerIdx(),self.data.item_id)
	--判断 battle_mode=40 降妖鞭
	if self.data.item_id == CommonItemId.XiangYaoBian and (BattleData.Instance:BaseData().monster_group_id ~= 4000
	or BattleData.Instance:CurCmdIdx() ~= BattleData.Instance:PlayerIdx()) then
		UH.SetIconToImage(self.Mask, self.data:IconId(), ICON_TYPE.ITEM)
		UH.SetText(self.MaskText, ErrorText[103])
		self.Mask:SetObjActive(true)
		self.Mask.transform:SetAsLastSibling()
	end
	--宠物封印卡
	if PetSealCard[self.data.item_id] then
		local catch_type = BattleData.Instance:checkPetSeal()
		if  PetSealCard[self.data.item_id] < (catch_type - 1) then
			--判断当前最新品阶宝宝
			UH.SetIconToImage(self.Mask, self.data:IconId(), ICON_TYPE.ITEM)
			UH.SetText(self.MaskText, ErrorText[116])
			self.Mask:SetObjActive(true)
			self.Mask.transform:SetAsLastSibling()
		end
	end
end
function UIWItemCell:SupplyLimit()
	if self.LimitText and self.LimitObj then
		if RoleData.Instance:GetRoleLevel() < self.data:LimitLv() then
			self:MakeGrey(true)
			UH.SetText(self.LimitText, string.format(Language.Common.LimitUse, self.data:LimitLv()))
			self.LimitObj:SetActive(true)
			self.Mask.transform:SetAsLastSibling()
			self.LimitObj.transform:SetAsLastSibling()
		else
			self:MakeGrey(false)
			self.LimitObj:SetActive(false)
		end
	end
	if self.ValueText then
		self.ValueText.transform:SetAsLastSibling()
	end
end
function UIWItemCell:MpItemCheck()
	local base = BattleData.Instance:BaseData()
	if base.mp_item_can_use_tiems <= 0 and self.data:IconId() > 0 then
		UH.SetIconToImage(self.Mask, self.data:IconId(), ICON_TYPE.ITEM)
		UH.SetText(self.MaskText, ErrorText[103])
		self.Mask:SetObjActive(true)
		self.Mask.transform:SetAsLastSibling()
	else
		self.Mask:SetObjActive(false)
	end
end

function UIWItemCell:OnClickMask()
	if Item.GetUseType(self.data.item_id) == ItemUseType.MPMedicine then
		local base = BattleData.Instance:BaseData()
		if base.mp_item_can_use_tiems <= 0 then
			if base.is_pvp then
				PublicPopupCtrl.Instance:Center(ErrorText[142])
			else
				local limit = Config.fight_other_auto.other[1].pve_mana_potions_limit
				PublicPopupCtrl.Instance:Center(string.format(ErrorText[173], limit))
			end
		else
			PublicPopupCtrl.Instance:Center(ErrorText[103])
		end
	else
		PublicPopupCtrl.Instance:Center(ErrorText[103])
	end
end

function UIWItemCell:OnPressDown()
	-- self.click_time = os.time()
	TimeHelper:CancelTimer(self.press_runner)
	self.press_runner = TimeHelper:AddRunTimer(function ()
		CellClicks.BagGridNoButton(self)
    end, 1, nil, false)
end
function UIWItemCell:OnPressUp()
	TimeHelper:CancelTimer(self.press_runner)
end

local meta_tab = getmetatable(UIWItemCell)
local t_index = meta_tab.__index
meta_tab.__index = 
function(mono_inst,key)
	local reval = nil
	local com_type = dynamic_ch_names[key]
	if com_type == nil then
		return t_index(mono_inst,key)
	else
		reval = mono_inst.dynamic_chs[key]
		if reval== nil then
			local clone_ch_obj = cell_full[key]
			local cell_obj = clone_ch_obj:Clone(mono_inst.gameObject,false)
			
			local full_sibling_idx = clone_ch_obj:GetSiblingIndex()
			local truely_sibling_idx = 3
			for k,idx in pairs(mono_inst.dynamic_sibling_cache) do
				if idx < full_sibling_idx then
					local comp_idx = mono_inst.dynamic_chs[k].gameObject:GetSiblingIndex()
					if truely_sibling_idx <= comp_idx then
						truely_sibling_idx = comp_idx + 1
					end
				end
			end
			cell_obj:SetSiblingIndex(truely_sibling_idx)
			if com_type == typeof(UnityGameObj) then	--defulat gameobject
				reval = cell_obj
			else
				reval = cell_obj:GetComponent(com_type)
			end
			mono_inst.dynamic_sibling_cache[key] = full_sibling_idx
			mono_inst.dynamic_chs[key] = reval
		end
		mono_inst.used_chs[key] = true
	end		
	return reval
end	

--公用物品道具item 可带名字
-----Name========Text
-----Call========UIWItemCell 默认配置用ItemCellDefault
-----IsQuality========Bool是否QuaName
-----IsShallow========Bool是否Shallow
----------------------------CommonItem----------------------------
CommonItem = CommonItem or DeclareMono("CommonItem", UIWidgetBaseItem)

function CommonItem:SetData(data)
    local item = nil
    if data.Name then
        item = data
    else
        item = Item.Create(data.item_data or data)
    end
    self.Call:SetData(item)

    if self.Name then
        if self.IsQuality then
            UH.SetText(self.Name,item:QuaName(self.IsShallow))
        else
            UH.SetText(self.Name,item:Name())
        end
    end
    UIWidgetBaseItem.SetData(self, data)
end