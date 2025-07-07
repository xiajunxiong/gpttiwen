FILE_REQ("modules/ling_bao/ling_bao_config")
FILE_REQ("modules/ling_bao/ling_bao")
FILE_REQ("modules/ling_bao/ling_bao_data")

VIEW_REQ("modules/ling_bao/ling_bao_view")
VIEW_REQ("modules/ling_bao/ling_bao_resonance")
VIEW_REQ("modules/ling_bao/ling_bao_bag")
VIEW_REQ("modules/ling_bao/ling_bao_resolve")
VIEW_REQ("modules/ling_bao/ling_bao_resolve_reward")
VIEW_REQ("modules/ling_bao/ling_bao_awake")
VIEW_REQ("modules/ling_bao/sheng_hen_skill")
VIEW_REQ("modules/ling_bao/sheng_hen_unlock")
VIEW_REQ("modules/ling_bao/ling_bao_get")
VIEW_REQ("modules/ling_bao/ling_bao_item_info")
VIEW_REQ("modules/ling_bao/ling_bao_str_up")
VIEW_REQ("modules/ling_bao/ling_bao_lv_up")
VIEW_REQ("modules/ling_bao/ling_bao_get_way")

LingBaoCtrl = LingBaoCtrl or BaseClass(BaseCtrl)

function LingBaoCtrl:__init()
    if LingBaoCtrl.Instance ~= nil then
        Debuger.LogError("[LingBaoCtrl] attempt to create singleton twice!")
        return
    end
    LingBaoCtrl.Instance = self
    self.data = LingBaoData.New()
end

function LingBaoCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
    FunOpen.Instance:SelfCondition(Mod.LingBao, function()
		return self.data:IsOpen()
    end)
    FunOpen.Instance:SelfCondition(Mod.LingBao.Main, function()
		return self.data:IsOpen()
	end)
end

function LingBaoCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.LingBao.Main,
        self.data.item_list,
        remind_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.LingBao.Main,
        self.data.equip_list,
        remind_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.LingBao.Main,
        self.data.info_data,
        remind_func
    )
    BagData.Instance:ListenItems(self.data:LingBaoRemindItemList(), function()
		self.data.info_data:Notify()
	end)
end

function LingBaoCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSLingBaoSystemReq)
    self:RegisterProtocol(SCLingBaoBagInfoAck, "OnSCLingBaoBagInfoAck")
    self:RegisterProtocol(SCLingBaoBagSingleInfo, "OnSCLingBaoBagSingleInfo")
    self:RegisterProtocol(SCLingBaoSystemShengHenWearInfo, "OnSCLingBaoSystemShengHenWearInfo")
    self:RegisterProtocol(SCLingBaoSystemShengHenWearSingleInfo, "OnSCLingBaoSystemShengHenWearSingleInfo")
    self:RegisterProtocol(SCLingBaoSystemShengHenAllInfo, "OnSCLingBaoSystemShengHenAllInfo")
    self:RegisterProtocol(SCLingBaoSystemShengHenPartInfo, "OnSCLingBaoSystemShengHenPartInfo")
    self:RegisterProtocol(SCLingBaoSystemRelatedNoitce, "OnSCLingBaoSystemRelatedNoitce")
    self:RegisterProtocol(CSLingBaoSystemDecomposeReq)
    self:RegisterProtocol(CSLingBaoSystemComposeReq)
end

function LingBaoCtrl:__delete()
    if LingBaoCtrl.Instance == self then
        LingBaoCtrl.Instance = nil
    end
end

function LingBaoCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function LingBaoCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

function LingBaoCtrl:SendReq(req_type, param1, param2)
    local protocol = ProtocolPool.Instance:GetProtocol(CSLingBaoSystemReq)
    protocol.req_type = req_type or LingBaoReqType.AllInfo
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    self:MyLogError(ColorStr("灵宝系统请求"..protocol.msg_type, COLORSTR.Red8), protocol)
    SendProtocol(protocol)
end

function LingBaoCtrl:OnSCLingBaoBagInfoAck(protocol)
    self:MyLogError(ColorStr("灵宝系统背包所有信息下发（登陆下发）"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetBagInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoBagSingleInfo(protocol)
    self:MyLogError(ColorStr("灵宝系统背包单条信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetBagSingleInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoSystemShengHenWearInfo(protocol)
    self:MyLogError(ColorStr("灵宝系统所有圣痕佩戴信息下发（登陆下发）"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetEquipListInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoSystemShengHenWearSingleInfo(protocol)
    self:MyLogError(ColorStr("灵宝系统圣痕佩戴单条信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetEquipListSingleInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoSystemShengHenAllInfo(protocol)
    self:MyLogError(ColorStr("灵宝系统圣痕所有信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetShengHenInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoSystemShengHenPartInfo(protocol)
    self:MyLogError(ColorStr("灵宝系统单个圣痕中所有灵宝的强化信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetShengHenStrSingleInfo(protocol)
end

function LingBaoCtrl:OnSCLingBaoSystemRelatedNoitce(protocol)
    self:MyLogError(ColorStr("灵宝系统相关通知下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetNotifyInfo(protocol)
end

function LingBaoCtrl:SendResolve(list)
    local protocol = ProtocolPool.Instance:GetProtocol(CSLingBaoSystemDecomposeReq)
    protocol.list = list
    SendProtocol(protocol)
    self:MyLogError(ColorStr("灵宝系统灵宝分解请求"..protocol.msg_type, COLORSTR.Red8), protocol)
end

function LingBaoCtrl:SendLingBaoLvUp(shenghen_type, index_list)
    local protocol = ProtocolPool.Instance:GetProtocol(CSLingBaoSystemComposeReq)
    protocol.shenghen_type = shenghen_type
    protocol.index_list = index_list
    SendProtocol(protocol)
    self:MyLogError(ColorStr("灵宝系统灵宝合成(升星/升阶)请求"..protocol.msg_type, COLORSTR.Red8), protocol)
end

function LingBaoCtrl:MyLogError(...)
    -- if IS_EDITOR then
    --     LogError(...)
    -- end
end

function LingBaoCtrl:GoToGetWay()
    PublicPopupCtrl.Instance:Center("没有神域呢")
end