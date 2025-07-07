VIEW_REQ("modules/serveractivity/rand_activity/tian_yi_yao_shi/tian_yi_yao_shi_view")

TianYiYaoShiCtrl = TianYiYaoShiCtrl or BaseClass(BaseCtrl)

TianYiYaoShiCtrl.OpType = {
    Info = 0,       -- 获取活动信息
    PlayOne = 1,    -- 单抽
    PlayTen = 2,    -- 十连抽
    Buy = 3,        -- 购买道具,seq
}

function TianYiYaoShiCtrl:__init()
	if TianYiYaoShiCtrl.Instance ~= nil then
		Debuger.LogError("[TianYiYaoShiCtrl] attempt to create singleton twice!")
		return
	end
	TianYiYaoShiCtrl.Instance = self
    self.data = TianYiYaoShiData.New()
    self:RegisterAllProtocols()
    Remind.Instance:Register(Mod.RandActivity.TianYiYaoShi, self.data.remind_data, function ()
        return self.data:GetRemindNum() 
    end)
    BagData.Instance:ListenItems({self.data:ItemId()}, function ()
        self.data:NoticeRemind()
    end)
end

function TianYiYaoShiCtrl:__delete()
    self:UnRegisterAllProtocols()
    TianYiYaoShiCtrl.Instance = nil
end

function TianYiYaoShiCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end


function TianYiYaoShiCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRATianYiYaoShiInfo, "OnSCRATianYiYaoShiInfo")
end


function TianYiYaoShiCtrl:OnSCRATianYiYaoShiInfo(protocol)
    self.data:SetInfoData(protocol)
end




TianYiYaoShiData = TianYiYaoShiData or BaseClass()

function TianYiYaoShiData:__init()
    if TianYiYaoShiData.Instance ~= nil then
        Debuger.LogError("[TianYiYaoShiData] attempt to create singleton twice!")
        return
    end
    TianYiYaoShiData.Instance = self
    self.info_data = SmartData.New()
    self.remind_data = SmartData.New({first_open_flag = false})
    self.cfg = Config.tianyiyaoshi_auto
    self.act_type = ACTIVITY_TYPE.RAND_TIAN_YI_YAO_SHI
    self.jump_animation = false
end

function TianYiYaoShiData:ClearData()
end

function TianYiYaoShiData:SetInfoData(protocol)
    self.info_data:Set(protocol.info)
    self:NoticeRemind()
end

-- 消耗道具
function TianYiYaoShiData:ItemId()
    return self.cfg.other[1].consume_item
end

function TianYiYaoShiData:GetShopCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.shop_configuration)
    local list = TableCopy(cfg)
    table.sort(list, function (a, b)
        local a_num = a.seq
        local b_num = b.seq
        if not self:IsCanBuy(a) then
            a_num = a_num + 100
        end
        if not self:IsCanBuy(b) then
            b_num = b_num + 100
        end
        return a_num < b_num
    end)
    return list
end

function TianYiYaoShiData:GetShopBuyTimes(seq)
    local list = self.info_data.buy_times_list or {}
    return list[seq] or 0
end

-- 商品是否可以购买
function TianYiYaoShiData:IsCanBuy(data)
    local times = self:GetShopBuyTimes(data.seq)
    return times < data.buy_times
end


function TianYiYaoShiData:GetBaseCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.basis_configuration)
    return cfg
end

function TianYiYaoShiData:GetLogList()
    return self.info_data.log_list or {}
end


function TianYiYaoShiData:NoticeRemind()
    self.remind_data:SetDirty("first_open_flag")
end

function TianYiYaoShiData:GetRemindNum()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
    if not self.remind_data.first_open_flag then
        return 1
    end
    local item_id = self:ItemId()
    local num = Item.GetNum(item_id)
    if num > 0 then
        return 1
    end
    return 0
end

-- 获取物品的模型资源信息,没有返回 空
function TianYiYaoShiData:GetModelInfoByItemId(item_id)
    local item_cfg = Item.GetConfig(item_id)
    local info = {}
    info.show_type = nil        -- 1显示普通模型 2显示角色模型
    info.data = nil
    info.name = item_cfg.name
    info.png = nil              -- 原画路径
    info.item_id = item_id


    local get_path_param = {
        avatar_type = RoleData.Instance:GetRoleAvatarType(), 
        prof_type = RoleData.Instance:ProfType(),
    }

    if item_cfg.item_type == ItemType.Fashion then
        local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
            if surface_cfg.type == IllusionConfig.FashionType.mount then
                -- 坐骑单独显示
                info.show_type = 1
                info.data = DrawerHelper.GetRidePath(surface_cfg.res_id)
                get_path_param.type = IllusionConfig.FashionType.mount
            else
                -- 武器、羽翼、角色时装
                local appearance = TableCopy(RoleData.Instance:GetApearance())
                if surface_cfg.type == IllusionConfig.FashionType.cloth then
                    appearance.color = 0
                    appearance.fashion_body_id = surface_cfg.res_id
                    appearance.special_appearance_type = 0
                    appearance.special_appearance_param = 0

                    get_path_param.type = IllusionConfig.FashionType.cloth
                elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
                    appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
                    appearance.fashion_weapon_id = surface_cfg.res_id
                    get_path_param.type = IllusionConfig.FashionType.weapon
                end
                info.show_type = 2
                info.data = appearance
            end
            local res_info = Cfg.SurfaceInfoByActiveStuffId(item_id)
            get_path_param.id = res_info.id
            info.png = IllusionData.Instance:GetShowPngPath(get_path_param)
        elseif DEBUG_MODE then
            LogError("幻化配置为空 id：", item_cfg.param1, "物品id：", item_cfg.id, "配置：", item_cfg)
        end
    elseif item_cfg.item_type == ItemType.Skin then
        -- 角色、宠物、伙伴皮肤需要带上模型
        local skin_cfg = nil
        if item_cfg.show_type == ShowType.PetSkin then
            skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
            if DEBUG_MODE and skin_cfg == nil then
				LogError("宠物皮肤物品",item_cfg.id,"的皮肤配置为空")
            end
            if skin_cfg then
                info.png = string.format("pet_skin/%s.png",skin_cfg.res_id)
            end
        elseif item_cfg.show_type == ShowType.PartnerSkin then
            skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
            if DEBUG_MODE and skin_cfg == nil then
				LogError("伙伴皮肤物品",item_cfg.id,"的皮肤配置为空")
            end
            if skin_cfg then
                info.png = string.format(PartnerConfig.PngPath,skin_cfg.res_id)
            end
        end
        if skin_cfg and skin_cfg.res_id then
            info.show_type = 1
            info.data = DrawerHelper.GetNpcPath(skin_cfg.res_id)
        end
	elseif item_cfg.item_type == ItemType.PetWeapon then
		local weapon_config = Cfg.PetWeaponBaseCfg(item_cfg.id)
		if weapon_config then
            info.show_type = 1
            info.data = DrawerHelper.GetWeaponPath(weapon_config.pet_weapon_res)
		end
	elseif item_cfg.show_type == ShowType.Pet then
		local pet_cfg = PetData.Instance:GetPetCfg(item_cfg.id)
		if pet_cfg and pet_cfg.res_id then
            info.show_type = 1
            info.data = DrawerHelper.GetNpcPath(pet_cfg.res_id)
		end
    end
    
    if info.show_type then
        return info
    end
end

-- 是否是重点道具
function TianYiYaoShiData:IsHearsay(item_id)
    local cfg  = self:GetBaseCfg()
    for i,v in pairs(cfg) do
        if v.reward_item.item_id == item_id and v.hearsay == 1 then
            return true
        end
    end
    return false
end