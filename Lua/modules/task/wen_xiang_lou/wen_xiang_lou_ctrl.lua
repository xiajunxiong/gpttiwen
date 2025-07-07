
FILE_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_config")
FILE_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_data")

VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_menu_view")
VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_shout_view")
VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_info_view")
VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_key_view")
VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_keys_view")
VIEW_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_cut_view")

WenXiangLouCtrl = WenXiangLouCtrl or BaseClass(BaseCtrl)

function WenXiangLouCtrl:__init()
    if WenXiangLouCtrl.Instance ~= nil then
        Debuger.LogError("[WenXiangLouCtrl] attempt to create singleton twice!")
        return
    end
    WenXiangLouCtrl.Instance = self
    self.data = WenXiangLouData.New()

    self:RegisterAllProtocols()
end

function WenXiangLouCtrl:__delete()
    if WenXiangLouCtrl.Instance == self then
        WenXiangLouCtrl.Instance = nil
    end
    self:UnRegisterAllProtocols()
end

function WenXiangLouCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSWenXiangLouReq)
    self:RegisterProtocol(SCWenXiangLouNpcInfo, "OnWenXiangLouNpcInfo")
    self:RegisterProtocol(SCWenXiangLouReqSceneInfo, "OnWenXiangLouReqSceneInfo")
end

function WenXiangLouCtrl:UnRegisterAllProtocols()
end

function WenXiangLouCtrl:OnWenXiangLouNpcInfo(protocol)
	self.data:SetWenXiangLouNpcInfo(protocol)
end

function WenXiangLouCtrl:OnWenXiangLouReqSceneInfo(protocol)
	self.data:SetWenXiangLouReqSceneInfo(protocol)
end

function WenXiangLouCtrl:SendWenXiangLouReq(param_t)
	local protocol = GetProtocol(CSWenXiangLouReq)
	protocol.oper_type = param_t.oper_type or 0
    protocol.param1 = param_t.param1 or 0
	SendProtocol(protocol)
end

function WenXiangLouCtrl:SendWenXiangLouTalk(seq)
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.talk,
        param1 = seq,
    }
	self:SendWenXiangLouReq(param_t)
end

function WenXiangLouCtrl:SendWenXiangLouFindItem(item_seq)
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.find_item,
        param1 = item_seq,
    }
	self:SendWenXiangLouReq(param_t)
end

function WenXiangLouCtrl:SendWenXiangLouIdentity(seq)
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.identity,
        param1 = seq,
    }
	self:SendWenXiangLouReq(param_t)
end

function WenXiangLouCtrl:SendWenXiangLouFight()
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.fight,
    }
	self:SendWenXiangLouReq(param_t)
end

function WenXiangLouCtrl:SendWenXiangLouNpcInfo()
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.npc_info,
    }
	self:SendWenXiangLouReq(param_t)
end

function WenXiangLouCtrl:SendWenXiangLouSceneInfo()
	local param_t = {
        oper_type = WenXiangLouConfig.OperType.scene_info,
    }
	self:SendWenXiangLouReq(param_t)
end