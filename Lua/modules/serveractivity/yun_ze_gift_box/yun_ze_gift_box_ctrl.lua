--复刻->云泽宝盒 ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX
FILE_REQ("modules/serveractivity/yun_ze_gift_box/yun_ze_gift_box_data")
VIEW_REQ("modules/serveractivity/yun_ze_gift_box/yun_ze_gift_box_view")

YunZeGiftBoxCtrl = YunZeGiftBoxCtrl or BaseClass(BaseCtrl)
function YunZeGiftBoxCtrl:__init()
    if YunZeGiftBoxCtrl.Instance ~= nil then
        Debuger.LogError("[YunZeGiftBoxCtrl] attempt to create singleton twice!")
        return
    end
    YunZeGiftBoxCtrl.Instance = self
    self.data = YunZeGiftBoxData.New()

    self:RegisterAllProtocols()
end

function YunZeGiftBoxCtrl:__delete()
    if YunZeGiftBoxCtrl.Instance == self then
        YunZeGiftBoxCtrl.Instance = nil
    end
end

function YunZeGiftBoxCtrl:OnInit()
    --初始化完毕操作
    Remind.Instance:Register(Mod.YunZe.YunZeGiftBox, self.data.gift_box_data, function()
		return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX) and self.data:GetRemindNum() or 0
	end)
end

function YunZeGiftBoxCtrl:OnUnloadGameLogic()
    --切换角色操作
end

function YunZeGiftBoxCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYunZeBaoHeInfo, "OnSCRAXianShiBaoHeInfo")
end

--限时宝盒
function YunZeGiftBoxCtrl:OnSCRAXianShiBaoHeInfo(protocol)
    self.data:SetGiftBoxInfo(protocol)
    -- self.data:SetBonusNum(ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX, protocol.flag_num, #self.data:GetGiftBoxShowList())
end
