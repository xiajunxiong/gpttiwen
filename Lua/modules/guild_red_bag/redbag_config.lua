GuildHongBaoConfig = {
    OperaType = {
        SEND_LIST_REQ = 0, --请求可发送列表信息 []
        GET_LIST_REQ = 1, --请求可领取列表信息	 [param_1:红包系统类型
        BASE_INFO_REQ = 2,	--请求单个红包的详细信息	[param_1:红包系统类型，param_2:红包ID]
        SEND_HONG_BAO = 3,  --发红包	[param_1:红包系统类型，param_2:红包类型]
        GET_HONG_BAO = 4,  --领红包	[param_1:红包系统类型，param_2:红包ID]
    },
    SysType = {
        GUILD = 0,   --家族红包
        MAX = 1, 
    },
    Type = {     ---红包类型ICON
        ShouChong = 1,   ---首充
        ChongZhi = 2,    ---充值
        ZhanLing = 3,    ---战令
        LeiChong = 4,    ---累充
    },
    IconByType = {  --通过类型获取对应的Icon
       [1] = 1,  
       [2] = 2,   --旧的
       [3] = 2,   --旧的
       [4] = 2,   --旧的
       [5] = 2,   --旧的
       [6] = 3,
       [7] = 2,   --新的7-12
       [8] = 4,   
       [9] = 4,
       [10] = 4,
       [11] = 4,
       [12] = 4,
    },
    TypeSeq = 6, --是不是红包
    DescIndex  = 158, --红包说明
}