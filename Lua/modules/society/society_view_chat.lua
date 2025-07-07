
SocietyViewChat = SocietyViewChat or DeclareMono("SocietyViewChat", UIWFlushPanelGroup)

function SocietyViewChat:SocietyViewChat()
    self.flush_events = {EventProtocol.ToEventKey(SCShiTuRelationInfo), EventProtocol.ToEventKey(SCFriendIntimacyInfoChange), EventProtocol.ToEventKey(SCShiTuSyncInfo)}
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society
end

function SocietyViewChat:onFlush()
    if nil == self.data.friend_cur_sel_group then return end
    local value = false
    local is_shi_tu = SocietyConfig.SocietyType.master_appearance == self.data.cur_sel_tab
    if is_shi_tu then
        value = nil ~= self.data.cur_sel_role and nil ~= self.data.cur_sel_role.info
    elseif SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type then
        value = nil ~= self.data.cur_sel_chat_group
    else
        value = nil ~= self.data.cur_sel_role
    end
    -- self.ChatObj:SetActive(value or (nil ~= self.data.cur_sel_role and nil ~= self.data.cur_sel_role.info))
    self.ChatObj:SetActive(value)
    self.DownObj:SetActive(value)
    local sel_friend_group = value and SocietyConfig.FriendGroupType.friend == self.data.friend_cur_sel_group.group_type
    self.BtnGiftObj:SetActive(sel_friend_group)
    self.Bg2Pos.sizeDelta = Vector2.New(self.Bg2Pos.sizeDelta.x, is_shi_tu and 450 or (sel_friend_group and 480 or 515))
    self.RecordsPos.sizeDelta = Vector2.New(self.RecordsPos.sizeDelta.x, is_shi_tu and 400 or (sel_friend_group and 465 or 500))
    self.IntimacyShow:SetObjActive(sel_friend_group)
    if sel_friend_group and self.data.cur_sel_role.info ~= nil then
        self.IntimacyShow.text = string.format(self.language.ChatIntimacyShow, self.data:GetFriendIntimacyVal(self.data.cur_sel_role.info.uid))
    end

	local cur_sel_group = self.data.friend_cur_sel_group
	local group_type = cur_sel_group.group_type
    local role_list = self.data:GetFriendListByType(group_type)
    local flag_none_friend = #role_list == 0 and group_type <= 2
	self.NoneObj:SetActive(flag_none_friend and not value)
    self.EmptyObj:SetActive(not value and not flag_none_friend)

    local is_master = self.data:IsMaster()
    local tips = 1
    if SocietyConfig.SocietyType.master_appearance == self.data.cur_sel_tab then tips = is_master and 3 or 2 end
    self.EmptyTips.text = Language.Society.EmptyTips[tips]

    self:FlushGroupNotice()
end

function SocietyViewChat:FlushGroupNotice()
    local bottom = 0
    local is_show = SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and nil ~= self.data.cur_sel_chat_group and self.data.cur_sel_chat_group:NoticeShow()
    is_show = is_show and not StringIsEmpty(self.data.cur_sel_chat_group:GroupNotice())
    if is_show then
        self.GroupNotice.text = string.format(Language.Society.ChatGroupNoticeShow, self.data.cur_sel_chat_group:GroupNotice())
        bottom = 50
    end
    self.GroupNoticeObj:SetActive(is_show)
	self.RecordListPos.offsetMin = Vector2.New(0, bottom)
end

function SocietyViewChat:OnClickCloseNotice()
    self.GroupNoticeObj:SetActive(false)
    self.RecordListPos.offsetMin = Vector2.New(0, 0)
    self.data.cur_sel_chat_group:BlockNoticeShow()
end

function SocietyViewChat:OnClickIntimacyTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[180].desc})
end

SocietyViewChatDetail = SocietyViewChatDetail or DeclareMono("SocietyViewChatDetail", UIWFlushPanel)

function SocietyViewChatDetail:SocietyViewChatDetail()
    self.data = SocietyCtrl.Instance:Data()
    self.chat_data = ChatCtrl.Instance:Data()
	self.world_num_limit = self.chat_data:GetSocietyLimitNum()

    self.data_cares = {
		{data = self.chat_data.chat_quick_input, func = self.FlushInputField, init_call = false},
    }

    self.chat_helper = ChatHelper.New{
        input_field = self.InputField,
        limit_num = self.chat_data:GetSocietyLimitNum()
    }
end

function SocietyViewChatDetail:Awake()
	UIWFlushPanel.Awake(self)
	self.BtnVoiceObj:SetActive(GLOBAL_CONFIG:FlagsVoice())
end

function SocietyViewChatDetail:OnClickSend()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    -- if not FunOpen.Instance:GetFunIsOpened(Mod.Chat.Main) then
    --     PublicPopupCtrl.Instance:Center(ErrorInfo[100037])
    --     return
    -- end
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
    if self.chat_helper:IsLimitNum() then return end
    local str = self.chat_helper:RealText()
    if not self.chat_helper:Switch() and self.chat_data:ChatCacheCheck(str, SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and ChatConfig.ChannelId.group or ChatConfig.ChannelId.single) then
		return
	end
    local function callback(str_f)
        if SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and SocietyConfig.SocietyType.master_appearance ~= self.data.cur_sel_tab then
            ChatCtrl.Instance:SendChannelChatText(str_f, self.chat_helper:ItemList(), ChatConfig.ChannelType.group, self.data.cur_sel_chat_group:GroupId())
            self.chat_helper:Empty()
        elseif nil ~= self.data.cur_sel_role then
            local illegal, tips = self.data:CheckChatIllegal(str)
            if illegal then
                PublicPopupCtrl.Instance:Center(tips)
                return
            end
            self.data:AddChatInfo(str_f, self.chat_helper:ItemList())
            self.chat_helper:Empty()
            ViewMgr:FlushView(SocietyView, "SocietyViewChat/SocietyViewChatRecordList")
            ViewMgr:FlushView(SocietyView, "SocietyViewShiTuInfo/SocietyViewChat/SocietyViewChatRecordList")
        end
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(str, 2,callback)
	else
		callback(ChatFilter.Instance:Filter(str))
	end    
end

function SocietyViewChatDetail:FlushInputField()
	self.chat_helper:AddChatItem(self.chat_data.chat_quick_input:Val())
end

function SocietyViewChatDetail:OnDownVoice()
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
	self.chat_data:VoiceStart()
end

function SocietyViewChatDetail:OnUpVoice()
    self.chat_data:VoiceFinish(function (rec_key,time)
        local content = string.format("%s;%d",rec_key,Mathf.Round(time))
        if SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and SocietyConfig.SocietyType.master_appearance ~= self.data.cur_sel_tab then
            ChatCtrl.Instance:SendChannelVoice(rec_key, time, ChatConfig.ChannelType.group, self.data.cur_sel_chat_group:GroupId())
        else
            self.data:AddChatInfo(content, nil, ChatConfig.ChatContentType.audio)
            ViewMgr:FlushView(SocietyView, "SocietyViewChat/SocietyViewChatRecordList")
        end
    end)
end

function SocietyViewChatDetail:OnDragVoice()
	self.chat_data:VoiceCheck()
end

function SocietyViewChatDetail:OnClickEmoj()
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
    ViewMgr:OpenView(ChatQuickView)
end

function SocietyViewChatDetail:OnClickGift()
    self.data:SetSocietySendInfo(SocietyConfig.ShareType.GodLandTrace, self.data.cur_sel_role.info.uid, 0)
	ViewMgr:OpenView(SocietySendView)
end


SocietyViewChatRecordList = SocietyViewChatRecordList or DeclareMono("SocietyViewChatRecordList", UIWFlushPanel)

function SocietyViewChatRecordList:SocietyViewChatRecordList()
    self.flush_events = {EventProtocol.ToEventKey(SCSingleChat)}

    self.data = SocietyCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.chat_group_channels, func = self.FlushGroupList},
        {data = self.data.send_info_sm, func = self.onFlush, keys = {"flush_msg"}, init_call = false},
    }
end

function SocietyViewChatRecordList:OnEnable()
    UIWFlushPanelGroup.OnEnable(self)

    self:onFlush()
end

function SocietyViewChatRecordList:FlushGroupList()
    if nil ~= self.data.friend_cur_sel_group and SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and SocietyConfig.SocietyType.master_appearance ~= self.data.cur_sel_tab then
        if nil == self.data.cur_sel_chat_group then return end
        local chat_list = self.data:GetGroupChannelChatListByGroupId(self.data.cur_sel_chat_group:GroupId())
        self:FlushRecordList(chat_list)
    end
end

function SocietyViewChatRecordList:onFlush()
    if nil ~= self.data.friend_cur_sel_group and SocietyConfig.FriendGroupType.group == self.data.friend_cur_sel_group.group_type and SocietyConfig.SocietyType.master_appearance ~= self.data.cur_sel_tab then
        if nil == self.data.cur_sel_chat_group then return end
        local chat_list = self.data:GetGroupChannelChatListByGroupId(self.data.cur_sel_chat_group:GroupId())
        self:FlushRecordList(chat_list)
    else
        local chat_list = self.data:GetChatListByUid()
        self:FlushRecordList(chat_list)
    end
end

function SocietyViewChatRecordList:FlushRecordList(chat_list)
    self.RecordList:SetData(chat_list)
end

SocietyViewChatRecordItem = DeclareMono("SocietyViewChatRecordItem", RichTextItem)
--SpeechTxt============Text
--RecTimeTxt===========Text
--SpeechBtnObj=========GameObject
--SpeechWaitObj========GameObject
--RecRootLayout========LayoutElement
--RecRootobj===========GameObject
local rec_max_sec = 30
local rec_min_sec = 3
local rec_min_w = 100
local rec_max_w = 300
local rec_speech_ret_cache = {}

function SocietyViewChatRecordItem:SocietyViewChatRecordItem()
    RichTextHelper.Bind(self.Content, BindTool.Bind(self.OnClickLink, self))
    RichTextHelper.Bind(self.TempGroupNotice, BindTool.Bind(self.OnClickLink, self))
    self.care_handle = ChatData.Instance:RecordData():Care(BindTool.Bind(ChatRecordItem.FlushPlayingKey,self),"cur_playing_key")
end

function SocietyViewChatRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if nil == data then return end
    local info = data.info
    local content = data:GetContent()
    self.is_item = "table" == type(content)
    self.is_notice_g = ChatConfig.MsgType.group_notice == info.msg_type
    self.is_notice_t = ChatConfig.MsgType.temp_group_notice == info.msg_type or ChatConfig.MsgType.intimacy_add_notice == info.msg_type
    self.is_notice = self.is_notice_g or self.is_notice_t
    self.is_pet = self.is_item and content.type == RichTextType.Pet
    self.is_gift = self.is_item and content.type == RichTextType.SocietyGift
    self.is_gift_flower = self.is_item and content.type == RichTextType.SocietyGiftFlower
    self.is_shitu_invite = self.is_item and content.type == RichTextType.SocietyShiTuInvite

    -- self.Time.text = os.date("%Y-%m-%d %X", info.msg_timestamp)
    if self.PeakLevel then
        self.PeakLevel:SetActive(info.top_level and info.top_level > 0)
	    AvatarEffect.SetImageEnabled(self.PeakImg,info.avatar_quality or RoleData.Instance:GetRoleAvatarQuality())
    end
    self.Time.text = info.name
    if data:IsSelf() then
        UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    else
        UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
    end
    if not self.is_item and not self.is_notice and not data:IsRecord() then self.Content.text = content end
    if self.is_pet then self.PetItem:SetData(content, info.item_list[1]) end
    if self.is_gift then self.GiftItem:SetData(content, info, self.GiftPos) end
    if self.is_gift_flower then self.GiftFlowerItem:SetData(content, info, self.GiftPos) end
    if self.is_shitu_invite then self.ShiTuInviteItem:SetData(content, info, self.GiftPos) end
    if self.is_notice_g then self.GroupNotice.text = info.content end
    if self.is_notice_t then
        self.TempGroupNotice.text = self.QuickChat and info.content_quick or info.content
        self.TempGroupNotice.alignment = ChatConfig.MsgType.intimacy_add_notice == info.msg_type and 4 or 3
    end
    -- LogError("data======",data:IsRecord(),data)
    if data:IsRecord() then
        self.rec_key = data:RecordKey()
        local speech_ret = rec_speech_ret_cache[self.rec_key]
        local speech_pw = 0
        if speech_ret ~= nil then
            self.SpeechBtnObj:SetActive(false)
            self.SpeechTxt.text = speech_ret
            self.SpeechTxt:SetObjActive(true)
            speech_pw = self.SpeechTxt.preferredWidth-- + layout_padding_x*2
            -- self.TextSplitDuiFangObj:SetActive(not is_self)
            -- self.TextSplitZiJiObj:SetActive(is_self)
        else
            self.SpeechBtnObj:SetActive(true)
            self.SpeechTxt:SetObjActive(false)
        end
        local rec_time = data:RecordTime()
        local t_min = math.floor(rec_time/60)
        local t_sec = rec_time%60
        if t_min > 0 then
            self.RecTimeTxt.text = string.format("%d'%d''",t_min,t_sec)
        else
            self.RecTimeTxt.text = string.format("%d''",t_sec)
        end
        local w_per = (rec_time - rec_min_sec)/(rec_max_sec-rec_min_sec)
        local root_w = math.min(math.max(speech_pw,Mathf.Lerp(rec_min_w,rec_max_w,w_per)),rec_max_w)
        self.RecRootLayout.preferredWidth = root_w
        self:FlushPlayingKey()
    end
    self:LayoutAdapt()
end

function SocietyViewChatRecordItem:LayoutAdapt()
    local is_self = self.data:IsSelf()
    self.ItemLayout.padding.top = self.is_notice and 5 or 35
	self.ItemLayout.padding.left = (is_self or self.is_notice) and 0 or 110
    self.ItemLayout.padding.right = (not is_self or self.is_notice) and 0 or 110
    self.BgLayout.padding.left = is_self and 15 or 25
	self.BgLayout.padding.right = is_self and 25 or 15
	self.GoObj:SetSiblingIndex(is_self and self.BgObj:GetSiblingIndex() - 1 or self.BgObj:GetSiblingIndex() + 1)

	self.BgSp.SpriteName = is_self and "DuiHuaKuang2" or "DuiHuaKuang1"
	self.UpPos.anchoredPosition = Vector2(is_self and self.UpPosVal or -self.UpPosVal, -47)
	self.DetailPos.anchoredPosition = Vector2(is_self and -55 or 55, 30)
	self.TimePos.pivot = Vector2.New(is_self and 1 or 0, 0.5)
	
    self.PetItem.gameObject:SetActive(self.is_pet)
    self.GiftItem.gameObject:SetActive(self.is_gift)
    self.GiftFlowerItem.gameObject:SetActive(self.is_gift_flower)
    self.ShiTuInviteItem.gameObject:SetActive(self.is_shitu_invite)
    self.NoticeObj:SetActive(self.is_notice_g)
    self.NoticeTObj:SetActive(self.is_notice_t)
	self.GoObj:SetActive(not self.is_item and not self.is_notice)
    self.BgObj:SetActive(not self.is_item and not self.is_notice)
    self.UpPos:SetObjActive(not self.is_notice)
    self.Content:SetObjActive(not self.is_item and not self.data:IsRecord())
    self.RecRootobj:SetActive(self.data:IsRecord())
end

function SocietyViewChatRecordItem:SetObjsActive(objs, active)
    for i = 1, objs:Length() do
        objs[i]:SetActive(active)
    end
end

function SocietyViewChatRecordItem:OnClickPlayRec()
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

function SocietyViewChatRecordItem:OnClickSpeech()
    self.SpeechBtnObj:SetActive(false)
    self.SpeechWaitObj:SetActive(true)
    VoiceRecorder.SpeechToText(self.rec_key,function(com_code,rec_key,ret)
        self.SpeechWaitObj:SetActive(false)
        if com_code == GVoiceComCode.STTSucc then
            rec_speech_ret_cache[rec_key] = ret
            self.SpeechTxt.text = ret
            self.SpeechTxt:SetObjActive(true)
            -- self.TextSplitDuiFangObj:SetActive(not self.is_self)
            -- self.TextSplitZiJiObj:SetActive(self.is_self)
            -- local speech_pw = self.SpeechTxt.preferredWidth-- + layout_padding_x*2
            -- if speech_pw > self.RootTran.sizeDelta.x then
            --     self.RecRootLayout.preferredWidth = math.min(speech_pw,rec_max_w)
            -- end
            -- self.ScrollListLayout:SetLayoutDirty()
        else
            self.SpeechBtnObj:SetActive(true)
            if com_code == GVoiceErrorNo.STTing then
                PublicPopupCtrl.Instance:Center(Language.Chat.SpeechingHint)
            end
        end
    end)
end

function SocietyViewChatRecordItem:FlushPlayingKey()
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

function SocietyViewChatRecordItem:OnDestroy()
    RichTextItem.OnDestroy(self)
    ChatData.Instance:RecordData():Uncare(self.care_handle)
    self.care_handle = nil
    if self.data ~= nil and self.data.IsRecord and self.data:IsRecord() then
        if self:IsEnabled() then
            local cur_playing_key = ChatData.Instance:RecordData().cur_playing_key
            if cur_playing_key == self.rec_key then
                VoiceRecorder.StopPlayRecord()
                ChatData.Instance:SetCurPlayingRec("")
            end
        end
    end
end