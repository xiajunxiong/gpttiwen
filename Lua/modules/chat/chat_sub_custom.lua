ChatSubCustom = {
    
    
}

local ChannelType = {
        World = 0,
        Current = 1,
        Guild = 3,
        Team = 4,
        TeamCall = 5,
        Prof = 6,
        System = 7,
        BroadCast = 8,
        Mult = 9,
        GroupTalk = 10,
        LandsBetweens = 11,
        Private = 100,
    }

local ChannelName = {
        [ChannelType.World] = "世界",
        [ChannelType.Current] = "当前",
        [ChannelType.Guild] = "家族",
        [ChannelType.Team] = "组队",
        [ChannelType.TeamCall] = "招募",
        [ChannelType.Prof] = "职业",
        [ChannelType.System] = "系统",
        [ChannelType.BroadCast] = "喇叭",
        [ChannelType.Mult] = "综合",
        [ChannelType.GroupTalk] = "群聊",
        [ChannelType.LandsBetweens] = "神域",
        [ChannelType.Private] = "私聊",
    }
-- 聊天上报
-- channle_id     |param
-- 1  -世界       |
-- 2  -当前X      |
-- 3  -家族       |
-- 4  -组队       |
-- 5  -招募X      |
-- 6  -职业       |
-- 7  -系统X      |
-- 8  -喇叭       |1本服2跨服
-- 9  -综合       |
-- 10 -群聊       |群组id
-- 100-私聊       |对方role_id

function ChatSubCustom:AgentChatSub(channel_id, param, content, toName)
    if GLOBAL_CONFIG:Urls().chat_event_url == nil or StringIsEmpty(GLOBAL_CONFIG:Urls().chat_event_url) 
        or GLOBAL_CONFIG:Urls().chat_report_cfg == nil or StringIsEmpty(GLOBAL_CONFIG:Urls().chat_report_cfg) then
        return
    end
    if GLOBAL_CONFIG:Urls().chat_report_cfg == "ManLing" then
        self:manLingChatSub(channel_id, param, content, toName)
    end
end

--漫灵渠道聊天上报
function ChatSubCustom:manLingChatSub(channel_id, param, content, toName)
    local key = GLOBAL_CONFIG:Urls().chat_key
    if key == nil or StringIsEmpty(key) then
        return
    end
    local sub_data = self:getSubDate()
    local form = {}
    form.vcode = bit:_xor(key, sub_data.chatTime * 1000 % 1000000)
    form.debug = 0
    form.userId = sub_data.userId
    form.roleId = sub_data.roleId
    form.roleName = sub_data.roleName-- WebRequester:UrlEncode(sub_data.roleName)
    form.serverName = sub_data.serverName-- WebRequester:UrlEncode(sub_data.serverName)
    form.serverId = sub_data.serverId
    form.level = sub_data.level
    form.vipLevel = sub_data.vipLevel
    form.recharge = sub_data.recharge
    form.chatTime = sub_data.chatTime * 1000
    local chatTo = ChannelName[channel_id] or ""
    if StringIsEmpty(chatTo) then
        LogError("chatTo is empty string channel_id is ",tostring(channel_id))
    end
    if channel_id == ChannelType.Guild then
        chatTo = chatTo.."_"..sub_data.guildId.."_"..sub_data.guildName
    elseif channel_id == ChannelType.Private then
        chatTo = chatTo.."_"..param.."_"..toName
    elseif channel_id ~= ChannelType.World then
        if param ~= nil then
            chatTo = chatTo.."_".. tostring(param)
        end
    end
    form.chatTo = chatTo-- WebRequester:UrlEncode(chatTo)
    form.ip = sub_data.ip
    form.content = content-- WebRequester:UrlEncode(content)
    self:sendSubUrl(GLOBAL_CONFIG:Urls().chat_event_url, form)
end

function ChatSubCustom:sendSubUrl(url, form)
    local call_back = function(data,error)
                if StringIsEmpty(error) then
                    if GLOBAL_CONFIG:Flags().log_print then
                        print("sendSubUrl ",data, form)
                    end
                end
            end
    if form ~= nil then
        WebRequester:Post(url, call_back, form)
    else
        WebRequester:Request(url, call_back)
    end
end

function ChatSubCustom:getSubDate()
    local data = {}
    local role_data = RoleData.Instance:GetBaseData()
    local login_data = LoginCtrl.Instance.data:AccountData()
    data.userId = login_data.key
    data.roleId = role_data.role_id
    data.roleName = role_data.name
    data.serverId = LoginCtrl.Instance.data:GetLastLoginServer()
    data.serverName = LoginCtrl.Instance.data:ServerName()
    data.level = role_data.level
    data.vipLevel = 0
    data.recharge = RechargeData.Instance:GetRechargeNum() / 10 --累计充值金额（元）
    data.chatTime = os.time()
    data.guildId = GuildData.Instance:GetGuildID()
    data.guildName = GuildData.Instance:GetGuildName()
    data.ip = GLOBAL_CONFIG:Urls().client_ip

    return data
end
--contant:文字内容
--chat_scene:需要判断屏蔽词的场景（1、名称，2、消息，3、标题，4、评论，5、签名，6、搜索，7、其他）
--文档地址 https://open.qqgame.qq.com/wiki/24/35/56/57/227/227.html
function ChatSubCustom.PCChatFilter(content, chat_scene, call_back)
    local url_head = GLOBAL_CONFIG:Urls().chat_filter_url
    if StringIsEmpty(url_head) then
        call_back(content)
        return
    end
    if not QQBigPlayerCtrl.IsQQPC() then
        if GLOBAL_CONFIG:PkgInfo().pc_sdk_type == "4399" then
            ChatSubCustom.PCChatFilter4399(content, call_back)
        end
    else
        ChatSubCustom.QQChatFilter(content, chat_scene, call_back)
    end
end

function ChatSubCustom.QQChatFilter(content, chat_scene, call_back)
    local url_head = GLOBAL_CONFIG:Urls().chat_filter_url
    local url_info = QQBigPlayerCtrl.Instance:GetUrlInfo()
    local send_url = Format("%s?openid=%s&openkey=%s&pfkey=%s&content=%s&scene=%s",
        url_head,
        url_info.openid,
        url_info.openkey,
        Nirvana.GameAgent.GetExtraInfo(),
        WebRequester:UrlEncode(content),
        chat_scene)
    WebRequester:Request(send_url,function (data,error)
        if StringIsEmpty(error) then
            local ret, status = TryParseJson(data)
            if status then
                if ret.ret == 0 and ret.text_result_cnt_ > 0 then
                    LogError("QQPCChatFilter ret",ret)
                    call_back(ret.text_result_list_[1].result_text_)
                else
                    LogError("QQPCChatFilter ret.ret is 0:",ret)
                end
            else
                LogError("QQPCChatFilter data",data)
            end
        end
    end)
end

function ChatSubCustom.PCChatFilter4399(content, call_back)
    local sig = GameUtil.StrMd5("699661327cc069b1fb9f1e9b494f7c07" .. content)
    local url = GLOBAL_CONFIG:Urls().chat_filter_url.."?toCheck="..content.."&app=wlshzj&byPinyin=true&sig="..sig
    -- local url = "https://wo.webgame138.com/test/matchService.do?toCheck="..content.."&app=wlshzj&byPinyin=true&sig="..sig
    WebRequester:Request(url,function (data,error)
        if StringIsEmpty(error) then
            local ret, status = TryParseJson(data)
            if status then
                if not TableIsEmpty(ret) then
                    LogError("4399 ret",ret)
                    for _, value in pairs(ret) do
                        local rep_str = ""
                        for i = value.startPos, value.endPos do
                            rep_str = rep_str .. "*"
                        end
                        content = string.gsub(content,value.maskWord,rep_str)
                    end
                    call_back(content)
                else
                    call_back(content)
                end
            end
        else
            LogError("PCChatFilter4399 Error:", error)
        end
    end)
end