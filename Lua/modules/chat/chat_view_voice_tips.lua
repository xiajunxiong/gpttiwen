
ChatViewVoiceTips = ChatViewVoiceTips or DeclareView("ChatViewVoiceTips", ChatConfig.ResPath .. "chat_voice_tips")

function ChatViewVoiceTips:ChatViewVoiceTips()
	self.data = ChatCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.language = Language.Chat
end

function ChatViewVoiceTips:OnFlush()
	local value = self.data.is_voice_up
	self.VoicingObj:SetActive(not value)
	self.CancelObj:SetActive(value)
end

function ChatViewVoiceTips:OnClickBlock()
	if ViewMgr:IsOpen(ChatView) then
		self.data:VoiceFinish(function (rec_key,time) 
			ChatCtrl.Instance:SendChannelVoice(rec_key,time)
			ChatData.Instance:SendGrey()
		end)
	elseif ViewMgr:IsOpen(SocietyView) then
		self.data:VoiceFinish(function (rec_key,time)
			local content = string.format("%s;%d",rec_key,Mathf.Round(time))
			if SocietyConfig.FriendGroupType.group == self.society_data.friend_cur_sel_group.group_type and SocietyConfig.SocietyType.master_appearance ~= self.society_data.cur_sel_tab then
				ChatCtrl.Instance:SendChannelVoice(rec_key, time, ChatConfig.ChannelType.group, self.society_data.cur_sel_chat_group:GroupId())
			else
				self.society_data:AddChatInfo(content, nil, ChatConfig.ChatContentType.audio)
				ViewMgr:FlushView(SocietyView, "SocietyViewChat/SocietyViewChatRecordList")
			end
		end)
	else
		self.data:VoiceFinish()
	end
end