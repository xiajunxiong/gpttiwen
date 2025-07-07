FILE_REQ("modules/serveractivity/flower_touch/flower_touch_data")
VIEW_REQ("modules/serveractivity/flower_touch/flower_touch_view")

ActFlowerTouchCtrl = ActFlowerTouchCtrl or BaseClass(BaseCtrl)

function ActFlowerTouchCtrl:__init()
    if ActFlowerTouchCtrl.Instance ~= nil then
        Debuger.LogError("[ActFlowerTouchCtrl] attempt to create singleton twice!")
        return
    end
    ActFlowerTouchCtrl.Instance = self
    self.data = ActFlowerTouchData.New()

    self:RegisterProtocol(SCRAYunZeBloomingRecordInfo, "OnSCRAYunZeBloomingRecordInfo")
    -- self:RegisterProtocol(SCRAZhaoCaiJinBaoDrawResult, "OnSCRAZhaoCaiJinBaoDrawResult")
   
	Remind.Instance:Register(Mod.RandActivity.FlowerTouch, self.data.flower_info, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD) then
			return 0
		end
		return self.data.red_info.val and 1 or 0
	end)
end

function ActFlowerTouchCtrl:OnInit()
    self.data:initConfig()
end

function ActFlowerTouchCtrl:OnSCRAYunZeBloomingRecordInfo(protocol)
    self.data:OnFlowerInfo(protocol)    
end
