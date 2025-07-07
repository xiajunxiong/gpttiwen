SwornConfig = {
	ResPath = "society/sworn/",

	DECL_WORD_MAX = 100,		--自荐宣言纳新宣言最大字数
	SWORN_DECL_WORD_MAX = 40,	--结义宣言最大字数
	CAN_KICK_OUT_TIME = 86400 * 2, --可踢人时间
	
	--义结金兰确认
	JIE_YI_REQ_RET_TYPE = {
		TYPE_CONFIRM = 0,		--结义确认
		TYPE_CANCEL = 1,		--结义取消
		TYPE_NAME_FINISH = 2,	--结义命名完成
		TYPE_LEAVE = 3,			--离开结义组	p1: 0取消 1离开
		TYPE_KICK = 4,			--踢结义组成员	p1: role_id
		TYPE_INVITE = 5,		--邀请加入		p1: role_id
		TYPE_INVITE_APPLY = 6,	--邀请回复		p1: 4211协议zu_id	p2:是否同意
		TYPE_ZU_INFO = 7,		--请求结义组信息 p1:zu_id --返回4207协议
		TYPE_TAN_HE = 8,		--弹劾
		TYPE_TAN_HE_VOTE = 9,	--弹劾投票	p1 role_id
	}
}
