
RichTextFormat = {}

local language_format = Language.RichText.Format

-- 无需下划线和点击事件的物品
local itemGetNoLink = {
    [CommonItemId.Coin] = true,
    [CommonItemId.Gold] = true,
    [CommonItemId.Exp] = true,
    [CommonItemId.Prestige] = true,
    [CommonItemId.ArenaPoint] = true,
    [CommonItemId.LevelComplementExp] = true,
    [CommonItemId.LingLiZhi] = true,
}

function RichTextFormat.FormatChatMainViewSpecial(chat_info, param_t)
    if nil == param_t then
        -- Debuger.LogError(chat_info.info)
        return
    end
    if RichTextType.Emoj == param_t.type then
        local res_id = param_t.res_id
        if res_id >= 1 and res_id <= ChatConfig.EMOJ_NUM_MAX then
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojSize.w, ChatConfig.EmojSize.h, {offset_y = 5, rate = 0.5}, "emoj")
        elseif res_id >= 100 and res_id <= 100 + ChatConfig.EMOJD_NUM_MAX then
            res_id = res_id - 100
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojDPicWidth[res_id] or ChatConfig.EmojSize.dw, ChatConfig.EmojDPicHeight[res_id] or ChatConfig.EmojSize.dh, {offset_y = 5, rate = 0.5, pic_num = ChatConfig.EmojDPicNum[res_id], speed = ChatConfig.EmojDPicSpeed[res_id] * 100}, "emoj")
        end
    end
    if RichTextType.Pet == param_t.type or RichTextType.ShengQi == param_t.type then
        return param_t
    end
    if RichTextType.Item == param_t.type then
        local item_list = chat_info:ItemList()
        local item = item_list[param_t.index]
        local item_data = Item.Init(item.item_id)
        BagData.Instance:ChatItemShowPlus(item_data,item.show,item.show2)
        local show_text = ""
        if item_data:IsEquip() or item_data:IsJewelry() then
            if item_data:IsStarGod() then
                show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.GodEquipLinkDesc, item_data:Name(), param_t.param_show), item_data:ColorStr())
            else
                show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr())
            end
        else
            show_text = item_data:QuaName()
        end
        return RichTextHelper.LinkStr(show_text, "ItemSignId", item.sign_id, "_ul", item_data:ColorStr())
    end
    if RichTextType.Recruit == param_t.type then
        local language = language_format.Recruit
        local knight_sp = param_t.is_complete and RichTextHelper.LinkStr(RichTextHelper.SpriteStrAtlas("_LocXiaYiZhi", 74, 30), 1, "teamApplyTips") or ""
        if param_t.other_desc and param_t.other_desc ~= "" then
            return string.format(language_format.RecruitOther, knight_sp, param_t.other_desc, param_t.num, param_t.num_max, param_t.low_level, param_t.high_level) .. RTParseLink.TeamApply(param_t.team_index, param_t.is_cross, COLORSTR.Purple1)
        end
        return string.format(language, knight_sp, param_t.team_target, param_t.target_name, param_t.num, param_t.num_max, param_t.low_level, param_t.high_level) .. RTParseLink.TeamApply(param_t.team_index, param_t.is_cross, COLORSTR.Purple1)
    end
    if RichTextType.GroupInvite == param_t.type then
        local language = language_format.GroupIvite
        return string.format(language, param_t.group_name) .. param_t.link_str
    end
    if RichTextType.HongBao == param_t.type then
        local show_txt = ChatHongBaoItem.TypeToString(param_t.hb_sys_type)
        show_txt = RichTextHelper.ColorStr(show_txt,COLORSTR.Red8)
        return RichTextHelper.LinkStr(show_txt,"HongBaoId",param_t.id,"_ul", COLORSTR.Red8) .. 
            RichTextHelper.SpriteStrAtlas("XiaoHongBao", 26, 27)
    end
    return param_t.str
end

function RichTextFormat.FormatChatMainViewChannel(channel_name, special)
    return RichTextHelper.ColorStr(string.format("%s%s", channel_name, special and  "】】】" or "】"), COLORSTR.Transparent)
end

function RichTextFormat.FormatChatMainViewSystem(info, channel_info, chat_info)
    local channel_name = RichTextFormat.FormatChatMainViewChannel(channel_info:ChannelName())
    local language = language_format.MainViewSystem
    if nil ~= info.item_get then
        local item = info.item_get.item
        local num = info.item_get.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return channel_name .. RichTextHelper.ColorStr(string.format(language.ItemGet, itemGetNoLink[item.item_id] and
            (item.qua_name or item:QuaName())
            or RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", info.item_get.is_grey and COLORSTR.Gray2 or item:ColorStr()), num), channel_info:ChannelColor())
    elseif nil ~= info.item_lost then
        local item = info.item_lost.item
        local num = info.item_lost.num or item:Num()
        return channel_name .. RichTextHelper.ColorStr(string.format(language.ItemLost, item.qua_name or item:QuaName(), num), channel_info:ChannelColor())
    elseif nil ~= info.pet_exp then
        -- 宠物获得经验不需要链接和下划线
        local item = info.pet_exp.item
        local num = info.pet_exp.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        -- return channel_name .. RichTextHelper.ColorStr(string.format(language.PetExp ,info.pet_exp.name, RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), channel_info:ChannelColor())
        return channel_name .. RichTextHelper.ColorStr(string.format(language.PetExp ,info.pet_exp.name, item.qua_name or item:QuaName(), num), channel_info:ChannelColor())
    elseif nil ~= info.role_exp then
        local item = info.role_exp.item
        local num = info.role_exp.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        content = channel_name .. RichTextHelper.ColorStr(string.format(language.ItemGet, itemGetNoLink[item.item_id] and
            (item.qua_name or item:QuaName())
            or RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", info.role_exp.is_grey and COLORSTR.Gray2 or item:ColorStr()), num), channel_info:ChannelColor())
        if info.addition then
            content = string.format("%s\n%s",content,info.addition)
        end
		return content
    elseif nil ~= info.seal_get then
        local str = RichTextHelper.ColorStr(string.format(language.SealGet, info.seal_get.name), channel_info:ChannelColor())
        return channel_name .. str
    elseif nil ~= info.pet_seal_get then
        local item = info.pet_seal_get.item
        local num = item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return channel_name .. RichTextHelper.ColorStr(string.format(language.PetSealGet, info.pet_seal_get.pet:QuaName(),
                RichTextHelper.LinkStr(item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), channel_info:ChannelColor())
    elseif nil ~= info.task_sub then
        local item = info.task_sub.item
        local num = info.task_sub.num
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return channel_name .. RichTextHelper.ColorStr(string.format(language.TaskSub, RichTextHelper.LinkStr(item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), channel_info:ChannelColor())
    elseif nil ~= info.cross_get then
        --获取服务端
        local str = language.CrossGet
        if LoginData.Instance:IsOnCrossSever() then
            str = RichTextHelper.ColorStr(string.format(str, Language.Login.CrossRealm), channel_info:ChannelColor())
        else
            str = RichTextHelper.ColorStr(string.format(str, info.cross_get.name), channel_info:ChannelColor())
        end
        return channel_name .. str
    elseif nil ~= info.pet_get then--获得宠物
        local pet_info = PetData.Instance:GetPetInfo(info.pet_get.index)
        if pet_info ~= nil then
            local str = RichTextHelper.ColorStr(string.format(language.PetGet, pet_info:Name()), channel_info:ChannelColor())
            return channel_name .. str
        end
    elseif nil ~= info.rumor_th then
        local state = info.rumor_th.state
        local min = math.floor((info.rumor_th.sec or 600) / 60)
        return channel_name .. string.format(language.RumorTh[state], min) .. RichTextHelper.LinkStr(language.RumorTh.link, "modKey", Mod.TreasureHunt.Main, "_ul", COLORSTR.Green7)
    elseif nil ~= info.team_salary then
        return channel_name .. RichTextHelper.LinkStr(RichTextHelper.SpriteStrAtlas("_LocDuiZhangGongZi", 94, 30), 1, "teamSalary") .. RichTextHelper.ColorStr(string.format(language.TeamSalary, RichTextHelper.SpriteStrAtlas(CommonItemId.LeaderCoin, 25, 25), info.team_salary), channel_info:ChannelColor())
    else
        local content =  RichTextHelper.ParseAllFunc(RichTextHelper.ColorStr(info.content, COLORSTR.Yellow16), true, RichTextFormat.FormatChatMainViewSpecial)
		return RichTextFormat.FormatChatMainViewNormal(info, channel_info, content)
    end
end

function RichTextFormat.FormatChatMainViewNormal(info, channel_info, content)
    local channel_name = RichTextFormat.FormatChatMainViewChannel(channel_info:ChannelName(), ChatConfig.MsgType.trumpet == info.msg_type and ChatConfig.ChannelId.general == channel_info:ChannelId())
    local str = channel_name
    if nil ~= info.prof and info.prof > 0 then
        local prof_sp = RoleData.GetProfSpByProfId(info.prof)
        str = str .. RichTextHelper.SpriteStrAtlas(prof_sp, 25, 25, {offset_y = 4})
    end
    if nil ~= info.name then
        str = str .. RichTextHelper.ColorStr(info.name .. "：", COLORSTR.Yellow2)
    end
    if "table" == type(content) then
        local item_list = info.item_list
        if nil == item_list then return str .. content.name end
        if RichTextType.Pet == content.type then
            local item = item_list[1]
            local item_data = Item.Init(item.item_id)
            str = str .. RichTextHelper.LinkStr(item_data:QuaName(), "ItemSignId", item.sign_id, "_ul", item_data:ColorStr()) .. RichTextHelper.SpriteStrAtlas(content.quality, 45, 29)
        else
            local item = item_list[content.seq]
            local item_data = Item.Init(item.item_id)
            local show_text = item_data:IsEquip() and RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr()) or item_data:QuaName()
            str = str .. RichTextHelper.LinkStr(show_text, "ItemSignId", item.sign_id, "_ul", item_data:ColorStr())
        end
    else
        str = str .. content
    end
    return str
end

function RichTextFormat.FormatChatContentSpecial(chat_info, param_t)
    if nil == param_t then
        -- Debuger.LogError(chat_info.info)
        return
    end
    if RichTextType.Emoj == param_t.type then
        local res_id = param_t.res_id
        if res_id >= 1 and res_id <= ChatConfig.EMOJ_NUM_MAX then
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojSize.w, ChatConfig.EmojSize.h, {offset_y = 5, size = 40, rate = 0.8}, "emoj")
        elseif res_id >= 100 and res_id <= 100 + ChatConfig.EMOJD_NUM_MAX then
            res_id = res_id - 100
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojDPicWidth[res_id] or ChatConfig.EmojSize.dw, ChatConfig.EmojDPicHeight[res_id] or ChatConfig.EmojSize.dh, {offset_y = 5, size = 40, rate = 0.8, pic_num = ChatConfig.EmojDPicNum[res_id], speed = ChatConfig.EmojDPicSpeed[res_id] * 100}, "emoj")
        end
    end
    if RichTextType.Pet == param_t.type or 
        RichTextType.HongBao == param_t.type or 
        RichTextType.HongBaoGet == param_t.type or
        RichTextType.SocietyGift == param_t.type or
        RichTextType.SocietyGiftFlower == param_t.type or
        RichTextType.ShengQi == param_t.type or
        RichTextType.SocietyShiTuInvite == param_t.type then
        return param_t
    end
    if RichTextType.Item == param_t.type then
        local item_list = chat_info:ItemList()
        local item = item_list[param_t.index]
        local item_data = item.item or Item.Init(item.item_id)
        BagData.Instance:ChatItemShowPlus(item_data,item.show,item.show2)
        local show_text = ""
        if item_data:IsEquip() or item_data:IsJewelry() then
            if item_data:IsStarGod() then
                show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.GodEquipLinkDesc, item_data:Name(), param_t.param_show), item_data:ColorStr())
            else
                show_text = RichTextHelper.ColorStr(string.format(Language.RichText.Format.EquipLinkDesc, item_data:Name(), item_data:LimitLv()), item_data:ColorStr())
            end
        else
            show_text = item_data:QuaName()
        end
        if item.sign_id then
            return RichTextHelper.LinkStr(show_text, "ItemSignId", item.sign_id, "_ul", item_data:ColorStr())
        else
            local infoHandle = RichTextHelper.StoreInfo(item_data)
            table.insert(chat_info.info_handles, infoHandle)
            return RichTextHelper.LinkStr(show_text, "infoHandle", infoHandle, "_ul", item_data:ColorStr())
        end
    end
    if RichTextType.Recruit == param_t.type then
        local language = language_format.Recruit
        local knight_sp = param_t.is_complete and RichTextHelper.LinkStr(RichTextHelper.SpriteStrAtlas("_LocXiaYiZhi", 74, 30), 1, "teamApplyTips") or ""
        if param_t.other_desc and param_t.other_desc ~= "" then
            return string.format(language_format.RecruitOther, knight_sp, param_t.other_desc, param_t.num, param_t.num_max, param_t.low_level, param_t.high_level) .. RTParseLink.TeamApply(param_t.team_index, param_t.is_cross, COLORSTR.Purple1)
        end
        return string.format(language, knight_sp, param_t.team_target, param_t.target_name, param_t.num, param_t.num_max, param_t.low_level, param_t.high_level) .. RTParseLink.TeamApply(param_t.team_index, param_t.is_cross, COLORSTR.Purple1)
    end
    if RichTextType.GroupInvite == param_t.type then
        local language = language_format.GroupIvite
        return string.format(language, param_t.group_name) .. param_t.link_str
    end
    return param_t.str
end

function RichTextFormat.FormatChatContentSystem(info, channel_info, chat_info)
    local language = language_format.ContentSystem
    local chat_tips = RichTextHelper.SpriteStrAtlas("_LocLiaoTianTiShi", 54, 28, {offset_y = 3})
    if ChatConfig.MsgType.bullet == info.msg_type then
        return RichTextHelper.SpriteStrAtlas("_LocDanMu", 54, 28) .. info.content
    end
    if nil ~= info.item_get then
        local item = info.item_get.item
        local num = info.item_get.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return chat_tips .. RichTextHelper.ColorStr(string.format(language.ItemGet, itemGetNoLink[item.item_id] and
            (item.qua_name or item:QuaName())
            or RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", info.item_get.is_grey and COLORSTR.Gray2 or item:ColorStr()), num), COLORSTR.White)
    elseif nil ~= info.item_lost then
        local item = info.item_lost.item
        local num = info.item_lost.num or item:Num()
        return chat_tips .. RichTextHelper.ColorStr(string.format(language.ItemLost, item.qua_name or item:QuaName(), num), COLORSTR.White)
    elseif nil ~= info.pet_exp then
        -- 宠物获得经验不需要链接和下划线
        local item = info.pet_exp.item
        local num = info.pet_exp.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        -- return chat_tips .. RichTextHelper.ColorStr(string.format(language.PetExp ,info.pet_exp.name, RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), COLORSTR.White)
        return chat_tips .. RichTextHelper.ColorStr(string.format(language.PetExp ,info.pet_exp.name, item.qua_name or item:QuaName(), num), COLORSTR.White)
    elseif nil ~= info.pet_seal_get then
        local item = info.pet_seal_get.item
        local num = item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return chat_tips .. RichTextHelper.ColorStr(string.format(language.PetSealGet, info.pet_seal_get.pet:QuaName(),
                RichTextHelper.LinkStr(item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), COLORSTR.White)
    elseif nil ~= info.task_sub then
        local item = info.task_sub.item
        local num = info.task_sub.num
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        return RichTextHelper.ColorStr(string.format(language.TaskSub, RichTextHelper.LinkStr(item:QuaName(), "infoHandle", infoHandle, "_ul", item:ColorStr()), num), channel_info:ChannelColor())
    elseif nil ~= info.seal_get then
        local str = RichTextHelper.ColorStr(string.format(language.SealGet, info.seal_get.name), channel_info:ChannelColor())
        return chat_tips .. str
    elseif nil ~= info.cross_get then
        --获取服务端
        local str = language.CrossGet
        if LoginData.Instance:IsOnCrossSever() then
            str = RichTextHelper.ColorStr(string.format(str, Language.Login.CrossRealm), COLORSTR.White)
        else
            str = RichTextHelper.ColorStr(string.format(str, info.cross_get.name), COLORSTR.White)
        end
        return chat_tips .. str
    elseif nil ~= info.pet_get then
        local pet_info = PetData.Instance:GetPetInfo(info.pet_get.index)
        if pet_info ~= nil then
            local str = RichTextHelper.ColorStr(string.format(language.PetGet, pet_info:Name()), COLORSTR.White)
            return chat_tips .. str
        end
    elseif nil ~= info.role_exp then
        local item = info.role_exp.item
        local num = info.role_exp.num or item:Num()
        local infoHandle = RichTextHelper.StoreInfo(item)
        table.insert(chat_info.info_handles, infoHandle)
        content = chat_tips .. RichTextHelper.ColorStr(string.format(language.ItemGet, itemGetNoLink[item.item_id] and
            (item.qua_name or item:QuaName())
            or RichTextHelper.LinkStr(item.qua_name or item:QuaName(), "infoHandle", infoHandle, "_ul", info.role_exp.is_grey and COLORSTR.Gray2 or item:ColorStr()), num), channel_info:ChannelColor())
        if info.addition then
            content = string.format("%s\n%s",content,info.addition)
        end
		return content
    elseif nil ~= info.rumor_th then
        local state = info.rumor_th.state
        local min = math.floor((info.rumor_th.sec or 600) / 60)
        chat_tips = RichTextHelper.SpriteStrAtlas("_ChuangWeng", 54, 28, {offset_y = 3})
        return chat_tips .. string.format(language.RumorTh[state], min).. RichTextHelper.LinkStr(language.RumorTh.link, "modKey", Mod.TreasureHunt.Main, "_ul", COLORSTR.Green7)
    elseif nil ~= info.team_salary then
        return chat_tips .. RichTextHelper.LinkStr(RichTextHelper.SpriteStrAtlas("_LocDuiZhangGongZi", 94, 30), 1, "teamSalary") .. RichTextHelper.ColorStr(string.format(language.TeamSalary, RichTextHelper.SpriteStrAtlas(CommonItemId.LeaderCoin, 25, 25), info.team_salary), COLORSTR.White)
    elseif string.find(info.content or "","randactivity_notice") or string.find(info.content or "","serveractivity_open") or string.find(info.content or "","mod_key") or string.find(info.content or "","medal_up_grade") 
        or string.find(info.content or "","hill_sea_notice") or string.find(info.content or "","medal_lottery") then
        chat_tips = RichTextHelper.SpriteStrAtlas("_ChuangWeng", 54, 28, {offset_y = 3})
        local content = RichTextHelper.ParseAllFunc(RichTextHelper.ColorStr(info.content, COLORSTR.Yellow16), true, RichTextFormat.FormatChatContentSpecial)
        return chat_tips .. content
    else
        local content = RichTextHelper.ParseAllFunc(RichTextHelper.ColorStr(info.content, COLORSTR.Yellow16), true, RichTextFormat.FormatChatContentSpecial)
		return RichTextFormat.FormatChatMainViewNormal(info, channel_info, content), true
    end
end

function RichTextFormat.FormatNpcBubbleSpecial(param_t)
    if RichTextType.Emoj == param_t.type then
        local res_id = param_t.res_id
        if res_id >= 1 and res_id <= ChatConfig.EMOJ_NUM_MAX then
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojSize.w, ChatConfig.EmojSize.h, {offset_y = 5, rate = 0.5}, "emoj")
        elseif res_id >= 100 and res_id <= 100 + ChatConfig.EMOJD_NUM_MAX then
            res_id = res_id - 100
            return RichTextHelper.SpriteStrAtlas(res_id, ChatConfig.EmojDPicWidth[res_id] or ChatConfig.EmojSize.dw, ChatConfig.EmojDPicHeight[res_id] or ChatConfig.EmojSize.dh, {offset_y = 5, rate = 0.5, pic_num = ChatConfig.EmojDPicNum[res_id], speed = ChatConfig.EmojDPicSpeed[res_id] * 100}, "emoj")
        end
    end
    return param_t.str
end