VIEW_REQ("modules/guild_red_bag/guild_redbag")
VIEW_REQ("modules/guild_red_bag/guild_redbag_tip")
VIEW_REQ("modules/guild_red_bag/redbag_config")
FILE_REQ("modules/guild_red_bag/guild_redbag_data")

GuildRedBagCtrl = GuildRedBagCtrl or BaseClass(BaseCtrl)

function GuildRedBagCtrl:__init()
    if GuildRedBagCtrl.Instance ~= nil then
        Debuger.LogError("[GuildRedBagCtrl] attempt to create singleton twice!")
        return
    end
    GuildRedBagCtrl.Instance = self
    self.data = GuildRedbagData.New()

	----------------家族红包---------------
    self:RegisterProtocol(CSHongBaoClientReq)
    self:RegisterProtocol(SCHongBaoSendList, "RecvSCHongBaoSendList")
    self:RegisterProtocol(SCHongBaoGetList, "RecvSCHongBaoGetList")
    self:RegisterProtocol(SCHongBaoBaseInfo, "RecvSCHongBaoBaseInfo")
    self:RegisterProtocol(SCHongBaoSendListChange, "RecvSCHongBaoSendListChange")
    self:RegisterProtocol(SCHongBaoGetListChange, "RecvSCHongBaoGetListChange")
    self:RegisterProtocol(SCHongBaoBaseInfoChange, "RecvSCHongBaoBaseInfoChange")
end

function GuildRedBagCtrl:FlushHongBaoRemind()
    local num = self.data:CheckApplyCount()
    return num
end

function GuildRedBagCtrl:__delete()
end

function GuildRedBagCtrl:OnUnloadGameLogic()
    self.data:ClearHongBaoGetList()
end

-------------家族红包------------
function GuildRedBagCtrl:RecvSCHongBaoSendList(protocol)
    self.data:SetHongBaoSendList(protocol)
end
function GuildRedBagCtrl:RecvSCHongBaoGetList(protocol)
    self.data:SetHongBaoGetList(protocol)
end
function GuildRedBagCtrl:RecvSCHongBaoBaseInfo(protocol)
    self.data:SetHongBaoGetOneInfo(protocol)
end
function GuildRedBagCtrl:RecvSCHongBaoSendListChange(protocol)
    self.data:SetHongBaoSendListChange(protocol)
end
function GuildRedBagCtrl:RecvSCHongBaoGetListChange(protocol)
    self.data:SetHongBaoGetListChange(protocol)
end
function GuildRedBagCtrl:RecvSCHongBaoBaseInfoChange(protocol)
    self.data:SetHongBaoBaseInfoChange(protocol)
    ChatData.Instance:OnHongBaoBaseInfoChange(protocol) --通知聊天模块有人领取了红包
end
function GuildRedBagCtrl.GuildHongBaoReq(param_t) --家族红包数据请求
    local protocol = GetProtocol(CSHongBaoClientReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.p1 or GuildHongBaoConfig.SysType.GUILD
    protocol.param2 = param_t.p2 or 0
    protocol.greeting = param_t.greeting or 0
    SendProtocol(protocol)
end
function GuildRedBagCtrl.SendGuildHongBao()  --请求可发送红包消息
    GuildRedBagCtrl.GuildHongBaoReq({req_type = GuildHongBaoConfig.OperaType.SEND_LIST_REQ})
end
function GuildRedBagCtrl.GetGuildHongBao()   --请求可领取红包消息
    GuildRedBagCtrl.GuildHongBaoReq({req_type = GuildHongBaoConfig.OperaType.GET_LIST_REQ})
end
function GuildRedBagCtrl.GetGuildHongBaoByID(hongbao_id)  --请求单个红包消息
    if GuildRedBagCtrl.Instance.data.family_redbag_info_cfg[hongbao_id] == nil then ----没有缓存过  请求下发
      GuildRedBagCtrl.GuildHongBaoReq({req_type = GuildHongBaoConfig.OperaType.BASE_INFO_REQ,
         p2 = hongbao_id})
    else
        GuildRedBagCtrl.Instance.data:SetHongBaoGetOneInfoById(hongbao_id)
    end
end
function GuildRedBagCtrl.SendGuildHongBaoByType(hongbao_type,greet)  --发送红包
    GuildRedBagCtrl.GuildHongBaoReq({req_type = GuildHongBaoConfig.OperaType.SEND_HONG_BAO,
     p2 = hongbao_type ,greeting = greet})
end
function GuildRedBagCtrl.ReqGuildHongBaoById(hongbao_id)  --请求领红包
    GuildRedBagCtrl.GuildHongBaoReq({req_type = GuildHongBaoConfig.OperaType.GET_HONG_BAO,
     p2 = hongbao_id})
end