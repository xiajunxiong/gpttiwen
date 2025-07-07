
-- 获取邮件列表
CSMailGetList = CSMailGetList or BaseClass(BaseProtocolStruct)
function CSMailGetList:__init()
	self.msg_type = 9552
end

function CSMailGetList:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 请求读取邮件内容
CSMailRead = CSMailRead or BaseClass(BaseProtocolStruct)
function CSMailRead:__init()
	self.msg_type = 9553
end

function CSMailRead:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.mail_index)
end

-- 领取邮箱
CSMailFetchAttachment = CSMailFetchAttachment or BaseClass(BaseProtocolStruct)
function CSMailFetchAttachment:__init()
	self.msg_type = 9554
end

function CSMailFetchAttachment:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.mail_index)
	MsgAdapter.WriteInt(self.item_index)
	MsgAdapter.WriteInt(self.is_last)
end

-- 删除邮件
CSMailDelete = CSMailDelete or BaseClass(BaseProtocolStruct)
function CSMailDelete:__init()
	self.msg_type = 9551
end

function CSMailDelete:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.mail_index)
end

-- 一键提取附件
CSMailOneKeyFetchAttachment = CSMailOneKeyFetchAttachment or BaseClass(BaseProtocolStruct)
function CSMailOneKeyFetchAttachment:__init()
	self.msg_type = 9556
end

function CSMailOneKeyFetchAttachment:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 一键删除
CSMailClean = CSMailClean or BaseClass(BaseProtocolStruct)
function CSMailClean:__init()
	self.msg_type = 9555
end

function CSMailClean:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

local function ReadMailStatus(t)
	-- t.sender_uid = MsgAdapter.ReadInt()
	-- t.sender_name =  MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.mail_index = MsgAdapter.ReadInt()
	t.has_attchment = MsgAdapter.ReadShort()
	t.subject_length = MsgAdapter.ReadShort()
	t.recv_time = MsgAdapter.ReadUInt()
	t.kind = MsgAdapter.ReadChar()
	t.is_read = MsgAdapter.ReadChar()
	t.is_lock = MsgAdapter.ReadChar()
	t.is_fetch = MsgAdapter.ReadChar()
	-- t.sender_level = MsgAdapter.ReadChar()
	-- t.sender_avatar = MsgAdapter.ReadChar()
	-- MsgAdapter.ReadShort()
	t.mail_subject = MsgAdapter.ReadStrN(t.subject_length)
end

-- 下发邮件列表
SCMailListAck = SCMailListAck or BaseClass(BaseProtocolStruct)
function SCMailListAck:__init()
	self.msg_type = 9504
end

function SCMailListAck:Decode()
  	self.is_init = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadShort()
	self.mail_info_list = {}
	for i = 1, self.count do
		local t = {}
		t.sort_index = i
		-- t.mail_index = MsgAdapter.ReadInt()
		-- t.has_attchment = MsgAdapter.ReadInt()
		ReadMailStatus(t)
		-- t.mail_subject = MsgAdapter.ReadStrN(MAIL_SUBJECT)
		self.mail_info_list[i] = t
	end
end

-- 下发邮件内容
SCMailDetailAck = SCMailDetailAck or BaseClass(BaseProtocolStruct)
function SCMailDetailAck:__init()
	self.msg_type = 9505
end

function SCMailDetailAck:Decode()
	local t = {}
	t.mail_index = MsgAdapter.ReadShort()
	t.mail_reason = MsgAdapter.ReadShort()
	
	t.mail_subject = MsgAdapter.ReadStrN(MAIL_SUBJECT)
	t.content_txt = MsgAdapter.ReadStrN(MAIL_CONTENT_TXT)
	
	t.count = MsgAdapter.ReadInt()
	t.item_list = {}
	for i = 1, t.count do
		t.item_list[i] = {}
		ItemDataWrapper(t.item_list[i],true)
	end

	t.virtual_item_list = {}
	for i = 1, #SocietyConfig.MailVirtualItemId do
		t.virtual_item_list[i] = MsgAdapter.ReadInt()
	end

	self.mail_detail = t
end

-- 领取附件
SCFetchAttachmentAck = SCFetchAttachmentAck or BaseClass(BaseProtocolStruct)
function SCFetchAttachmentAck:__init()
	self.msg_type = 9506
end

function SCFetchAttachmentAck:Decode()
	local t = {}
	t.mail_index = MsgAdapter.ReadInt()
	t.item_index = MsgAdapter.ReadInt()
	t.ret = MsgAdapter.ReadInt()
	self.fetch_info = t
end

local mail_sort = {
	recv_time = 0,
	sort_index = 1,
}

-- 新邮件通知
SCRecvNewMail = SCRecvNewMail or BaseClass(BaseProtocolStruct)
function SCRecvNewMail:__init()
	self.msg_type = 9507
end

function SCRecvNewMail:Decode()
	local t = {}
	-- t.mail_index = MsgAdapter.ReadInt()
	-- t.has_attchment = MsgAdapter.ReadInt()
	ReadMailStatus(t)
	-- t.mail_subject = MsgAdapter.ReadStrN(MAIL_SUBJECT)

	mail_sort.sort_index = mail_sort.recv_time == t.recv_time and (mail_sort.sort_index + 1) or 1
	mail_sort.recv_time = t.recv_time
	t.sort_index = mail_sort.sort_index
	
	self.mail_info = t
end

-- 删除邮件返回
SCMailDeleteAck = SCMailDeleteAck or BaseClass(BaseProtocolStruct)
function SCMailDeleteAck:__init()
	self.msg_type = 9501
end

function SCMailDeleteAck:Decode()
  self.mail_index = MsgAdapter.ReadInt()
  self.ret = MsgAdapter.ReadInt()			-- 0 成功
end