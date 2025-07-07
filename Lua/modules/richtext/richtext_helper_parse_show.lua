
local language_show = Language.RichText.ShowStr

RTParseShow = {}

function RTParseShow.i(param_t)
    local item_id = tonumber(param_t[1])
    local is_shallow = tonumber(param_t[2]) or 0
    return Item.GetQuaName(item_id,is_shallow == 1)
end

function RTParseShow.r(param_t)
    return RichTextHelper.ColorStr(param_t[2], COLORSTR.Green7)
end

--颜色{c;颜色} 需要染色的染色的内容 {c;0}
function RTParseShow.c(param_t)
    if param_t[2] == nil then
        return tonumber(param_t[1]) and "</color>" or string.format("<color=#%s>",param_t[1])
    else
        RichTextHelper.ColorStr(param_t[2], param_t[1])
    end
end

function RTParseShow.trademarket(param_t)
    return Item.GetQuaName(tonumber(param_t[6] or 0))
end

function RTParseShow.exchange_constant(param_t)
    return Item.GetQuaName(tonumber(param_t[6] or 0)) .. Language.Mall.NoticeTip .. RichTextHelper.ColorStr(param_t[5].. Language.Common.HuoBi[0], COLORSTR.Yellow1)
end

function RTParseShow.wordsColor(param_t)
    return RichTextHelper.ColorStr(param_t[2], param_t[1])
end

function RTParseShow.gamename(param_t)
    return RichTextHelper.ColorStr(GLOBAL_CONFIG:GameName(),"bf242a")
end

--符咒类型
--param_t[1]=charm_id(符咒id)
function RTParseShow.mr_charm(param_t)
	local rune_id = tonumber(param_t[1]) or -1
	local re = MazeRunData.RuneName(rune_id)
	return re
end

function RTParseShow.world_boss_id(param_t, others)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1]))
    return RichTextHelper.ColorStr(npc_info.name, RTParseType.SocietyMail == others.parse_type and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.world_boss_2_id(param_t, others)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1])) or {}
    if npc_info.name == nil then
        LogError(Format("NPC: %s配置不在",param_t[1] or 0))
    end
    return RichTextHelper.ColorStr(npc_info.name or "", RTParseType.SocietyMail == others.parse_type and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.world_boss_3_id(param_t, others)
    local npc_info = NpcDialogCtrl.Instance:NpcInfo(tonumber(param_t[1])) or {}
    if npc_info.name == nil then
        LogError(Format("NPC: %s配置不在",param_t[1] or 0))
    end
    return RichTextHelper.ColorStr(npc_info.name or "", RTParseType.SocietyMail == others.parse_type and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.npc_id(param_t,others)
    local npc_id = tonumber(param_t[1]) or 0
    local config = Cfg.NpcById(npc_id) or Cfg.NpcBySeq or {}
    return RichTextHelper.ColorStr(config.name or "", RTParseType.SocietyMail == others.parse_type and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.scene_id(param_t, others)
    local scene_info = SceneData.Instance:GetSceneCfg(tonumber(param_t[1]))
    return RichTextHelper.ColorStr(scene_info.name, RTParseType.SocietyMail == others.parse_type and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.scene_list(param_t)
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

function RTParseShow.chat_type(param_t)
    local type = tonumber(param_t[1])
    return Language.Chat.ChatNames[type]
end

function RTParseShow.activitystate(param_t)
    local act_type = tonumber(param_t[1])
    local status = tonumber(param_t[2])
    local act_info = ActivityData.Instance:GetActivityInfo(act_type)
    return string.format(language_show.activitystate[status], nil ~= act_info and act_info.act_name or language_show.act_names[act_type] or act_type)
end

function RTParseShow.activity_id(param_t)
    local act_info = ActivityData.Instance:GetActivityInfo(tonumber(param_t[1]))
    return act_info and act_info.act_name or ""
end

function RTParseShow.base_prof(param_t)
    local param = tonumber(param_t[1])
    local prof_name = Language.Common.Prof[math.floor(param / 100)]
    return RichTextHelper.ColorStr(prof_name,COLORSTR.Green7)
end

function RTParseShow.chief_prof(param_t)
    local param = tonumber(param_t[1])
    local prof_name = Language.ChiefElection.ProfTitle .. Language.Common.Prof[math.floor(param / 100)]
    return RichTextHelper.ColorStr(prof_name,COLORSTR.Yellow25)
end

function RTParseShow.chief_election_login(param_t)
    TopLayerMarqueeData.Instance.show_special_effect = true
    local param = tonumber(param_t[1]) or 0
    local role_name = param_t[2] or ""
    local prof_name = Language.ChiefElection.ProfTitle .. Language.Common.Prof[math.floor(param / 100)]
    return RichTextHelper.ColorStr(prof_name,COLORSTR.Yellow25) .. Format("<color=#00e240>%s</color>",role_name)
end

function RTParseShow.openLink(param_t)
    return language_link.OpenLink[tonumber(param_t[1])]
end

function RTParseShow.peak_battle_round(param_t)
    local param = tonumber(param_t[1])
    local show = Language.ChiefPeakBattle.RichtextShow
    if param >= 4 then
        return RichTextHelper.ColorStr(show[4],COLORSTR.Green7)
    else
        return RichTextHelper.ColorStr(show[param],COLORSTR.Green7)
    end
end

function RTParseShow.platform_number(param_t)
    local seq = tonumber(param_t[1])
    local arena = ArenaMatchData.Instance:GetArenaBySeq(seq)
    if arena then return arena.leitai_name end
end

function RTParseShow.prestige_get(param_t)
    local id = tonumber(param_t[1])
    local value = tonumber(param_t[2])
    local config = Config.prestige_cfg_auto.village_list[id]
    local item = Item.Init(65561)
    if config then
        return string.format(language_show.prestige_get, config and RichTextHelper.ColorStr(config.name .. item:Name(), QualityColorStr[item:Color()]) or item:QuaName(), value)
    end
end

function RTParseShow.prestige_up(param_t)
    local id = tonumber(param_t[1])
    local level = tonumber(param_t[2])
    local config1 = Config.prestige_cfg_auto.village_list[id]
    local config2 = PrestigeData.Instance:GetPrestigeUpgradeData(level, id)
    local item = Item.Init(65561)
    if config1 and config2 then
        return string.format(language_show.prestige_up, config1 and RichTextHelper.ColorStr(config1.name .. item:Name(), QualityColorStr[item:Color()]) or item:QuaName(), RichTextHelper.ColorStr(config2.name, COLORSTR.Yellow18))
    end
end

function RTParseShow.item_not_enough_fb3(param_t)
    local item_id = tonumber(param_t[1])
    local item_cfg = Item.GetConfig(item_id)
    if item_cfg.color <= 3 then
        return string.format(language_show.item_not_enough_fb3, ColorStr(item_cfg.name,QualityColorStr[item_cfg.color]))
    else
        return string.format(language_show.item_not_enough_fb4, ColorStr(item_cfg.name,QualityColorStr[item_cfg.color]))
    end
end

function RTParseShow.item_not_enough_fb4(param_t)
    local item_id = tonumber(param_t[1])
    if FightData.Instance:GetSweepBuyTimes() - FightData.Instance.fight_crystal_sweep.buy_times > 0 then
        ViewMgr:OpenView(FightCrystalSweepItemView)
    end
    return string.format(language_show.item_not_enough_fb5, Item.GetQuaName(item_id))
end

function RTParseShow.item_not_enough_fb5(param_t)
    local item_id = tonumber(param_t[2])
    local name = param_t[1]
    return string.format(language_show.item_not_enough_fb6, name,Item.GetQuaName(item_id))
end

function RTParseShow.rank_level(param_t)
    local rank = tonumber(param_t[1])
    return rank or ""
end

function RTParseShow.achievement(param_t)
    local stage_id = tonumber(param_t[1])
    local star_id = tonumber(param_t[2])
    return AchieveNewData.GetStageName(stage_id,star_id)
end

function RTParseShow.collection(param_t)
    local id = tonumber(param_t[1]) + 1
    local id_name = IllustrateData.Instance:GetCollectionName(id) or ""
    return RichTextHelper.ColorStr(id_name,COLORSTR.Red5)
end

function RTParseShow.buytrademarket(param_t)
    local item_id = tonumber(param_t[1])
    local money_price = tonumber(param_t[3])
    return Format(Language.Mall.SllItemGetTip,Item.GetName(item_id),money_price)
end

function RTParseShow.adv_task(param_t)
    local task_id = tonumber(param_t[1])
    local co = Config.tasklist_auto.task_list[task_id] or Config.challengetask_list_auto.task_list[task_id] or Config.medaltask_list_auto.task_list[task_id] or
    Config.strange_tasklist_auto.task_list[task_id]
    return co and string.format(language_show.adv_task, Language.Task.TaskType[co.task_type], co.task_name) or ""
end

function RTParseShow.task_name(param_t)
    local task_id = tonumber(param_t[1])
    local co = Config.tasklist_auto.task_list[task_id] or Config.challengetask_list_auto.task_list[task_id] or Config.medaltask_list_auto.task_list[task_id] or
    Config.strange_tasklist_auto.task_list[task_id]
    return co and co.task_name or ""
end

function RTParseShow.on_consume(param_t)
    local ct = tonumber(param_t[1])
    return string.format(language_show.on_consume, param_t[2], Language.Common.HuoBi[ct] or "")
end

function RTParseShow.attr_type(param_t)
    local attr_type = tonumber(param_t[1])
    return Language.Common.AttrList[attr_type]
end

function RTParseShow.battle_add_exp_pet(param_t)
    local pet = Pet.New{id = tonumber(param_t[1])}
    return string.format(language_show.battle_add_exp_pet, pet:Name(), param_t[2])
end

function RTParseShow.trademarket_notice(param_t)
    return RichTextHelper.ColorStr(Language.RichText.LinkStr.OpenLink[201], COLORSTR.Green7)
end

function RTParseShow.first_kill_name(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.Yellow16)
end

function RTParseShow.first_kill_boss(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.White)
end

function RTParseShow.first_kill_round(param_t)
    return RichTextHelper.ColorStr(param_t[1], COLORSTR.White)
end

function RTParseShow.advancefb_notic(param_t)
    local num = tonumber(param_t[1]) or 0
    local sceneid = SceneData.Instance:SceneId()
    local adv_config = FightData.Instance:GetAdvancementsSceneData(sceneid)
    return Format(language_show.advance_fb,num,Item.GetQuaName(adv_config.item_id))
end

function RTParseShow.guild_honor_result(param_t)
    local type = tonumber(param_t[1]) or 0
    local result = language_show.guild_honor_result[type]
    return result
end

function RTParseShow.hill_sea_notice(param_t)
    return RichTextHelper.ColorStr(Language.Common.ChallengeTip,COLORSTR.Green7)
end

--随机活动文字解析 + 跳转
function RTParseShow.randactivity_notice(param_t)
    return RichTextHelper.ColorStr(ActivityRandData.GetName(tonumber(param_t[1])), COLORSTR.Green7)
end

--新服活动文字解析 + 跳转
function RTParseShow.serveractivity_open(param_t)
    local config = ServerActivityData.Instance:GetOpenServerConfig(tonumber(param_t[1]))
    return RichTextHelper.ColorStr(config.name, COLORSTR.Green7)
end

--系统模块名字解析 + 跳转
function RTParseShow.mod_key(param_t)
    local mod_key = tonumber(param_t[1]) or 0
    return language_show.mod_key[mod_key] or Language.RichText.ShowStr.mod_key[mod_key] or ""
end

-- 神兽恩泽传闻跳转
function RTParseShow.shenshouenze(param_t)
    local mod_key = Mod.Reward.GodBeastGrace
    return language_show.mod_key[mod_key] or ""
end

--随机活动 邮件显示的 仅显示
function RTParseShow.serveractivity_notice(param_t)
    return RichTextHelper.ColorStr(ActivityRandData.GetName(tonumber(param_t[1])), COLORSTR.Green9)
end

--限时活动 限时活动 解析活动名称 仅显示 is_shallow 是否深色底
function RTParseShow.act_type(param_t)
    local act_type,is_shallow = tonumber(param_t[1]),param_t[2]
    local name = ActivityRandData.GetName(act_type) or (ActivityData.GetConfig(act_type) or {}).name_text
    return ColorStr(name or "",is_shallow == nil and COLORSTR.Green9 or COLORSTR.Green7)
end

function RTParseShow.act_sign_up(param_t)
    local config = ActivityData.GetConfig(tonumber(param_t[1]))
    return RichTextHelper.ColorStr(config.name_text, COLORSTR.Green7)
end

function RTParseShow.consume_item(param_t)
    return string.format(language_show.consume_item, Item.GetQuaName(tonumber(param_t[1])), tonumber(param_t[2]))
end

function RTParseShow.huanjierank(param_t)
    local score = tonumber(param_t[1]) or 0
    local config = BattlefieldData.Instance:GetCurRankConfig(score)
    return config.ranking_list or ""
end

function RTParseShow.partner_id(param_t)
    local partner_id = tonumber(param_t[1]) or 0
    local config = Config.partner_cfg_auto.partner_info[partner_id]
    local partner_name = config and config.name or ""
    local quality = tonumber(param_t[2]) or 0
    if quality ~= nil then
        return ColorStr(partner_name,QualityColorStr[quality])
    end
    return partner_name
end

-- 宠物唤灵，没用了
function RTParseShow.pet_quality(param_t)
    local quality = tonumber(param_t[1]) or 1
    return Language.Pet.ColorLevelS[quality]
end

-- 唤灵
function RTParseShow.xunbao(param_t)
    param_t[3] = tonumber(param_t[3])
    local item = Item.Init(param_t[3])
    local item_name = Item.GetQuaName(param_t[3])
    local show_type = item:ShowType()
    if show_type == ShowType.Pet then
        local pet_cfg = Cfg.PetById(param_t[3])
        if not TableIsEmpty(pet_cfg) then
            return Format(Language.TreasureHunt.TopShowTip, param_t[2], Language.Pet.ColorLevelS[pet_cfg.quality], item_name)
        end
    elseif show_type == ShowType.Partner then
        local quality = item:Color()
        return Format(Language.TreasureHunt.TopShowTip, param_t[2], Language.Partner.QuaLevel[quality], item_name)
    else
        return Format(Language.TreasureHunt.TopShowTip, param_t[2], "", item_name)
    end
end

-- 跨服英雄录
function RTParseShow.kfyxl(param_t)
    return Language.KFHeroRecord.Title[tonumber(param_t[1])]
end

--声望村子
function RTParseShow.village_id(param_t)
    local village_id = tonumber(param_t[1]) or 1
    local config = PrestigeData.Instance:GetVillageList(village_id)
    return config and config.name or ""
end

function RTParseShow.partner_quality(param_t)
    local quality = tonumber(param_t[1]) or 0
    local segment = tonumber(param_t[2]) or 0
    local quality_name = Language.Partner.QuaLevel[quality]
    local segment_name = string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(segment))
    return quality_name and ColorStr(quality_name..segment_name,QualityColorStr[quality]) or ""
end

-- 伙伴唤灵，没用了
function RTParseShow.partner_quality_treasure(param_t)
    local quality = tonumber(param_t[1]) or 0
    local quality_name = Language.Partner.QuaLevel[quality]
    return quality_name and ColorStr(quality_name,QualityColorStr[quality]) or ""
end

function RTParseShow.guild_boss_chapter_id(params)
    local chapter_id = tonumber(params[1]) or 1
    local config = GuildData.Instance:ChapterConfig(chapter_id)
	if config and config[0] then
		return config[0].chapter_name
	end
	return ""
end

function RTParseShow.skin_id(params)
    local skin_co = PartnerData.Instance.partner_skin_ids[tonumber(params[1]) or 1] or {}
    local active_item = Item.Init(skin_co.active_stuff_id or 65532)
    return RichTextHelper.ColorStr(skin_co.name, active_item:ColorStr())
end

function RTParseShow.god_token_time_seq(params)
    local timestamp = params[1]
    local token_time = os.date("%Y/%m/%d",timestamp)
    local time_t = Split(token_time,"/")
    return DataHelper.GetWord(tonumber(time_t[2]))
end

function RTParseShow.chat_type(params)
    local channel_type = tonumber(params[1])
    return Language.Chat.ChatNames[channel_type or 0]
end

--解析奇闻名字
function RTParseShow.anecdote(params)
    local anecdote_type = tonumber(params[1])
    local config = AnecdoteData.GetConfig(anecdote_type)
    return config and ColorStr(config.name or "", COLORSTR.Green9) or ""
end

function RTParseShow.chief_election_result(param_t)
    local round = tonumber(param_t[1])
    local is_pass = tonumber(param_t[2])
    if round == 4 and is_pass == 0 then
        return Language.ChiefElection.MailResultTip[2]
    else
        return Format(Language.ChiefElection.MailResultTip[is_pass],Language.ChiefElection.FailTip[round] or "")
    end
end

function RTParseShow.god_equip_id(param_t)
    local item_id = tonumber(param_t[1])
    return Item.GetName(item_id) 
end

function RTParseShow.monster_group_id(param_t)
    local monster_group_id = tonumber(param_t[1])
    local config = Cfg.MonsterGroupById(monster_group_id)
    return config and config.name or ""
end
function RTParseShow.guild_honor_round(param_t)
    --人数--当前回合
    local num = tonumber(param_t[1])
    local round = tonumber(param_t[2])
    -- local calc_round = GuildEmbattleData.Instance:CalcRound(num, round)
    local stage = GuildEmbattleData.Instance:Stage()
    if round >= 4 then
        return Language.GuildEmbattle.RoundName[round]
    else
        return Language.GuildEmbattle.RoundName[stage]
    end
end
function RTParseShow.is_right(param_t)
    local is_right = tonumber(param_t[1])
    return Language.GuildEmbattle.GuessResult[is_right]
end

function RTParseShow.guild_honor_rank(param_t)
    local rank = tonumber(param_t[1])
    return string.format(Language.GuildEmbattle.RankNum, rank)
end

function RTParseShow.is_last_round_battle(param_t)
    local is_last_round_battle = tonumber(param_t[1])
    if is_last_round_battle == 1 then
        TopLayerMarqueeData.Instance.show_special_effect = true
    end
    local color = COLORSTR.Green7
    local id1 = tonumber(param_t[2])
    local id2 = tonumber(param_t[3])
    local name1 = GuildEmbattleData.Instance:GetGuildName(id1)
    local name2 = GuildEmbattleData.Instance:GetGuildName(id2)
    return string.format(Language.RichText.ShowStr.honorbattle, name1,name2)
end

function RTParseShow.guild_honor_winner_login(param_t)
    TopLayerMarqueeData.Instance.show_special_effect = true
    local rank_type = tonumber(param_t[1])
    local guild_name = param_t[2]
    local player_name = param_t[3]
    local re_val = string.format(Language.RichText.ShowStr.guild_honor_winner_login.str,
        Language.RichText.ShowStr.guild_honor_winner_login.rt[rank_type],guild_name,player_name)
    return re_val
end

function RTParseShow.honor_namelist()
    return ""
end

function RTParseShow.fabao_id(param_t)
    local fabao_id = tonumber(param_t[1])
    local data = FabaoData.Instance:FabaoAttrInfo(fabao_id)
    return data.name
end

function RTParseShow.first_recharge_day(param_t)
    local day = tonumber(param_t[1])
    return Format(Language.FirstCharge.FirstChargeDay, DataHelper.GetDaXie(day))
end

function RTParseShow.hmtj_fb_id(param_t)
    LogError("p1===",param_t[1])
    local cfg = Cfg.PromevalParadiseBossFbId(tonumber(param_t[1]))
    local fg_name = cfg and cfg.fb_name or "Unknow"
    local color_str = ""
    if cfg and cfg.fb_type >= 2 then
        fg_name = string.format("%s(%s)",fg_name,Language.HarmonyHeaven.Menu.TypeShow[cfg.fb_type])
        color_str = QualityColorStrShallow[ItemColor.Red] 
    else
        color_str =QualityColorStrShallow[ItemColor.Blue]
    end
    return RichTextHelper.ColorStr(fg_name,color_str)
end

function RTParseShow.server_competition_rank(param_t)
    local rank = tonumber(param_t[1])
    return Format(Language.CompetitionRank.RankChat,DataHelper.GetDaXie(rank))
end
function RTParseShow.server_competition(param_t)
    local type = tonumber(param_t[1])
    return Language.CompetitionRank.TabName2[type]
end

function RTParseShow.card_type(param_t)
    local card_type = tonumber(param_t[1])
    return Language.MonthCard.CardType[card_type] or ""
end

function RTParseShow.duel_posi(param_t)
    return ""
end

function RTParseShow.life_skill(param_t)
    local skill_type = tonumber(param_t[1]) or 0
    local cfg = LiveData.Instance:GetSkillTypeCfg(skill_type)
    return cfg.name or ""
end

--保留小数位数，{float_num;%f;%d} 第一个是数值,第二个是要显示多少个小数点
function RTParseShow.float_num(param_t)
    local value = tonumber(param_t[1])
    local x = tonumber(param_t[2])
    return string.format("%." .. x .. "f", value)
end

-- 变身卡
function RTParseShow.change_card(param_t)
    local seq = tonumber(param_t[1])
    local cfg = LiveData.Instance:GetPropCfg(seq) or {}
    return cfg.name or ""
end

function RTParseShow.wedding_bless(param_t)
    local name = param_t[1]
    local shop_item_index = tonumber(param_t[2])
    local co = Cfg.GetMarriedHeatItemCoByIndex(shop_item_index)
    if co and language_show.wedding_bless[shop_item_index] then
        return string.format(language_show.wedding_bless[shop_item_index], name, Item.GetQuaName(co.item_id))
    end
end

function RTParseShow.qing_yuan_gift(param_t)
    local type = tonumber(param_t[1])
    local config = Config.get_married_auto.marry_gift[type]
    return config and config.gift_name or ""
end

function RTParseShow.peak_token(param_t)
    local config = SceneData.Instance:GetSceneCfg(tonumber(param_t[1]))
    return config.name .. language_show.PeakToken
end

function RTParseShow.build_type(param_t)
    local m_type = tonumber(param_t[1])
    return GuildBuildingData.Instance:BuildingName(m_type)
end
function RTParseShow.shitu_break_time_notice(param_t)
    local id = RoleData.Instance:GetRoleId()
    local name = tostring(param_t[2])
    if id ~= tonumber(param_t[1]) then
        name = tostring(param_t[2])
        name = RichTextHelper.ColorStr(name, COLORSTR.Green3)
    end
    if id ~= tonumber(param_t[3]) then
        name = tostring(param_t[4])
        name = RichTextHelper.ColorStr(name, COLORSTR.Green3)
    end
    local time = TimeManager.FormatDHMS(tonumber(param_t[5]))
    return string.format(Language.Society.Master.MailDesc[1], name)
end
function RTParseShow.shitu_break_notice(param_t)
    local id = RoleData.Instance:GetRoleId()
    local name = tostring(param_t[2])
    if id ~= tonumber(param_t[1]) then
        name = tostring(param_t[2])
        name = RichTextHelper.ColorStr(name, COLORSTR.Green3)
    end
    if id ~= tonumber(param_t[3]) then
        name = tostring(param_t[4])
        name = RichTextHelper.ColorStr(name, COLORSTR.Green3)
    end
    return string.format(Language.Society.Master.MailDesc[2], name)
end

function RTParseShow.huanshou_egg_type(param_t)
    local egg_id = tonumber(param_t[1])
    local cfg = Config.huanshou_auto.egg_item[egg_id + 1]
    local item_id = cfg.item_id
    return Item.GetQuaName(item_id)
end

function RTParseShow.huanshou_type(param_t)
    local qua = tonumber(param_t[1])
    return Language.HuanShou.QuaStr3[qua]
end

function RTParseShow.huanshou_name(param_t)
    local huanshou_id = tonumber(param_t[1])
    local qua_name, name, color = HuanShou.GetQuaName(huanshou_id)
    qua_name = ColorStr(string.format("【%s】", name), color)
    return qua_name
end
function RTParseShow.guixuzhijing_rank_subject(param_t)
    LogDG("归墟邮件信息", param_t)
    local rank_type = tonumber(param_t[1])
    return Language.GodAreasActs.MailRankName[rank_type + 3]
end
function RTParseShow.guixuzhijing_rank_content(param_t)
    LogDG("归墟邮件信息内容", param_t)
    local rank_type = tonumber(param_t[1])
    local rank_value = tonumber(param_t[2])
    return string.format(Language.GodAreasActs.RuinMail, Language.GodAreasActs.MailRankName[rank_type + 3],rank_value)
end
function RTParseShow.yundingtiangong_rank_subject(param_t)
    local rank_type = tonumber(param_t[1])
    return Language.GodAreasActs.MailRankName2[rank_type + 4]
end
function RTParseShow.yundingtiangong_rank_content(param_t)
    local rank_type = tonumber(param_t[1])
    local rank_value = tonumber(param_t[2])
    return string.format(Language.GodAreasActs.PalacesMail, Language.GodAreasActs.MailRankName2[rank_type + 4],rank_value)
end

function RTParseShow.broadcast_drop_type(param_t)
    return Language.Common.BroadcastDropType[tonumber(param_t[1])]
end