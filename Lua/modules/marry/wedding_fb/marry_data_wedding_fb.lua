
function MarryData:MarryDataWeddingFB()
    self.config_game_des = self.config.game_des
    self.config_heat_item = self.config.heat_item
    self.config_wedding_party = self.config.wedding_party
    self.config_heat_reward = self.config.heat_reward
    self.config_wedding_party_order = self.config.wedding_party_order
    self.config_float_parade = self.config.float_parade
    self.config_flowerball_way = self.config.flowerball_way
    self.config_other = self.config.other[1]
    

    self.wedding_scene_info = SmartData.New{init = true}
    self.wedding_role_info = SmartData.New{init = true}
    self.wedding_status_info = SmartData.New{init = true}

    self.marry_order_info = SmartData.New{init = true}
    self.marry_invite_info = SmartData.New{
        info = SmartData.New{init = true, hosts = {}},
        invite_list = SmartData.New{},
        apply_list = SmartData.New{},
    }

    self.marry_order_sm = SmartData.New{
        type = SmartData.New{init = true},
        info = SmartData.New(),
    }

    self.wedding_game_sm = SmartData.New{}
    self.wedding_bless_sm = SmartData.New{}
    self.marry_car_sm = SmartData.New{}

    self.wedding_fb_bwxq_sm = SmartData.New()

    self.wedding_fb_zsgz_sm = SmartData.New({fetch_flag = 0})

    self.marry_invite_sm = SmartData.New{
        type = SmartData.New{val = 1},
        info = SmartData.New()
    }


    self.wedding_fb_event = SmartData.New{type = MarryConfig.FBEventType.none}

    self.bwxq_monsters_pos = {}
    for _, v in pairs(self.config_flowerball_way) do
        local pos_b = string.split(v.birth_xyz, "|")
        local pos_1 = string.split(v.xyz1, "|")
        local pos_d = string.split(v.destination_xyz, "|")
        self.bwxq_monsters_pos[v.npc_id] = {
            [0] = {x = tonumber(pos_b[1]), y = tonumber(pos_b[2])},
            [1] = {x = tonumber(pos_1[1]), y = tonumber(pos_1[2])},
            [2] = {x = tonumber(pos_d[1]), y = tonumber(pos_d[2])},
            [3] = {x = tonumber(pos_1[1]), y = tonumber(pos_1[2])},
        }
    end

    self.wedding_heat_reward_str_show = {}

    Cfg.Register("GetMarriedHeatItemCoByIndex",Cfg.RealTimeSearch(Config.get_married_auto.heat_item,"shop_item_index"))
end


-- 数据清理、监听
function MarryData:ClearWeddingFBScene()
    self.wedding_fb_event.type = MarryConfig.FBEventType.none
end

function MarryData:CareWeddingFBEventChange(func)
    return self.wedding_fb_event:Care(func)
end




-- 配置
function MarryData:GetMarryCarShowList()
    return self.config_float_parade
end

function MarryData:ConfigOtherSceneId()
    return self.config_other.scene_id
end

function MarryData:ConfigOtherInviteNumber()
    return self.config_other.invite_number
end

function MarryData:ConfigOtherAddNumber()
    return self.config_other.add_number
end

function MarryData:ConfigOtherMoneyType()
    return self.config_other.money_type
end

function MarryData:ConfigOtherAddConsume()
    return self.config_other.add_consume
end

function MarryData:ConfigOtherAddTimes()
    return self.config_other.add_times
end

function MarryData:ConfigOtherEatTimeSpace()
    return self.config_other.eat_time_space
end

function MarryData:ConfigOtherEatTimesMax()
    return self.config_other.eat_times_max
end

function MarryData:ConfigOtherPartakeLevelLimit()
    return self.config_other.partake_level_limit
end




-- 协议部分
function MarryData:SetWeddingFBSceneInfo(protocol)
    -- ylog("SetWeddingFBSceneInfo", protocol)
    local info = protocol.info
    local scene_info = self.wedding_scene_info
    if scene_info.init or (info.host_uid1 ~= scene_info.host_uid1 or info.host_uid2 ~= scene_info.host_uid2) then
        local vos = {}
        table.insert(vos, SceneCtrl.Instance:Objs():GetCharaVoByUid(info.host_uid1) or nil)
        table.insert(vos, SceneCtrl.Instance:Objs():GetCharaVoByUid(info.host_uid2) or nil)
        for _, vo in ipairs(vos) do
            vo = CommonStruct.NoWeaponNoRideWeddingVo(vo)
        end
    end
    self.wedding_scene_info:Set(info)
    if info.link_seq ~= self.wedding_fb_event.type then
        self.wedding_fb_event.type = info.link_seq
    end
end

function MarryData:SetWeddingFBRoleInfo(protocol)
    -- ylog("SetWeddingFBRoleInfo", protocol)
    local info = protocol.common_param
    self.wedding_role_info:Set(info)
end

function MarryData:SetWeddingFBSync(protocol)
    if MarryConfig.FBSyncType.bless_shop == protocol.sync_type and 0 == UnityPlayerPrefs.GetInt(PrefKeys.WeddingFBBlessEffect()) then
        SocietyData.Instance:PlayGiftEffect(2, protocol.param)
    elseif MarryConfig.FBSyncType.heat == protocol.sync_type then
        self.wedding_scene_info.wedding_heat = protocol.param
    end
end

function MarryData:SetWeddingRegistList(protocol)
    -- ylog("SetWeddingRegistList", protocol)
    self.marry_order_info:Set(protocol.info)
end

function MarryData:SetWeddingInviteList(protocol)
    -- ylog("SetWeddingInviteList", protocol)
    local info = self.marry_invite_info.info
    protocol.info.count = 1 == protocol.is_total and protocol.count or ((info.count or 0) + protocol.count)
    self.marry_invite_info.info:Set(protocol.info)
    if 1 == protocol.is_total then
        self.marry_invite_info.invite_list:Set(protocol.invite_list)
    else
        for uid, role in pairs(protocol.invite_list) do
            self.marry_invite_info.invite_list[uid] = role
        end
    end
end

function MarryData:SetWeddingApplyList(protocol)
    -- ylog("SetWeddingApplyList", protocol)
    if 1 == protocol.is_total then
        self.marry_invite_info.apply_list:Set(protocol.apply_list)
    else
        for uid, role in pairs(protocol.apply_list) do
            self.marry_invite_info.apply_list[uid] = role
        end
    end
end

function MarryData:SetWeddingChange(protocol)
    -- ylog("SetWeddingChange", protocol)
    if MarryConfig.WeddingChangeType.regist_add == protocol.change_type then
        local info = self.marry_order_info
        info.regist_time_list[protocol.param_1] = true
    elseif MarryConfig.WeddingChangeType.regist_remove == protocol.change_type then
        local info = self.marry_order_info
        info.regist_time_list[protocol.param_1] = false
    elseif MarryConfig.WeddingChangeType.role_regist_change == protocol.change_type then
        local info = self.marry_order_info
        info.regist_time = protocol.param_1
    end
end

function MarryData:SetWeddingStatusSync(protocol)
    -- ylog("SetWeddingStatusSync", protocol)
    local info = protocol.info
    info.finish_time = info.begin_time + 1500
    self.wedding_status_info:Set(protocol.info)
end

function MarryData:SetWeddingShowInfo(protocol)
    -- ylog("SetWeddingShowInfo", protocol)
    local info = protocol.info
    local ft = os.date("*t", info.begin_time)
    local language = Language.Marry.WeddingShowInfo
    local can_go = info.hosts[RoleData.Instance:GetRoleId()] or MarryConfig.WeddingShowType.invited == info.status
    ViewMgr:OpenView(MarryProposeView, {
        propose_type = MarryConfig.PROPOSE_TYPE.WEDDING,
        role_info = info.host_list,
        date_desc = string.format(language.DateDesc, ft.month, ft.day, ft.hour, ft.min),
        btn_name = can_go and language.BtnShow.go or language.BtnShow.apply,
        confirm_func = can_go and function ()
            ViewMgr:CloseView(MarryProposeView)
            if not LoginData.Instance:IsOnCrossSever() then
                PublicPopupCtrl.Instance:DialogTips{
                    content = Language.Marry.WeddingCrossTips,
                    confirm = {func = function()
                        ViewMgr:CloseView(DialogTipsView)
                        LoginCtrl.Instance:SendCrossStartReq()
                    end}
                }
            else
                if TeamData.Instance:InTeam() then
                    PublicPopupCtrl.Instance:DialogTips{
                        content = Language.Marry.WeddingTeamTips,
                        confirm = {func = function()
                            TeamCtrl.Instance:SendExitTeam()
                            ViewMgr:CloseView(DialogTipsView)
                            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.WEDDING_FB, info.wedding_id)
                        end}
                    }
                else
                    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.WEDDING_FB, info.wedding_id)
                end
            end
        end or function ()
            ViewMgr:CloseView(MarryProposeView)
            if not LoginData.Instance:IsOnCrossSever() then
                PublicPopupCtrl.Instance:DialogTips{
                    content = Language.Marry.WeddingCrossTips,
                    confirm = {func = function()
                        ViewMgr:CloseView(DialogTipsView)
                        LoginCtrl.Instance:SendCrossStartReq()
                    end}
                }
            else
                MarryCtrl.Instance:SendWeddingFBReqApply(info.wedding_id)
            end
        end
    })
end

function MarryData:SetWeddingSubActBaoWeiXiuQiu(protocol)
    -- ylog("SetWeddingSubActBaoWeiXiuQiu", protocol)
    self.wedding_fb_bwxq_sm:Set(protocol.obj_info)
end

function MarryData:SetWeddingSubActZaoShengGuiZi(protocol)
    -- ylog("SetWeddingSubActBaoWeiXiuQiu", protocol)
    self.wedding_fb_zsgz_sm.fetch_flag = protocol.fetch_flag
end

-- 小游戏说明
function MarryData:GetWeddingFBGameList()
    if nil == self.wedding_fb_game_show_list then
        self.wedding_fb_game_show_list = {}
        for _, v in ipairs(self.config_game_des) do
            table.insert(self.wedding_fb_game_show_list, v)
        end
    end
    return self.wedding_fb_game_show_list
end

function MarryData:GetWeddingFBGamePrepareTime()
    if nil == self.wedding_game_prepare_time then
        self.wedding_game_prepare_time = 0
        for _, v in ipairs(self.config_wedding_party) do
            if v.wedding_list < MarryConfig.FBParseType.sub_act then
                self.wedding_game_prepare_time = self.wedding_game_prepare_time + v.last_time
            end
        end
    end
    return self.wedding_game_prepare_time
end




-- 小游戏 保卫绣球
function MarryData:GetBaoWeiXiuQiuMonsterList()
    return self.config_flowerball_way
end

function MarryData:GetBaoWeiXiuQiuMonsterPos(npc_id)
    return self.bwxq_monsters_pos[npc_id]
end

function MarryData:GetBaoWeiXiuQiuMonsterShow(npc_id)
    return MarryConfig.FBEventType.act_bwxq == self.wedding_fb_event.type and self.wedding_fb_bwxq_sm[npc_id]
end




-- 副本菜单
function MarryData:GetWeddingFBMenuRewardShow()
    local scene_info = self.wedding_scene_info
    local role_info = self.wedding_role_info
    if scene_info.init or role_info.init then return false end
    for _, v in ipairs(self.config_heat_reward) do
        if scene_info.wedding_heat >= v.heat_request and 0 == (bit:_rshift(role_info[MarryConfig.FBCommonOperType.heat_reward] or 0, v.heat_type) % 2) then
            return true
        end
    end
    return false
end

function MarryData:GetWeddingFBMenuCandyShow()
    local scene_info = self.wedding_scene_info
    local role_info = self.wedding_role_info
    if scene_info.init or role_info.init then return false end
    return scene_info.link_seq >= MarryConfig.FBEventType.warm and 0 == role_info[MarryConfig.FBCommonOperType.candies]
end




-- 祝福
function MarryData:GetWeddingFBBlessShowList()
    if nil == self.wedding_fb_bless_show_list then
        self.wedding_fb_bless_show_list = {}
        for _, v in ipairs(self.config_heat_item) do
            v.item = Item.Init(v.item_id)
            table.insert(self.wedding_fb_bless_show_list, v)
        end
    end
    return self.wedding_fb_bless_show_list
end




-- 热度奖励
function MarryData:GetWeddingFBHeatRewardMax()
    if nil == self.wedding_heat_reward_max then
        self.wedding_heat_reward_max = 0
        for _, v in ipairs(self.config_heat_reward) do
            self.wedding_heat_reward_max = math.max(self.wedding_heat_reward_max, v.heat_request)
        end
    end
    return self.wedding_heat_reward_max
end

function MarryData:GetWeddingFBHeatRewardStrShow(mark)
    if nil == self.wedding_heat_reward_str_show[mark] then
        local str_show = ""
        for _, v in ipairs(self.config_heat_reward) do
            str_show = str_show .. v.heat_request .. mark
        end
        str_show = string.sub(str_show, 1, - string.len(mark) - 1)
        self.wedding_heat_reward_str_show[mark] = str_show
    end
    return self.wedding_heat_reward_str_show[mark]
end




-- 婚宴预约
function MarryData:GetMarryOrderTypeList()
    local server_time = TimeHelper.GetServerTime()
    local list = {}
    for i = 1, 3 do
        list[i] = {
            index = i,
            month = tonumber(os.date("%m", server_time)),
            day = tonumber(os.date("%d", server_time)),
            start = TimeHelper.DayStartAny(server_time),
        }
        server_time = server_time + 86400
    end
    return list
end

function MarryData:GetMarryOrderShowList()
    if nil == self.marry_order_show_list then
        self.marry_order_show_list = {}
        for _, v in ipairs(self.config_wedding_party_order) do
            table.insert(self.marry_order_show_list, v)
        end
    end
    return self.marry_order_show_list
end

function MarryData:GetMarryOrderRewardList()
    if nil == self.marry_order_reward_show then
        self.marry_order_reward_show = {}
        for i = 0, #self.config_other.reward do
            local reward_data = self.config_other.reward[i]
            table.insert(self.marry_order_reward_show, Item.Init(reward_data.item_id, reward_data.num, reward_data.is_bind))
        end
    end
    return self.marry_order_reward_show
end

function MarryData:GetMarryOrderIsRegist(open_time_show)
    if self.marry_order_info.init or self.marry_order_sm.type.init then return false end
    local regist_time_list = self.marry_order_info.regist_time_list
    local type = self.marry_order_sm.type
    return regist_time_list[type.start + open_time_show]
end

function MarryData:GetMarryOrderOutDate(open_time_show)
    if self.marry_order_sm.type.init then return false end
    local type = self.marry_order_sm.type
    local start_time = type.start + open_time_show
    return TimeHelper.GetServerTime() >= start_time
end




-- 婚宴剧情
function MarryData:IsPlayStory()
    local role_info = self.wedding_role_info
    return not role_info.init and 1 == role_info[MarryConfig.FBCommonOperType.rite]
end

function MarryData:GetMarryWeddingAvatars()
    local scene_info = self.wedding_scene_info
    if scene_info.init then return false end
    local xl = scene_info.host_avatar_type_xl or 0
    local xn = scene_info.host_avatar_type_xn or 0
    xl = math.min(xl, 6)
    xl = math.max(xl, 1)
    xn = math.min(xn, 6)
    xn = math.max(xn, 1)
    return true, {
        xl,
        xn,
    }
end

function MarryData.AvatarTypeToResId(avatar_type)
    if 0 == avatar_type then return 0 end
    return ResHelper.RoleResId(avatar_type, MarryConfig.FASHION_BODY_ID, 0)
end




-- 婚宴邀请
function MarryData:GetWeddingInviteList()
    return self.marry_invite_info.invite_list
end

function MarryData:GetWeddingInviteInfo()
    return self.marry_invite_info.info
end

function MarryData:GetWeddingApplyList()
    local list = {}
    local invite_list = MarryData.Instance:GetWeddingInviteList()
    for uid, info in pairs(self.marry_invite_info.apply_list) do
        if not invite_list[uid] then
            table.insert(list, info)
        end
    end
    return list
end

function MarryData:WeddingApplyRedPoint()
    local invite_list = MarryData.Instance:GetWeddingInviteList()
    for uid, info in pairs(self.marry_invite_info.apply_list) do
        if not invite_list[uid] then
            return true
        end
    end
    return false
end




-- 场景部分
function MarryData:GetWeddingIsHost()
    local scene_info = self.wedding_scene_info
    if scene_info.init then return false end
    return 1 == scene_info.is_host
end

function MarryData:GetWeddingIsHostByUid(uid)
    local scene_info = self.wedding_scene_info
    if scene_info.init then return false end
    return scene_info.host_uid1 == uid or scene_info.host_uid2 == uid
end

function MarryData:WeddingEatMeal()
    local scene_info = self.wedding_scene_info
    local role_info = self.wedding_role_info
    if scene_info.init or role_info.init then return end
    local language = Language.Marry.WeddingFBMenu
    local eat_time_space = self:ConfigOtherEatTimeSpace()
    if TimeHelper.GetServerTime() - role_info.meal_last_fetch_time <= eat_time_space then
        PublicPopupCtrl.Instance:Center(string.format(language.EatMenuTime, math.floor(eat_time_space - (TimeHelper.GetServerTime() - role_info.meal_last_fetch_time))))
    else
        if self:ConfigOtherEatTimesMax() == role_info[MarryConfig.FBCommonOperType.meal] then
            PublicPopupCtrl.Instance:Center(language.EatMenuFinish)
        else
            MarryCtrl.Instance:SendWeddingFBReqMeal()
        end
    end
end