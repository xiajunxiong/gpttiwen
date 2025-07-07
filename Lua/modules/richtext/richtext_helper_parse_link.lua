
-- Example: {openLink;200}
-- [200] = {
--     open_func,
--     colorStr or Green7;
-- }
local language_link = Language.RichText.LinkStr

--客户端自定义传闻链接点击配置 文字在language_link.OpenLink
OpenLinks = {
    ["200"] = {
        function()
            ViewMgr:OpenViewByKey(Mod.Bag.Main)
        end,
    },
    ["201"] = {
        function(others)
            TradeData.Instance:SetOpenLinkList(others)
            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
        end,
    },
    ["202"] = {
        function()
            ViewMgr:OpenViewByKey(Mod.Guild.GuildMember)
        end,
    },
    ["203"] = {
        function()
            ViewMgr:OpenViewByKey(Mod.Splendid.PetBefall)
        end,
    },
    ["204"] = {
        function()
            if SceneData.Instance:SceneId() == SceneLogic.Instance:MainCityID() then
                if not LoginData.Instance:IsOnCrossSever() then
                    LoginCtrl.Instance:SendCrossStartReq()
                end
            else
                SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(), nil, function()
                    if not LoginData.Instance:IsOnCrossSever() then
                        LoginCtrl.Instance:SendCrossStartReq()
                    end
                end, nil, true)
            end
        end,
    }
}

RTParseLink = {}

--客户端自定义传闻链接点击
function RTParseLink.openLink(param_t)
    local config = OpenLinks[param_t[1]]
    local str = ColorStr(language_link.OpenLink[tonumber(param_t[1])], config[2] or COLORSTR.Green7)
    return RichTextHelper.LinkStr(str, "_ul", config[2] or COLORSTR.Green7, 1, "openLink", 2, param_t[1])
end

function RTParseLink.r(param_t)
    return RichTextHelper.ColorStr(param_t[2], COLORSTR.Green7)
end

function RTParseLink.trademarket(param_t)
    TradeData.Instance:SetOpenLinkData(param_t)
    return Item.GetQuaName(tonumber(param_t[6] or 0))
end

function RTParseLink.exchange_constant(param_t)
    return Item.GetQuaName(tonumber(param_t[6] or 0)) .. Language.Mall.NoticeTip .. RichTextHelper.ColorStr(param_t[5].. Language.Common.HuoBi[0], COLORSTR.Yellow1)
end

function RTParseLink.mr_charm(param_t)
    return ""
end

function RTParseLink.world_boss_id(param_t)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1]))
    return npc_info.name
end

function RTParseLink.world_boss_2_id(param_t)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1]))
    return npc_info.name
end

function RTParseLink.world_boss_3_id(param_t)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1]))
    return npc_info.name
end

function RTParseLink.scene_id(param_t)
    local scene_info = SceneData.Instance:GetSceneCfg(tonumber(param_t[1]))
    return RichTextHelper.ColorStr(scene_info.name,COLORSTR.Yellow25)
end

function RTParseLink.scene_list(param_t)
    local name_show = ""
    local scene_list = Split(param_t[1],",")
    for i,v in ipairs(scene_list) do
        local scene_id = tonumber(v) or 0
        if scene_id ~= 0 then
            local config = SceneData.Instance:GetSceneCfg(scene_id)
            if config then
                name_show = name_show .. (name_show ~= "" and "、" or "") .. config.name
            end
        end
    end
    return RichTextHelper.ColorStr(name_show,COLORSTR.Yellow25)
end

function RTParseLink.recruit(param_t, others)
    local target_id = tonumber(param_t[2])
    local target = TeamData.Instance:GetTeamGoalWithId(target_id)
    if target == nil then return end
    local team_type = TeamData.Instance:GetTeamTypeWithType(target.team_type)
    local role_level = RoleData.Instance:GetRoleLevel()
    local target_name = string.gsub(target.desc, " ", RichTextHelper.ColorStr("】", COLORSTR.Transparent))
    if others.func then
        return others.func{
            type = RichTextType.Recruit,
            is_recruit = true;
            team_index = tonumber(param_t[1]),
            target_id = tonumber(param_t[2]),
            target_name = target_name,
            team_target = team_type.team_target,
            low_level = tonumber(param_t[3]),
            high_level = tonumber(param_t[4]),
            role_level = role_level,
            is_cross = 1 == tonumber(param_t[5]),
            num = tonumber(param_t[6]),
            num_max = TeamConfig.TEAM_MEMBER_MAX_T[target_id] or TeamConfig.TEAM_MEMBER_MAX,
            is_complete = TaskData.Instance:IsTeamTargetTaskComplete2(target, false),
            hide_main = true,
            other_desc = target.other_desc
        }
    end
end

function RTParseLink.group_invite(param_t, others)
    if others.func then
        local link_str = RichTextHelper.LinkStr(language_link.GroupIvite, "_ul", COLORSTR.Purple1, 1, "groupInvite", 2, param_t[2], 3, param_t[4], 4, param_t[1], 5, param_t[3])
        return others.func{
            type = RichTextType.GroupInvite,
            group_name = param_t[3],
            link_str = link_str,
        }
    end
end

function RTParseLink.TeamApply(team_index, is_cross, _ul)
    return RichTextHelper.LinkStr(language_link.TeamApply, "_ul", _ul, 1, "teamApply", 2, team_index, 3, is_cross and 1 or 0)
end

function RTParseLink.chat_quick_item(param_t, others)
    if others.func then
        return others.func(ChatItemMono.ItemParse(tonumber(param_t[1]), param_t[2]))
    end
end

function RTParseLink.web_link(param_t,others)
    local address = ""
    if others.func then others.func({mark = param_t[3],address = param_t[4]}) end 
    return RichTextHelper.LinkStr(param_t[1], "_ul", param_t[2], 1,"weblink",2 ,param_t[3], 3, others.parse_type)
end

function RTParseLink.chat_quick_pet(param_t, others)
    if others.func then
        return others.func(ChatItemPet.ItemParse(param_t))
    end
end

function RTParseLink.chat_quick_shengqi(param_t, others)
    if others.func then
        local t = others.func(ChatItemShengQi.ItemParse(param_t))
        return t
    end
end

function RTParseLink.chat_quick_gift(param_t, others)
    if others.func then
        return others.func(ChatItemGift.ItemParse(param_t))
    end
end

function RTParseLink.chat_quick_gift_flower(param_t, others)
    if others.func then
        return others.func(ChatItemGiftFlower.ItemParse(param_t))
    end
end

function RTParseLink.chat_quick_shitu_invite(param_t, others)
    if others.func then
        return others.func(ChatItemShiTuInvite.ItemParse(param_t))
    end
end

function RTParseLink.hongbao(param_t,others)
    if others.func then
        return others.func(ChatHongBaoItem.ItemParse(param_t))
    end
end

function RTParseLink.hongbao_get(param_t,others)
    if others.func then
        return others.func(ChatHongBaoGetItem.ItemParse(param_t))
    end
end

function RTParseLink.trademarket_notice(param_t)
    return RichTextHelper.LinkStr(ColorStr(language_link.OpenLink[201],COLORSTR.Green7), "_ul", COLORSTR.Green7, 1, "openLink", 2, "201", 3, param_t[1])
end

function RTParseLink.first_kill_name(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.Yellow7)
end

function RTParseLink.first_kill_boss(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.Red12)
end

function RTParseLink.first_kill_round(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.Yellow7)
end

function RTParseLink.hill_sea_notice(param_t)
    return RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.Common.ChallengeTip, COLORSTR.Green7), "_ul", COLORSTR.Green7,1,"hillSeaNotice", 2, param_t[1])
end

--随机活动文字解析 + 跳转
function RTParseLink.randactivity_notice(param_t)
    local name = ActivityRandData.GetName(tonumber(param_t[1]))--ActivityRandData.Instance:GetConfig(tonumber(param_t[1]))
    return RichTextHelper.LinkStr(RichTextHelper.ColorStr(name, COLORSTR.Green7), "_ul", COLORSTR.Green7,1,"randactivity", 2, param_t[1]) or ""
end

--新服活动文字解析 + 跳转
function RTParseLink.serveractivity_open(param_t)
    local config = ServerActivityData.Instance:GetOpenServerConfig(tonumber(param_t[1]))
    local str = RichTextHelper.ColorStr(config.name, COLORSTR.Green7)
    return RichTextHelper.LinkStr(str, "_ul", COLORSTR.Green7,1,"serveractivity", 2, param_t[1])
end

--系统模块名字解析 + 跳转
function RTParseLink.mod_key(param_t)
    local mod_key = tonumber(param_t[1])
    if language_link.AutoToNpc[mod_key] then
        return RichTextHelper.LinkStr(language_link.mod_key[mod_key], "_ul", COLORSTR.Green7,"autoToNpc",language_link.AutoToNpc[mod_key])
    else
        local str = language_link.mod_key[mod_key]
        return str and RichTextHelper.LinkStr(str, "_ul", COLORSTR.Green7,"modKey",param_t[1]) or ""
    end
end

-- 神兽恩泽传闻跳转
function RTParseLink.shenshouenze(param_t)
    local mod_key = Mod.Reward.GodBeastGrace
    local str = language_link.mod_key[mod_key]
    return RichTextHelper.LinkStr(str, "_ul", COLORSTR.Green7, "modKey", mod_key) or ""
end

function RTParseLink.consume_item(param_t)
    return string.format(language_link.consume_item, Item.GetQuaName(tonumber(param_t[1])), tonumber(param_t[2]))
end

-- 宠物唤灵，没用了
function RTParseLink.pet_quality(param_t)
    local quality = tonumber(param_t[1]) or 1
    return RichTextHelper.SpriteStrAtlas(PetData.PetLevel[quality], 45, 29)
end

-- 唤灵
function RTParseLink.xunbao(param_t)
    param_t[3] = tonumber(param_t[3])
    local item = Item.Init(param_t[3])
    local item_name = Item.GetQuaName(param_t[3])
    local show_type = item:ShowType()
    if show_type == ShowType.Pet then
        local pet_cfg = Cfg.PetById(param_t[3])
        if not TableIsEmpty(pet_cfg) then
            return Format(Language.TreasureHunt.TopShowTip, param_t[2], RichTextHelper.SpriteStrAtlas(PetData.PetLevel[pet_cfg.quality], 45, 29), item_name)
        end
    elseif show_type == ShowType.Partner then
        local quality = item:Color()
        return Format(Language.TreasureHunt.TopShowTip, param_t[2], Language.Partner.QuaLevel[quality], item_name)
    else
        return Format(Language.TreasureHunt.TopShowTip, param_t[2], "", item_name)
    end
end

function RTParseLink.person_rank(param_t)
    local rank_type = tonumber(param_t[1]) or 1
    return Language.Society.Rank.RankNames[SocietyConfig.RankTypeRoleToC[rank_type]]
end

function RTParseLink.link_item(param_t)
    local sign_id = string.format("%ss%s", param_t[1], param_t[2])
    local item_data = Item.Init(tonumber(param_t[3]))
    local show_text = ""
    if Equip.IsDivine(tonumber(param_t[3])) then 
        show_text = item_data:IsEquip() and RichTextHelper.ColorStr(string.format(Language.RichText.Format.GodEquipLinkDesc, item_data:Name(), 1), item_data:ColorStr()) or item_data:QuaName()
    else 
        show_text = item_data:IsEquip() and RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr()) or item_data:QuaName()
    end 
    return RichTextHelper.LinkStr(show_text, "ItemSignId", sign_id, "_ul", item_data:ColorStr())
end

function RTParseLink.god_link_item(param_t)
    local sign_id = string.format("%ss%s", param_t[2], param_t[3])
    local item_data = Item.Init(tonumber(param_t[4]))
    local show_text = ""
    if Equip.IsDivine(tonumber(param_t[4])) then 
        show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.GodEquipLinkDesc, item_data:Name(), 1), item_data:ColorStr())
    else 
        show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr())
    end 
    local link_item = RichTextHelper.LinkStr(show_text, "ItemSignId", sign_id, "_ul",  item_data:ColorStr())

    return string.format(Language.RichText.Format.GodEquipLinkDetailName,param_t[1])..link_item..Language.RichText.Format.GodEquipLinkDetailEnd
end

function RTParseLink.god_equip_break_through(param_t)
    local sign_id = string.format("%ss%s", param_t[3], param_t[4])
    local item_data = Item.Init(tonumber(param_t[2]))
    local show_text = ""
    if Equip.IsDivine(tonumber(param_t[2])) then 
        show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.GodEquipLinkDesc, item_data:Name(), param_t[5] or 1), item_data:ColorStr())
    else 
        show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr())
    end 
    local link_item = RichTextHelper.LinkStr(show_text, "ItemSignId", sign_id, "_ul",  item_data:ColorStr())

    return string.format(Language.RichText.Format.GodEquipBreakThroughLink,param_t[1],link_item)
end

function RTParseLink.link_pet(param_t)
    local sign_id = string.format("%ss%s", param_t[1], param_t[2])
    local item_data = Item.Init(tonumber(param_t[3]))
    local quality = tonumber(param_t[4] or 1)
    return RichTextHelper.LinkStr(item_data:QuaName(), "ItemSignId", sign_id, "_ul", item_data:ColorStr()) .. RichTextHelper.SpriteStrAtlas(PetData.PetLevel[quality], 45, 29)
end

function RTParseLink.medal_up_grade(param_t)
    local name = RichTextHelper.ColorStr(param_t[1], COLORSTR.Green7) 
    local medal_before = Item.Create({item_id = tonumber(param_t[2]),param ={grade = tonumber(param_t[3]),sp_id = tonumber(param_t[4])}})
    local infoHandle_1 = RichTextHelper.StoreInfo(medal_before)
    local name_1 = RichTextHelper.LinkStr(medal_before:QuaName(), "infoHandle", infoHandle_1, "_ul",  medal_before:ColorStr())

    local medal_after = Item.Create({item_id = tonumber(param_t[5]),param ={grade = tonumber(param_t[6]),sp_id = tonumber(param_t[7])}})
    local infoHandle_2 = RichTextHelper.StoreInfo(medal_after)
    local name_2 = RichTextHelper.LinkStr(medal_after:QuaName(), "infoHandle", infoHandle_2, "_ul",  medal_after:ColorStr())
    return string.format( Language.Medal.MedalUpgradeNotice,name,name_1,name_2)
end

function RTParseLink.medal_lottery(param_t)
    local name = RichTextHelper.ColorStr(param_t[2], COLORSTR.Green7) 
    local item = Item.Create({item_id = tonumber(param_t[3])})
    BagData.Instance:ChatItemShowPlus(item,tonumber(param_t[4]))
    local fornt_str = string.format( Language.Medal.MedalLotteryRedSpecial,name,item:QuaName())

    local color = COLORSTR.Green7
    local behind_str = RichTextHelper.LinkStr(Language.Medal.MedalLotteryLink, "modKey",14002,"_ul",  color)
    return fornt_str..behind_str
end

function RTParseLink.join_temp(param_t, others)
    local str = string.format(language_link.join_temp, param_t[1])
    local color = RTParseType.SocietyChat == others.parse_type and COLORSTR.Yellow14 or COLORSTR.Yellow1
    return string.gsub(str, "namecolor", color)
end

function RTParseLink.create_temp(param_t, others)
    local color = RTParseType.SocietyChat == others.parse_type and COLORSTR.Yellow14 or COLORSTR.Yellow1
    return string.gsub(language_link.create_temp, "namecolor", color)
end

function RTParseLink.become_official(param_t, others)
    local str = string.format(language_link.become_official, param_t[1], param_t[2])
    local color = RTParseType.SocietyChat == others.parse_type and COLORSTR.Yellow14 or COLORSTR.Yellow1
    str = string.gsub(str, "namecolor", color)
    if 0 == tonumber(param_t[4]) then
        local exit_group = string.gsub(language_link.exit_group, "namecolor", color)
        str = str .. RichTextHelper.LinkStr(exit_group, "groupExit", param_t[3], "_ul",  color)
    end
    return str
end

function RTParseLink.welcome_join_temp(param_t, others)
    local color = RTParseType.SocietyChat == others.parse_type and COLORSTR.Yellow14 or COLORSTR.Yellow1
    local str = string.format(language_link.welcome_join_temp, param_t[1])
    return string.gsub(str, "namecolor", color)
end

function RTParseLink.handover_leadership(param_t, others)
    local color = RTParseType.SocietyChat == others.parse_type and COLORSTR.Yellow14 or COLORSTR.Yellow1
    local str = string.format(language_link.handover_leadership, param_t[1], param_t[2])
    return string.gsub(str, "namecolor", color)
end

function RTParseLink.is_last_round_battle(param_t)
    local is_last_round_battle = tonumber(param_t[1])
    if is_last_round_battle == 1 then
        TopLayerMarqueeData.Instance.show_special_effect = true
    end
    local color = COLORSTR.Green7
    local id1 = tonumber(param_t[2])
    local id2 = tonumber(param_t[3])
    local name1 = GuildEmbattleData.Instance:GetGuildName(id1)
    local name2 = GuildEmbattleData.Instance:GetGuildName(id2)
    local str = string.format(Language.RichText.ShowStr.honorbattle, name1,name2)
    str = str .. RichTextHelper.LinkStr(language_link.ViewBattle, "honorviewbattle", param_t[2], "_ul",  color)
    return str
end

function RTParseLink.honor_namelist()
    local color = COLORSTR.Green7
    local str = RichTextHelper.LinkStr(language_link.NameList, "honornamelist", "0", "_ul",  color)
    return str
end

function RTParseLink.share_god_land_trace(param_t)
    local item = Item.Init(tonumber(param_t[3]))
    local str = string.format(language_link.share_god_land_trace, RichTextHelper.LinkStr(item:QuaName(), 1, "societyShare", 2, param_t[1], 3, param_t[2], 4, param_t[3], 5, param_t[4] or 0, "_ul", item:ColorStr()))
    return str
end

function RTParseLink.paimaihang_notice(param_t)
    ServerActivityData.Instance:OnAddPaiMaiHangNotice(param_t)
    return RichTextHelper.LinkStr(language_link.paimaihang_notice, "_ul", COLORSTR.Green7, 1, "paimaihangNotice", 2, param_t[1], 3, param_t[2])
end

function RTParseLink.duel_posi(param_t)
    return RichTextHelper.LinkStr(language_link.WatchBattle, "_ul", COLORSTR.Green7, 1, "duel_posi", 2, param_t[1], 3, param_t[2], 4, param_t[3])
end

function RTParseLink.change_line()
    return "\n"
end

function RTParseLink.format_sign()
    return "%s"
end

function RTParseLink.mail_survey(param_t)
    local type = tonumber(param_t[1])
    local color_str = param_t[2]
    local str = language_link.mail_survey[type]
    if str then
        str = RichTextHelper.ColorStr(str, color_str)
        str = RichTextHelper.LinkStr(str, "_ul", color_str, "mail_survey", type)
    end
    return str
end

function RTParseLink.currency_link(param_t)
    return FormatEnum.CurrencyType[tonumber(param_t[1])]
end

function RTParseLink.wedding_invite(param_t)
    local wedding_id = param_t[1]
    return RichTextHelper.LinkStr(language_link.wedding_invite, "_ul", COLORSTR.Red2, "wedding_invite", wedding_id)
end

function RTParseLink.peak_token(param_t)
    return RichTextHelper.LinkStr(Language.RichText.ShowStr.PeakToken, "_ul", COLORSTR.Green7, 1, 
    "peak_token", 2,param_t[1],3,param_t[2],4,param_t[3],5,param_t[4],6,param_t[5])
end

function RTParseLink.conceont_knot_invite(param_t)
    return RichTextHelper.LinkStr(Language.ConcentricKnot.Link, "_ul", COLORSTR.Purple1, 1, "conceont_knot_invite", 2, param_t[2], 3, param_t[3])
end

-- 家族任务·特殊字段·解析
function RTParseLink.guild_cooperate_task(param_t)
    local item = Item.Init(tonumber(param_t[1]))
    return RichTextHelper.LinkStr(item:QuaName(), "_ul", item:ColorStr(), 1,"guild_cooperate_task", 2,param_t[1])
end

-- 神域·特殊字段·解析
function RTParseLink.landsbetween_share_pos(param_t)
    local link_mark = param_t[1]
    local type = param_t[2] 
    local fix_link = LandsBetweenData.FixLink(link_mark)

    local pos_str = ColorStr(string.format(Language.LandsBetween.Desc.ShareContectStr,fix_link[1],fix_link[2]), COLORSTR.Green7)

    local pass_info = param_t[1].."|"..param_t[2]
    return RichTextHelper.LinkStr(pos_str, "_ul", COLORSTR.Green7, 1,"landsbetween_share_pos", 2,fix_link[1],3,fix_link[2],4,type)
end

function RTParseLink.godspacerank(param_t)
    return Language.ShenYuRank.rank_type_title[tonumber(param_t[2])] .. Language.ShenYuRank.target_type[tonumber(param_t[1])]
end

function RTParseLink.broadcast_drop_type(param_t)
    return Language.Common.BroadcastDropType[tonumber(param_t[1])]
end