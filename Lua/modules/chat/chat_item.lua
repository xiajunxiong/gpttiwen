----------------- chat_item -----------------
ChatItem = ChatItem or BaseClass()

function ChatItem:__init(param_t)
    self.type = param_t.type or -1
    self.param1 = param_t.param1 or -1
    self.param2 = param_t.param2 or -1
end

function ChatItem:Type()
    return self.type
end

function ChatItem:Param1()
    return self.param1
end

function ChatItem:Param2()
    return self.param2
end

function ChatItem.EmptyItem()
    return ChatItem.New{
        type = -1,
        param1 = -1,
        param2 = -1,
    }
end

function ChatItem.Mark()
    return "chat_quick_item"
end

function ChatItem:ShowText()
    return ""
end

function ChatItem:WordNum()
    return ChatConfig.ITEM_WORD_NUM
end

function ChatItem:ParamShow()
    return 0
end
----------------- chat_item -----------------

----------------- chat_item_mono -----------------
ChatItemMono = ChatItemMono or BaseClass(ChatItem)

function ChatItemMono:__init(item)
    self.type = ChatConfig.ChatItemType.Item
    self.param1 = item.column_type
    self.param2 = item.index
    self.item = item
end

function ChatItemMono.Mark()
    return "chat_quick_item"
end

function ChatItemMono:ShowText()
    local item_name = self.item:Name()
    if self.item:IsEquip() or self.item:IsJewelry() then
        if self.item:IsStarGod() then
            return string.format("%s(%s)", item_name, self.item:StrengthenLevel())
        else
            return string.format("%s(%s)", item_name, self.item:LimitLv())
        end
    else
        return item_name
    end
end

function ChatItemMono:Item()
    return self.item
end

function ChatItemMono.ItemParse(index, param_show)
    return {
        type = RichTextType.Item,
        index = index,
        param_show = param_show,
    }
end

function ChatItemMono:ParamShow()
    return self.item:IsStarGod() and self.item:StrengthenLevel() or 0
end

ChatItemMonoEquip = ChatItemMonoEquip or BaseClass(ChatItemMono)

function ChatItemMonoEquip:__init(item)
    self.type = ChatConfig.ChatItemType.Equip
    self.param1 = item.chat_quick_show_equip
    self.param2 = -1
end

ChatItemMonoJewelry = ChatItemMonoJewelry or BaseClass(ChatItemMono)

function ChatItemMonoJewelry:__init(item)
    self.type = ChatConfig.ChatItemType.Jewelry
    self.param1 = item.chat_quick_show_jewelry
    self.param2 = -1
end

ChatItemMonoMedal = ChatItemMonoMedal or BaseClass(ChatItemMono)

function ChatItemMonoMedal:__init(item)
    self.type = ChatConfig.ChatItemType.Medal
    self.param1 = item.chat_quick_show_medal
    self.param2 = -1
end

----------------- chat_item_mono -----------------

----------------- chat_item_pet -----------------
ChatItemPet = ChatItemPet or BaseClass(ChatItem)

function ChatItemPet:__init(pet)
    self.type = ChatConfig.ChatItemType.Pet
    self.param1 = pet.index
    self.pet = pet
end

function ChatItemPet:RealText()
    return self.quick_msg
end

function ChatItemPet:Pet()
    return self.pet
end

function ChatItemPet.ItemIntro(pet)
    return string.format("{chat_quick_pet;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s}",
        pet:ID(),
        RichTextHelper.ColorStr(pet:Name(), COLORSTR.Yellow13),
        string.format("%s/%s", RichTextHelper.ColorStr(pet:TotalGrowth(), COLORSTR.Green3), RichTextHelper.ColorStr(pet:ConfigGrowth(), COLORSTR.Yellow1)),
        pet:GetScore(),
        pet:Level(),
        pet:StrLevel(),
        pet.config.mutant_times,
        PetData.PetLevel[pet.config.quality],
        pet:SkinId(),
        pet:FeiShengTimes(),
        pet.info.lian_yao_pet_id
    )
end

function ChatItemPet.ItemParse(param_t)
    return {
        type = RichTextType.Pet,
        id = tonumber(param_t[1]),
        name = param_t[2],
        growth = param_t[3],
        cap = tonumber(param_t[4]),
        level = tonumber(param_t[5]),
        str_level = tonumber(param_t[6]),
        mutant_times = tonumber(param_t[7]),
        quality = param_t[8],
        skin_id = tonumber(param_t[9]),
        fs_lv = tonumber(param_t[10]),
        lian_yao_pet_id = tonumber(param_t[11])
    }
end
----------------- chat_item_pet -----------------



----------------- chat_item_shengqi -----------------
ChatItemShengQi = ChatItemShengQi or BaseClass(ChatItem)

function ChatItemShengQi:__init(sheng_qi)
    self.type = ChatConfig.ChatItemType.ShengQi
    self.param1 = sheng_qi.index
    self.sheng_qi = sheng_qi
end

function ChatItemShengQi:RealText()
    return self.quick_msg
end

function ChatItemShengQi:ShengQi()
    return self.sheng_qi
end

function ChatItemShengQi.ItemIntro(sheng_qi)
    return string.format("{chat_quick_shengqi;%s;%s;%s;%s;%s}",
        sheng_qi.item_id,
        RichTextHelper.ColorStr(sheng_qi:Name(), COLORSTR.Yellow13),
        string.format("%s", RichTextHelper.ColorStr(sheng_qi:TypeName(), COLORSTR.Yellow1)),
        sheng_qi:Score(),
        sheng_qi:Level()
    )
end

function ChatItemShengQi.ItemParse(param_t)
    return {
        type = RichTextType.ShengQi,
        item_id = tonumber(param_t[1]),
        name = param_t[2],
        type_name = param_t[3],
        score = tonumber(param_t[4]),
        level = tonumber(param_t[5]),
        seq = 1,
    }
end




----------------- chat_item_gift -----------------
ChatItemGift = ChatItemGift or BaseClass(ChatItem)

function ChatItemGift.ItemIntro(uuid_key, state)
    return string.format("{chat_quick_gift;%s;%s}",
        uuid_key, state
    )
end

function ChatItemGift.ItemParse(param_t)
    return {
        type = RichTextType.SocietyGift,
        uuid_key = param_t[1],
        state = tonumber(param_t[2])
    }
end
----------------- chat_item_gift -----------------

----------------- chat_item_gift_flower -----------------
ChatItemGiftFlower = ChatItemGiftFlower or BaseClass(ChatItem)

function ChatItemGiftFlower.ItemIntro(uuid_key)
    return string.format("{chat_quick_gift_flower;%s}", uuid_key)
end

function ChatItemGiftFlower.ItemParse(param_t)
    return {
        type = RichTextType.SocietyGiftFlower,
        uuid_key = param_t[1],
    }
end
----------------- chat_item_gift -----------------

----------------- chat_item_shitu_invite -----------------
ChatItemShiTuInvite = ChatItemShiTuInvite or BaseClass(ChatItem)

function ChatItemShiTuInvite.ItemIntro(state)
    return string.format("{chat_quick_shitu_invite;%s}", state)
end

function ChatItemShiTuInvite.ItemParse(param_t)
    return {
        type = RichTextType.SocietyShiTuInvite,
        state = tonumber(param_t[1])
    }
end
----------------- chat_item_gift -----------------

----------------- chat_item_emoj -----------------
ChatItemEmoj = ChatItemEmoj or BaseClass(ChatItem)

function ChatItemEmoj:__init(emoj)
    self.type = ChatConfig.ChatItemType.Emoj
    self.quick_msg = emoj.quick_msg
end

function ChatItemEmoj:RealText()
    return self.quick_msg
end

function ChatItemEmoj:ShowText()
    return self.quick_msg
end

function ChatItemEmoj:WordNum()
    return DataHelper.GetStringWordNum(self.quick_msg)
end

function ChatItemEmoj.ItemParse(res_id)
    return {
        type = RichTextType.Emoj,
        res_id = res_id,
        str = string.format("/%se", res_id)
    }
end
----------------- chat_item_emoj -----------------

----------------- chat_pet_item -----------------
ChatPetItem = DeclareMono("ChatPetItem", RichTextItem)

function ChatPetItem:SetData(data, info)
	self.info = info
    local pet = Pet.New{id = data.id, skin_id = data.skin_id,fei_sheng_times = data.fs_lv, lian_yao_pet_id = data.lian_yao_pet_id}
	self.Cell:SetData(pet)
	UH.SetText(self.Name, pet:Name())--data.name
    if pet.IsLianYao then
        UH.Color(self.Name, pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
        if self.YaoObj then
            self.YaoObj:SetActive(pet:IsLianYao())
        end
    end
	self.Growth.text = data.growth
	self.Cap.text = data.cap
	UH.SetText(self.Level, data.level)
	PetData.SetStrLevel(nil, self.StrLevel, data.str_level)
end

function ChatPetItem:OnClickItem()
    if nil == self.info then return end
    if self.info.sign_id then
        ChatCtrl.Instance:SendChatItemInfoReq(self.info.sign_id)
    elseif self.info.Pet then
        ViewMgr:OpenView(PetDetailView, self.info:Pet())
    end
end

----------------- chat_pet_item -----------------

----------------- chat_gift_item -----------------
ChatGiftItem = DeclareMono("ChatGiftItem", RichTextItem)
function ChatGiftItem:ChatGiftItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.Share
end

function ChatGiftItem:SetData(data, info, pos)
    UIWidgetBaseItem.SetData(self, data)
	self.info = info
    pos = pos or 0
    
	local is_self = RoleData.Instance:GetRoleId() == info.from_uid
    self.ItemPos.localPosition = Vector3.New(is_self and pos or -pos, 0, 0)
    self.BgPos.localScale = is_self and Vector3.New(-1,1,1) or Vector3.one

    self:FlushInfo()
end

function ChatGiftItem:FlushInfo()
    local gift_info = self.society_data.get_gift_info[self.data.uuid_key or 0]
    if gift_info then
        local is_get = gift_info.fetch_time > 0
        gift_info.item.is_grey = is_get
        self.Cell:SetData(gift_info.item)
        self.Name.text = gift_info.item:Name()
        self.Content.text = (SocietyConfig.GiftMsgType.fetch == self.data.state and is_get) and self.language.GetMsg or gift_info.str
        self.State.text = is_get and self.language.Geted or self.language.NotGet
        self.ItemInter.Interactable = not is_get
    end
end

function ChatGiftItem:OnClickItem()
    if nil == self.info then return end
    local gift_info = self.society_data.get_gift_info[self.data.uuid_key or 0]
    if gift_info then
        -- if SocietyConfig.GiftMsgType.fetch == self.data.state and RoleData.Instance:GetRoleId() == gift_info.to_role_id then
        --     SocietyCtrl.Instance:SendGiftReqFetch(gift_info.uuid)
        -- end
        self.society_data:SetSocietyGiftInfo(self.data.state, self.data.uuid_key)
        ViewMgr:OpenView(SocietyGiftView)
    end
end

----------------- chat_gift_flower_item -----------------
ChatGiftFlowerItem = DeclareMono("ChatGiftFlowerItem", RichTextItem)
function ChatGiftFlowerItem:ChatGiftFlowerItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.Share
end

function ChatGiftFlowerItem:SetData(data, info, pos)
    UIWidgetBaseItem.SetData(self, data)
	self.info = info
    pos = pos or 0
    
	local is_self = RoleData.Instance:GetRoleId() == info.from_uid
    self.ItemPos.localPosition = Vector3.New(is_self and pos or -pos, 0, 0)
    self.BgPos.localScale = is_self and Vector3.New(-1,1,1) or Vector3.one

    self:FlushInfo()
end

function ChatGiftFlowerItem:FlushInfo()
    local gift_info = self.society_data.get_gift_info[self.data.uuid_key or 0]
    if gift_info then
        self.Cell:SetData(gift_info.item)
        self.Name.text = gift_info.item:Name()
        self.Content.text = gift_info.str
        self.Desc.text = string.format(self.language.flower_intimacy_add, self.society_data:GetGiftFlowerIntimacyAdd(gift_info.item.item_id))
    end
end

----------------- chat_shitu_invite_item -----------------
ChatShiTuInviteItem = DeclareMono("ChatShiTuInviteItem", RichTextItem)
function ChatShiTuInviteItem:ChatShiTuInviteItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuInvite
end

function ChatShiTuInviteItem:SetData(data, info, pos)
    UIWidgetBaseItem.SetData(self, data)
	self.info = info
    pos = pos or 0
    
	local is_self = RoleData.Instance:GetRoleId() == info.from_uid
    self.ItemPos.localPosition = Vector3.New(is_self and pos or -pos, 0, 0)
    -- self.BgPos.localScale = is_self and Vector3.New(-1,1,1) or Vector3.one

    self.ShowDesc.text = self.language.StateShow[data.state] or ""
end

function ChatShiTuInviteItem:OnClickInvite()
    SocietyCtrl.Instance:SendShiTuReqInvite()
end

----------------- chat_pet_item -----------------

ChatRecordItem = DeclareMono("ChatRecordItem")
local rec_speech_ret_cache = {}
--SpeechTxt======Text
--RecTimeTxt=====Text
--SpeechBtnObj===GameObject
--SpeechWaitObj==GameObject
--RootTran=======RectTransform
--RootVerticalLayout==VerticalLayoutGroup
--ScrollListLayout==ScrollVerticalLayoutGroup
--HorLayout=======HorizontalLayoutGroup
--BgImageSet======UIImageSpriteSet
--TextSplitDuiFangObj===GameObject
--TextSplitZiJiObj======GameObject
--RecIconAnim=====Animation
local rec_max_sec = 30
local rec_min_sec = 3
local rec_min_w = 125
local rec_max_w = 335
local layout_padding_x = nil
local speech_btn_pos = nil
local speech_wait_pos = nil

function ChatRecordItem:ChatRecordItem()
    if layout_padding_x == nil then
        layout_padding_x = self.RootVerticalLayout.padding.left + self.RootVerticalLayout.padding.right
    end
    if speech_btn_pos == nil then
        speech_btn_pos = math.abs(self.SpeechBtnObj.transform.anchoredPosition.x)
    end
    if speech_wait_pos == nil then
        speech_wait_pos = math.abs(self.SpeechWaitObj.transform.anchoredPosition.x)
    end
end

-- function ChatRecordItem:ItemHeight()
--     local re = math.max(self.RootVerticalLayout.preferredHeight + 35,95)
--     LogError("ChatRecordItem:Height===",re,self.RootVerticalLayout.preferredHeight,self.rec_time)
--     return re
-- end

function ChatRecordItem:Awake()
    self.care_handle = ChatData.Instance:RecordData():Care(BindTool.Bind(ChatRecordItem.FlushPlayingKey,self),"cur_playing_key")
end

function ChatRecordItem:OnDestroy()
    if self.care_handle ~= nil then
        ChatData.Instance:RecordData():Uncare(self.care_handle)
        self.care_handle = nil
    end
    if self:IsEnabled() then
        local cur_playing_key = ChatData.Instance:RecordData().cur_playing_key
        if cur_playing_key == self.rec_key then
            VoiceRecorder.StopPlayRecord()
            ChatData.Instance:SetCurPlayingRec("")
        end
    end
    -- LogError("CUnCARE!!!!!!!!!!!")
end

function ChatRecordItem.ItemHeight(data,exp_txt)
    local speech_ret = rec_speech_ret_cache[data:RecordKey()]
    local re_hei = 35 + 50
    if speech_ret ~= nil then
        exp_txt.text = speech_ret
        re_hei = re_hei + exp_txt.preferredHeight + 16 --垂直layout的间隔距离
    end
    re_hei = math.max(re_hei,95)
    return re_hei
end

function ChatRecordItem:SetData(rec_key,rec_time,is_self)
    self.rec_key = rec_key
    self.rec_time = rec_time
    self.is_self = is_self
    local t_min = math.floor(rec_time/60)
    local t_sec = rec_time%60
    if t_min > 0 then
        self.RecTimeTxt.text = string.format("%d'%d''",t_min,t_sec)
    else
        self.RecTimeTxt.text = string.format("%d''",t_sec)
    end
    local speech_ret = rec_speech_ret_cache[rec_key]
    local speech_pw = 0
    if speech_ret ~= nil then
        self.SpeechBtnObj:SetActive(false)
        self.SpeechTxt.text = speech_ret
        self.SpeechTxt:SetObjActive(true)
        speech_pw = self.SpeechTxt.preferredWidth + layout_padding_x
        self.TextSplitDuiFangObj:SetActive(not is_self)
        self.TextSplitZiJiObj:SetActive(is_self)
    else
        self.SpeechBtnObj:SetActive(true)
        self.SpeechTxt:SetObjActive(false)
    end
    if is_self then
        self.RootTran.pivot = Vector2.New(1,1)
        self.RootTran.anchorMax = Vector2.New(1,1)
        self.RootTran.anchorMin = Vector2.New(1,1)
        self.HorLayout.childAlignment = UnityEngine.TextAnchor.MiddleRight
        self.SpeechBtnObj.transform.anchorMax = Vector2.New(0,0.5)
        self.SpeechBtnObj.transform.anchorMin = Vector2.New(0,0.5)
        self.SpeechWaitObj.transform.anchorMax = Vector2.New(0,0.5)
        self.SpeechWaitObj.transform.anchorMin = Vector2.New(0,0.5)
        self.SpeechBtnObj.transform.anchoredPosition = Vector2.New(speech_btn_pos,self.SpeechBtnObj.transform.anchoredPosition.y)
        self.SpeechWaitObj.transform.anchoredPosition = Vector2.New(speech_wait_pos,self.SpeechWaitObj.transform.anchoredPosition.y)
        self.BgImageSet.SpriteName = "XiaoXi_YuYin_ZiJi"
        self.RootTran.anchoredPosition = Vector2.New(-5,0)
    else
        self.RootTran.pivot = Vector2.New(0,1)
        self.RootTran.anchorMax = Vector2.New(0,1)
        self.RootTran.anchorMin = Vector2.New(0,1)
        self.HorLayout.childAlignment = UnityEngine.TextAnchor.MiddleLeft
        self.SpeechBtnObj.transform.anchorMax = Vector2.New(1,0.5)
        self.SpeechBtnObj.transform.anchorMin = Vector2.New(1,0.5)
        self.SpeechWaitObj.transform.anchorMax = Vector2.New(1,0.5)
        self.SpeechWaitObj.transform.anchorMin = Vector2.New(1,0.5)
        self.SpeechBtnObj.transform.anchoredPosition = Vector2.New(-speech_btn_pos,self.SpeechBtnObj.transform.anchoredPosition.y)
        self.SpeechWaitObj.transform.anchoredPosition = Vector2.New(-speech_wait_pos,self.SpeechWaitObj.transform.anchoredPosition.y)
        self.BgImageSet.SpriteName = "XiaoXi_YuYin_DuiFang"
        self.RootTran.anchoredPosition = Vector2.New(5,0)
    end
    
    local w_per = (self.rec_time - rec_min_sec)/(rec_max_sec-rec_min_sec)
    local root_w = math.min(math.max(speech_pw,Mathf.Lerp(rec_min_w,rec_max_w,w_per)),rec_max_w)
    self.RootTran.sizeDelta = Vector2.New(root_w,self.RootTran.sizeDelta.y)

    self:FlushPlayingKey()
end

function ChatRecordItem:OnClickSpeech()
    self.SpeechBtnObj:SetActive(false)
    self.SpeechWaitObj:SetActive(true)
    VoiceRecorder.SpeechToText(self.rec_key,function(com_code,rec_key,ret)
        self.SpeechWaitObj:SetActive(false)
        if com_code == GVoiceComCode.STTSucc then
            rec_speech_ret_cache[rec_key] = ret
            self.SpeechTxt.text = ret
            self.SpeechTxt:SetObjActive(true)
            self.TextSplitDuiFangObj:SetActive(not self.is_self)
            self.TextSplitZiJiObj:SetActive(self.is_self)
            local speech_pw = self.SpeechTxt.preferredWidth + layout_padding_x
            if speech_pw > self.RootTran.sizeDelta.x then
                self.RootTran.sizeDelta = Vector2.New(math.min(speech_pw,rec_max_w),self.RootTran.sizeDelta.y)
            end
            self.ScrollListLayout:SetLayoutDirty()
        else
            self.SpeechBtnObj:SetActive(true)
            if com_code == GVoiceErrorNo.STTing then
                PublicPopupCtrl.Instance:Center(Language.Chat.SpeechingHint)
            end
        end
    end)
end

function ChatRecordItem:OnClickPlayRec()
    if ChatData.Instance:RecordData().cur_playing_key == self.rec_key then
		ChatData.Instance:SetCurPlayingRec("")
		VoiceRecorder.StopPlayRecord()
	else
        local play_suc = VoiceRecorder.PlayRecord(self.rec_key,function (com_code,rec_key)
            if com_code ~= GVoiceComCode.PlayFileDone then
                return
            end
            if self.rec_key == ChatData.Instance:RecordData().cur_playing_key then
                ChatData.Instance:SetCurPlayingRec("")
            end
        end)
        if play_suc == GVoiceErrorNo.Succ then
            ChatData.Instance:SetCurPlayingRec(self.rec_key)
        end
    end
end

function ChatRecordItem:FlushPlayingKey()
    if self:Destroyed() then
        if self.care_handle ~= nil then
            ChatData.Instance:RecordData():Uncare(self.care_handle)
            self.care_handle = nil
        end
        return
    end
    if self:IsEnabled() == false then
        return
    end
    local cur_playing_key = ChatData.Instance:RecordData().cur_playing_key
    if cur_playing_key == self.rec_key then
        self.RecIconAnim:Play("rec_icon_playing")
    else
        self.RecIconAnim:Play("rec_icon_idle")
    end
end

------- chat_item_guild_task ------------
ChatGuildTaskItem = DeclareMono("ChatGuildTaskItem", RichTextItem)
function ChatGuildTaskItem:ChatGuildTaskItem()
    RichTextHelper.Bind(self.Content, BindTool.Bind(self.OnClickLink, self))
end
function ChatGuildTaskItem:SetData(data, info)
    self.info = info
    self.data = data
    local language = Language.RichText.Format.ContentSystem
    local all_info, task_info = GuildData.Instance:GWTTaskInfo()
    if task_info == nil then
        LogError("家族任务出现错误", all_info)
        self.gameObject:SetActive(false)
        return
    end
    self.gameObject:SetActive(true)
    local str = RichTextHelper.ColorStr(string.format(language.GuildTask, task_info.desc_2), COLORSTR.White)
    if all_info.task_prog >= task_info.param_2 then
        str = RichTextHelper.ColorStr(string.format(language.GuildTaskFinish), COLORSTR.White)
    end
    local chat_tips = RichTextHelper.SpriteStrAtlas("_LocRenWuBiaoShi", 54, 28, {offset_y = 3})
    local link = RichTextHelper.LinkStr(language.GuildTaskLink, "modKey", Mod.Guild.GuildMission, "_ul", COLORSTR.Green7)
    self.Content.text = chat_tips .. str .."\n\n              "..link
    self.ValueBar:SetProgress(all_info.task_prog / task_info.param_2)
    self.ValueBar:SetText(string.format("%s/%s", all_info.task_prog, task_info.param_2))
end
function ChatGuildTaskItem:OnClickLink()
    ViewMgr:OpenViewByKey(Mod.Guild.GuildMission)
end

ChatSimpleRecordItem = DeclareMono("ChatSimpleRecordItem", UIWidgetBaseItem)

function ChatSimpleRecordItem:ChatSimpleRecordItem()
    self.chat_data = ChatCtrl.Instance:Data()
end

function ChatSimpleRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Content.text = data.content
end

function ChatSimpleRecordItem:OnClickItem()
    local str = self.data.content
    self.chat_data:ChatSimpleSwitch()
    ChatData.Instance:SendGrey()
    if self.chat_data:ChatCacheCheck(str, self.chat_data.cur_sel_channel:ChannelId()) then
		return
	end
    ChatCtrl.Instance:SendChannelChatText(str)
end

function ChatSimpleRecordItem:OnClickModify()
    ViewMgr:OpenView(ChatRecordEidtView, {index = self.data.index})
end



---------------聊天里的红包对象--------------------
ChatHongBaoItem = DeclareMono("ChatHongBaoItem")--, RichTextItem)
--HongBaoKai=====GameObject
--HongBaoGuan====GameObject
--Interactor=====InteractorHandler
--WishWord=======Text
--GetTag=========Text
--TypeTxt========Text
--BgTran=========RectTransform

function ChatHongBaoItem.TypeToString(sys_type)
    return Language.Chat.HongBao.SysType[sys_type] or 
        string.format(Language.Chat.HongBao.SysTypeUnknow,sys_type)
end

function ChatHongBaoItem:SetData(data, is_self)
    if self.data ~= nil and self.self_getted_care ~= nil then
        self.data.sm_data:Uncare(self.self_getted_care)
    end
    self.self_getted_care = nil
    self.data = data
    self.BgTran.localScale = is_self and Vector3.New(-1,1,1) or Vector3.one
    UH.SetText(self.WishWord,data.greeting)
    UH.SetText(self.TypeTxt,ChatHongBaoItem.TypeToString(data.hb_sys_type))    --收到未知红包类型时给异常表现
    
    if self.self_getted_care == nil then
        self.self_getted_care = self.data.sm_data:Care(BindTool.Bind(self.FlushGetState,self),"self_getted")
    end
    self:FlushGetState()
end

function ChatHongBaoItem:OnClickItem()
    if self.data.sm_data.self_getted == false then
        if GuildRedbagData.Instance:GetHongBaoGetList().redbag_get_list == nil then
            return
        end
        local view_par = {}
        local info = nil
        for i,v in ipairs(GuildRedbagData.Instance:GetHongBaoGetList().redbag_get_list)  do
            if v.hong_bao_id == self.data.id then
                info = v
                break
            end
        end
        if info ~= nil then
            GuildRedBagCtrl.GetGuildHongBaoByID(self.data.id)
            view_par.info = info
            GuildRedbagData.Instance:SetCurGetHongBaoIdInfo(view_par.info)
            ViewMgr:OpenView(OpenRedbagView)
        end
    else
        GuildRedbagData.Instance:ShowHongBaoDetail(self.data.id)
        ViewMgr:OpenView(OpenRedbagView)
    end
    -- GuildCtrl.GuildHongBaoReq({req_type=OPERA_TYPE.OPERA_TYPE_Get_HONG_BAO,p2=self.data.id})
end

function ChatHongBaoItem:OnEnable()

end

function ChatHongBaoItem:OnDisable()
    if self.self_getted_care ~= nil then
        self.data.sm_data:Uncare(self.self_getted_care)
        self.self_getted_care = nil
    end
    self.data = nil
end

function ChatHongBaoItem:FlushGetState()
    local self_getted = self.data.sm_data.self_getted
    self.Interactor.Interactable = not self_getted
    self.HongBaoKai:SetActive(not self_getted)
    self.HongBaoGuan:SetActive(self_getted)
    UH.SetText(self.GetTag,self_getted and Language.Chat.HongBao.HasGeted or Language.Chat.HongBao.HasntGet)
end

function ChatHongBaoItem.ItemParse(param_t)
    return {
        type = RichTextType.HongBao,
        hb_sys_type = tonumber(param_t[1]), --0.家族红包|1.世界红包
        hb_type = tonumber(param_t[2]),     --1.首充|2.充值|3.充值|4.充值|5.充值|6.占令
        id = tonumber(param_t[3]),
        greeting = param_t[4],
        sm_data = SmartData.New({
            self_getted = false,        --自己是否已领取
        })
    }
end


--------------红包领取消息---------------------

ChatHongBaoGetItem = DeclareMono("ChatHongBaoGetItem")--, RichTextItem)
--ShowTxt=====RichText
function ChatHongBaoGetItem:SetData(data)
    local luck_str = ""
    if data.lucky_type == 1 then
        luck_str = Language.Chat.HongBao.LuckyMax --string.format("<color=#%s>手气王</color>",COLORSTR.Red8)
    elseif data.lucky_type == 2 then
        luck_str = Language.Chat.HongBao.LuckyMin--string.format("<color=#%s>倒霉蛋</color>",COLORSTR.Green9)
    end
    local format_str = luck_str .. string.format(Language.Chat.HongBao.GetTips,data.name, data.num)
    --string.format("<color=#%s>%s</color>抢到了<color=#%s>%d</color>",COLORSTR.Yellow16,data.name,COLORSTR.Yellow16,data.num)
    format_str = format_str .. RichTextHelper.SpriteStrAtlas(tostring(data.item_id),35,35)
    self.ShowTxt.text = format_str--Debuger.TableToString(data)
end


function ChatHongBaoGetItem.ItemParse(param_t)
    return {
        type = RichTextType.HongBaoGet,
        name = param_t[1], --领取到红包玩家的名字
        num = tonumber(param_t[2]),     --领到的数量
        item_id = tonumber(param_t[3]), --领到了哪个货币
        lucky_type = tonumber(param_t[4]) --0,普通;1,手气王;2,倒霉鬼
    }
end