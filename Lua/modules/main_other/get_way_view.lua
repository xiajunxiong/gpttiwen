GetWayView = GetWayView or DeclareView("GetWayView", "main_other/get_way")

VIEW_DECLARE_MASK(GetWayView,ViewMask.BgBlockClose)

function GetWayView:LoadCallback(data)
    local item = data.item or Item.New(data.item_data)
    self.Call:SetData(item)
    self.item = item
    UH.SetText(self.Name,item:QuaName())
    if item:IsPet() then
        UH.SetText(self.Content,"")
        self:ShowPetGetWay(item.item_id)
    else
        UH.SetText(self.Content,Format(Language.Mall.Content,item:Desc()))
		local type_dec = item:TypeDesc()
		local show_info = ""
		if type_dec then
			if type(type_dec) == [[table]] then 
				show_info = type_dec[1]
			else
				show_info = Language.Mall.SellType .. type_dec
			end
		end
        UH.SetText(self.Type, show_info)
        UH.SetText(self.Level, item:LevelDesc())
        UH.SetText(self.Level, item:LevelDesc())
        self.Functions:SetData(GetWayData.GetWayList(item))
    end
    self.PetList:SetActive(item:IsPet())
    UH.AnchoredPosition(self.TipsRect, Vector2.New(-5, item:IsPet() and 130 or -10))
    ItemInfoCtrl.Instance:Data():SetItemInfoData(item)
    self.funcs = data.func and data.func() or {}
    self.operatelist.gameObject:SetActive(#self.funcs > 0)
    if #self.funcs > 0 then 
        self.operatelist:SetData(self.funcs)
    end
    self:FlushDebugMode()

    GuideManager.Instance:RegisterGetGuideUi("GetWayGotoMY", function()	
		return self:GetGotoBtn("216")	--216是前往梦渊谷
	end)

    GuideManager.Instance:RegisterGetGuideUi("GetWayGotoSH", function()	
		return self:GetGotoBtn("149")	--149商会购买
	end)
    
end

function GetWayView:FlushDebugMode()
	for i=1,self.GMBtnList:Length() do
		self.GMBtnList[i]:SetActive(DEBUG_MODE == true or GmData.Instance.view_data.get_item == true)
	end
end

function GetWayView:OnClickAdd(param)
    GMCmdCtrl.Instance:SendGMCommand("additem", self.item.item_id .. " " .. param .. " 0")
end

function GetWayView:OnFlush()
    -- self.operatelist.gameObject:SetActive(nil ~= next(self.funcs or {})and nil == ItemInfoCtrl.Instance:Data().derive_view)
    -- self.blank.gameObject:SetActive(not (nil ~= next(self.funcs or {})and nil == ItemInfoCtrl.Instance:Data().derive_view))
end

function GetWayView:OnClickClose()
    if nil ~= ItemInfoCtrl.Instance:Data().derive_view then
		ItemInfoCtrl.Instance:ItemDeriveView()
    else 
        ViewMgr:CloseView(GetWayView)
        BagData.Instance:ClearGetWaySweepMark()
	end
end

function GetWayView:OnClickBlank()
	if nil ~= ItemInfoCtrl.Instance:Data().derive_view then
		ItemInfoCtrl.Instance:ItemDeriveView()
	end
end

function GetWayView:CloseCallback()
    ItemInfoCtrl.Instance:Data():SetItemInfoData()
end

function GetWayView:ShowPetGetWay(pet_id)
    local pet_cfg = PetData.Instance:GetPetCfg(pet_id)
    local way_cfg = PetData.Instance:GetCatchWay(pet_cfg.catch_way_str)
    UH.SetText(self.Type, Language.Pet.QualityTitle .. Language.Pet.QualityName[pet_cfg.quality])
    if PetData.Instance:GetTrade(pet_id) == 1 then
        table.insert(way_cfg,{catch_way = Language.Pet.Trade, jump_type = PetData.CatchWayJumpType.JumpTrade, catch_link = pet_id})
    end
    self.PetFunctions:SetData(way_cfg)
end

function GetWayView:GetGotoBtn(gw_data)
    local item = self.Functions:GuideVertical(function (data) return data == gw_data end,2)
    if item ~= nil then
        return item.Goto
    end
end

---------------------------------------------------------------------
GetWayViewItem = DeclareMono("GetWayViewItem", UIWidgetBaseItem)
function GetWayViewItem:GetWayViewItem()
	self.info = ItemInfoCtrl.Instance:Data():ItemInfo()
    self.sp_func = nil
end 

function GetWayViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemInter.Interactable = true
    self.LockObj:SetActive(false)
    if [[string]] == type(data) then
		local info = BagData.Instance:GetWayCfg(data)
        self.way_cfg = info
        UH.SetText(self.Name,info.desc)
		if StringIsEmpty(info.npc) and not StringIsEmpty(info.open_panel) then
            local plus_open_func = GetWayPlusFunOpen[info.open_panel] or GetWayPlusFunOpen[ModToModule(info.open_panel == "" and 0 or info.open_panel)]
			if not FunOpen.Instance:GetFunIsOpened(info.open_panel) or (plus_open_func~=nil and plus_open_func(info,self:GetItem()))  then
				self.ItemInter.Interactable = false
				self.LockObj:SetActive(true)
                self.TimerObj:SetActive(false)
                UH.SetText(self.GotoText,Language.Common.GoTo)
            else 
                self:FlushDetail()
			end
        elseif not StringIsEmpty(info.npc) then 
            UH.SetText(self.GotoText,Language.Common.GoTo)
            self.TimerObj:SetActive(false)
            local scene_id = tonumber(info.npc) ~= nil and VoMgr:NpcTargetByNpcId(tonumber(info.npc)) or 0
			if scene_id ~= 0 then
				local scene_show_cfg = MapData.Instance:GetMapShowData(scene_id)
				if scene_show_cfg then
					local min_level = MapData.Instance:GetMapMinLevel(scene_show_cfg.restrict_in)
					if min_level and RoleData.Instance:GetRoleLevel() < min_level then
						self.ItemInter.Interactable = false
						self.LockObj:SetActive(true)
					end
				end
			end
        elseif StringIsEmpty(info.open_panel) and StringIsEmpty(info.npc) then
            self.TimerObj:SetActive(false)
            self.Goto:SetActive(false)
		end
	else
        if self.info.item_list == nil or self.info.item_list[1] == nil then
            LogError("某个道具下的获取途径不存在",data)
            return
        end
		self.item = self.info.item_list[1]
		local config = ItemInfoConfig.ButtonConfig[data.type]
        local ItemButtonName = Language.ItemInfo.ItemButtonName
        UH.SetText(self.Name,ItemButtonName[self.data.name])
		if self.ButtonSp then
            UH.Color(self.Name,config.LbColor)
            UH.SpriteName(self.ButtonSp,config.SpName)
		end
		if self.item.param.remain_times then
			local is_open = self.item.param.is_turn_on == 1
            UH.SetText(self.Name,is_open and ItemButtonName.Close or ItemButtonName.Open)
            UH.Color(self.Name,is_open and ItemInfoConfig.ButtonConfig[2].LbColor or ItemInfoConfig.ButtonConfig[1].LbColor)
            UH.SpriteName(self.ButtonSp,is_open and ItemInfoConfig.ButtonConfig[2].SpName or ItemInfoConfig.ButtonConfig[1].SpName)
		end
	end
end 

function GetWayViewItem:OnDestroy()
    self:uncareData()
end

function GetWayViewItem:FlushDetail()
    local cfg = GetWayFBFlush[self.way_cfg.open_panel]
    if cfg and not cfg.Errorfunc(self.way_cfg) then 
        local view_func = function()
        -- btn_style 
        local param_btn = cfg.BtnStyleFunc(self.way_cfg)
        UH.SpriteName(self.GotoPic,param_btn.type == 1 and "AnNiu_1" or "AnNiu_3" )
        UH.Color(self.GotoText,param_btn.type == 1 and COLORS.Red1 or COLORS.Green1)
        UH.SetText(self.GotoText, param_btn.text)
        
        -- Timer
        local param_timer = cfg.TimerFunc(self.way_cfg)
        self.TimerObj:SetActive(param_timer.is_act)
        UH.SetText(self.Timer,param_timer.text)

        -- LockFunc
        local Inter_param = cfg.InterFunc(self.way_cfg)
        self.ItemInter.Interactable = Inter_param 

        -- SP Func
        self.sp_func = cfg.BtnSpClickFunc
        end 

        if cfg.care_data then 
            self:careData(cfg.care_data(), BindTool.Bind(view_func, self))
        else 
            view_func()
        end 
    else
        UH.SetText(self.GotoText,Language.Common.GoTo)
        self.Goto:SetActive(true)
        self.TimerObj:SetActive(false)
    end 
end 

function GetWayViewItem:OnClick()
    if self.way_cfg then
        local item = self:GetItem()
        if self.sp_func == nil or (self.sp_func ~= nil and self.sp_func(self.way_cfg,item)) then
		    GetWayData.OpenPanel(self.way_cfg,item)
        end 
	else
        ItemInfoViewFuncItem[self.data.name](self)
	end
end

function GetWayViewItem:GetItem()
    local item = self.info.item_list and self.info.item_list[1]
    if not item then
        return {}
    end
    -- 宠物口粮id特殊转换
    if PetExpItemMarketData[item.item_id or 0] then
        local vo = item.vo
        vo.item_id = PetExpItemMarketData[item.item_id]
        item = Item.Create(vo)
    end
    return item
end

GetWayViewPetItem = GetWayViewPetItem or DeclareMono("GetWayViewPetItem", UIWidgetBaseItem)

function GetWayViewPetItem:SetData(data)
    UH.SetText(self.Name, data.catch_way)
    self.Goto:SetActive(data.jump_type ~= PetData.CatchWayJumpType.JumpActivity)
    UIWidgetBaseItem.SetData(self, data)
end

function GetWayViewPetItem:OnClick()
    if self.data.jump_type == PetData.CatchWayJumpType.JumpScene then
        if self.data.catch_link ~= "" then
            local map_cfg = MapData.Instance:GetMapShowData(self.data.catch_link)
            local limit_level = MapData.Instance:GetMapMinLevel(map_cfg.restrict_in)
            if limit_level <= RoleData.Instance:GetRoleLevel() then
                -- 跳转场景后，打开自动抓宠设置界面
                --组队不生效
                if TeamData.Instance:InTeam() then
                    PublicPopupCtrl.Instance:Center(Language.Fight.Seal.TeamTip)
                    return
                end
                ViewMgr:CloseView(PetView)
                SceneLogic.Instance:AutoToPos(self.data.catch_link, nil, nil, nil, true)
                ViewMgr:OpenView(FightAutoSealSetView,{["scene_id"] = self.data.catch_link})
            else
                PublicPopupCtrl.Instance:Center(limit_level..Language.Pet.CatchMapUnlockLevel)
                return
            end
        end
    elseif self.data.jump_type == PetData.CatchWayJumpType.JumpShop then
        MallCtrl.Instance:Jump(self.data.catch_link)
    elseif self.data.jump_type == PetData.CatchWayJumpType.JumpView then
        ViewMgr:OpenViewByKey(self.data.catch_link)
    elseif self.data.jump_type == PetData.CatchWayJumpType.JumpNpc then
        SceneLogic.Instance:AutoToNpc(self.data.catch_link,nil,false,true)
        ViewMgr:CloseView(PetView)
    elseif self.data.jump_type == PetData.CatchWayJumpType.JumpTrade then
        TradeData.Instance:SetSearchItemId(self.data.catch_link)
        ViewMgr:OpenViewByKey(Mod.Mall.Trade)
    else
        LogError("宠物获取途径跳转类型 ",self.data.jump_type," 不存在")
    end
    ViewMgr:CloseView(GetWayView)
end