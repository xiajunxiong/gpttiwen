
ClothAromaData = ClothAromaData or BaseClass()

function ClothAromaData:__init()
    if ClothAromaData.Instance ~= nil then
        Debuger.LogError("[ClothAromaData] attempt to create singleton twice!")
        return
    end
    ClothAromaData.Instance = self

    self.config = Config.jinyitianxiang_auto

    self.reward_info = SmartData.New({flush = false})
end

function ClothAromaData:SetInfo(protocol)

    if self.reward_info.has_draw_value then
        local add_num = protocol.has_draw_value - self.reward_info.has_draw_value
        if add_num > 0 then
            local num_str = ColorStr(add_num, COLORSTR.Yellow6)
            PublicPopupCtrl.Instance:Center(string.format(Language.ClothAroma.ItemTip, num_str))
        end
    end
    self.reward_info.draw_times = protocol.draw_times               -- 当前抽奖次数
    self.reward_info.get_reward_flag = protocol.get_reward_flag
    self.reward_info.has_draw_value = protocol.has_draw_value       -- 可抽奖的积分
    self.reward_info.flush = not self.reward_info.flush
end

function ClothAromaData:ClearData()
    self.reward_info:Set({flush = false})
end

-- 显示标记
function ClothAromaData:Mark()
    return self.config.other[1].mark
end

-- 获取抽取列表
function ClothAromaData:GetShowList()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG,self.config.reward,"seq")
    local list = {}
    local t_mark = self:Mark()
    for i,v in pairs(cfg) do
        if v.mark == t_mark then
            table.insert(list, v)
        end
    end
    return list
end

function ClothAromaData:GetFixShowList()
    local nomal_list = self:GetShowList()
    local mod_list = self:GetModShowList()

    local result_list = {}
    for k,v in pairs(nomal_list) do 
        local vo = {}
        vo.seq = v.seq
        vo.item = Item.Create(v.reward_item)
        vo.cfg = v
        for key, value in pairs(mod_list) do 
            if value.item.item_id == v.reward_item.item_id then 
                vo.mod = value
            end 
        end 
        table.insert(result_list,vo)
    end 

    table.sort(result_list, DataSorter.KeyLowerSorter("seq"))
    return result_list
end

-- 获取状态列表
function ClothAromaData:GetCellsStatus()
    local list = {}
    for i = 1,9 do 
        local vo = {}
        vo.got = self.reward_info.get_reward_flag[i-1] == 1

        table.insert(list,vo)
    end 

    return list
end

-- 获取演出标记表
function ClothAromaData:GetCellsMarkStatus()
    local list = {}
    for i = 1,9 do 
        local vo = {}
        vo.got = self.mark_list[i-1] == 1

        table.insert(list,vo)
    end 

    return list
end

function ClothAromaData:MarkTarget()
    self.mark_list = self.reward_info.get_reward_flag or {}
end

function ClothAromaData:CheckTarget()
    for k,v in pairs(self.reward_info.get_reward_flag) do 
        if self.mark_list[k] ~= v then 
            local num = k+1
            return num
        end 
    end 

    
    return 3
end

function ClothAromaData:ClearMark()
    self.mark_list = nil
end

-- 剩下的数量
function ClothAromaData:LastNum()
    local status = self:GetCellsStatus()
    local result = 0
    for k,v in pairs(status) do 
        if v.got == false then result = result + 1 end 
    end 

    return result
end

function ClothAromaData:GetItemNum()
    --return Item.GetNum(Config.jinyitianxiang_auto.other[1].reward_item.item_id)
    return self.reward_info.has_draw_value or 0
end

function ClothAromaData:GetDrawItemNum()
    -- local max = #Config.jinyitianxiang_auto.other
    -- local draw_times = self.reward_info.draw_times == nil and 1 or ((self.reward_info.draw_times + 1) > max and max or self.reward_info.draw_times + 1)
    -- return Config.jinyitianxiang_auto.other[draw_times].reward_item.num

    local cfg = self:GetPlayCfg()
    return cfg.draw_consume
end

function ClothAromaData:DrawComplete()
    local max = #Config.jinyitianxiang_auto.other
    return self.reward_info.draw_times == max
end

function ClothAromaData:GetDrawCfg()
    -- local max = #Config.jinyitianxiang_auto.other
    -- local draw_times = self.reward_info.draw_times == nil and 1 or ((self.reward_info.draw_times + 1) > max and max or self.reward_info.draw_times + 1)
    -- return Config.jinyitianxiang_auto.other[draw_times]

    return self:GetBuyCfg()
end

function ClothAromaData:GetItemEnough()
    local num = self:GetItemNum()
    local need_num = self:GetDrawItemNum()
    return num >= need_num
end

function ClothAromaData:IsMarking()
    return self.mark_list ~= nil 
end

-- type ==1 obj, type==2 role
function ClothAromaData:GetModShowList()
    local cur_list = self:GetShowList()
    local mod_list = {} 
    for k,v in pairs(cur_list) do 
        if v.show1 > 0 then 
            local vo = {}
            local item_cfg = Item.Init(v.reward_item.item_id):Config()
            vo.item = Item.Init(v.reward_item.item_id)
            -- vo.appearance = {}
            if item_cfg.item_type == ItemType.Fashion then
                local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
                if surface_cfg and surface_cfg.res_id ~= 0 then
                    if surface_cfg.type == IllusionConfig.FashionType.mount then
                        vo.res = DrawerHelper.GetRidePath(surface_cfg.res_id)
                        vo.type = 1
                        vo.res_info = Cfg.SurfaceInfoByActiveStuffId(v.reward_item.item_id)
                        vo.ill_show = IllusionConfig.FashionType.mount
                    else
                        vo.type = 2
                        vo.appearance = TableCopy(RoleData.Instance:GetApearance())
                        vo.res_info = Cfg.SurfaceInfoByActiveStuffId(v.reward_item.item_id)
                        if surface_cfg.type == IllusionConfig.FashionType.cloth then

                            vo.appearance.color = 0
                            vo.appearance.fashion_body_id = surface_cfg.res_id
                            vo.appearance.special_appearance_type = 0
                            vo.appearance.special_appearance_param = 0
                            vo.ill_show = IllusionConfig.FashionType.cloth
                        elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
                            vo.appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
                            vo.appearance.fashion_weapon_id = surface_cfg.res_id
                            vo.ill_show = IllusionConfig.FashionType.weapon
                        end 
                    end 
                elseif DEBUG_MODE then
                    LogError()
                end
            elseif item_cfg.item_type == ItemType.Skin then
                -- 角色、宠物、伙伴皮肤需要带上模型
                local skin_cfg = nil
                if item_cfg.show_type == ShowType.PetSkin then
                    skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
                    if DEBUG_MODE and skin_cfg == nil then
                        LogError("宠物皮肤物品",item_cfg.id,"的皮肤配置为空")
                    else 
                        vo.res_info = Cfg.SkinPetInfoByActiveStuffId(v.reward_item.item_id)
                        vo.ill_show = IllusionConfig.FashionType.pet
                    end
                elseif item_cfg.show_type == ShowType.PartnerSkin then
                    skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
                    if DEBUG_MODE and skin_cfg == nil then
                        LogError("伙伴皮肤物品",item_cfg.id,"的皮肤配置为空")
                    else 
                        vo.res_info = Cfg.SkinPartnerInfoByActiveStuffId(v.reward_item.item_id)
                        vo.ill_show = IllusionConfig.FashionType.partner
                    end
                end
                if skin_cfg and skin_cfg.res_id then
                    vo.type = 1
                    vo.res = DrawerHelper.GetNpcPath(skin_cfg.res_id)
                end
            elseif item_cfg.item_type == ItemType.PetWeapon then
                local weapon_config = Cfg.PetWeaponBaseCfg(item_cfg.id)
                if weapon_config then
                    vo.type = 1
                    vo.res = DrawerHelper.GetWeaponPath(weapon_config.pet_weapon_res)
                    vo.res_info = nil
                end
            elseif item_cfg.show_type == ShowType.Pet then
                local pet_cfg = PetData.Instance:GetPetCfg(item_cfg.id)
                if pet_cfg and pet_cfg.res_id then
                    vo.type = 1
                    vo.res = DrawerHelper.GetNpcPath(pet_cfg.res_id)
                    vo.res_info = nil
                end
            end 
            -- vo.res_id = 
            if vo.type ~= nil then 
                table.insert(mod_list,vo)
            end 
        end 
    end 

    return mod_list 
end


-- 抽奖配置
function ClothAromaData:GetPlayCfg()
    local play_times = self.reward_info.draw_times or 0
    for i,v in pairs(self.config.draw) do
        if v.draw == play_times + 1 then
            return v
        end
    end
    local len = #self.config.draw
    return self.config.draw[len]
end

-- 购买配置
function ClothAromaData:GetBuyCfg()
    local play_cfg = self:GetPlayCfg()
    local seq = play_cfg.seq
    local cfg = self.config.buy[seq + 1]
    return cfg
end

function ClothAromaData:StuffItemId()
    return self.config.other[1].reward_item.item_id
end