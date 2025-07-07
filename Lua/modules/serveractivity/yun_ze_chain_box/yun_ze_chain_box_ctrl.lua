--复刻->梦泽宝袋 ACTIVITY_TYPE.RAND_SEA_CHAIN_BOX
FILE_REQ("modules/serveractivity/yun_ze_chain_box/yun_ze_chain_box_data")
VIEW_REQ("modules/serveractivity/yun_ze_chain_box/yun_ze_chain_box_view")

YunZeChainBoxCtrl = YunZeChainBoxCtrl or BaseClass(BaseCtrl)
function YunZeChainBoxCtrl:__init()
    if YunZeChainBoxCtrl.Instance ~= nil then
        Debuger.LogError("[YunZeChainBoxCtrl] attempt to create singleton twice!")
        return
    end
    YunZeChainBoxCtrl.Instance = self
    self.data = YunZeChainBoxData.New()

    self:RegisterAllProtocols()
end

function YunZeChainBoxCtrl:__delete()
    if YunZeChainBoxCtrl.Instance == self then
        YunZeChainBoxCtrl.Instance = nil
    end
end

function YunZeChainBoxCtrl:OnInit()
    Remind.Instance:Register(Mod.YunZe.YunZeChainBox, self.data.chain_box_data, function()
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUN_ZE_CHAIN_BOX) and self.data:GetRemindNum() or 0
    end)
end

function YunZeChainBoxCtrl:OnUnloadGameLogic()
end

function YunZeChainBoxCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYunZeBaoDaiInfo, "OnSCRAXianShiBaoDaiInfo")
end

function YunZeChainBoxCtrl:OnSCRAXianShiBaoDaiInfo(protocol)
    self.data:SetChainBoxInfo(protocol)
end
