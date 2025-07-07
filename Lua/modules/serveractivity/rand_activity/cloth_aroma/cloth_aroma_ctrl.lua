FILE_REQ("modules/serveractivity/rand_activity/cloth_aroma/cloth_aroma_data")
VIEW_REQ("modules/serveractivity/rand_activity/cloth_aroma/cloth_aroma_view")
VIEW_REQ("modules/serveractivity/rand_activity/cloth_aroma/cloth_aroma_buy_tips")

ClothAromaCtrl = ClothAromaCtrl or BaseClass(BaseCtrl)
function ClothAromaCtrl:__init()
	if ClothAromaCtrl.Instance ~= nil then
		Debuger.LogError("[ClothAromaCtrl] attempt to create singleton twice!")
		return
	end	
	ClothAromaCtrl.Instance = self
	self.data = ClothAromaData.New()

	self:RegisterAllProtocols()
end

function ClothAromaCtrl:OnInit()
	
end

function ClothAromaCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function ClothAromaCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAJinYiTianXiang,"RevcJinYiTianXiang")
end 

function ClothAromaCtrl:RevcJinYiTianXiang(protocol)
	-- LogDG("平哥哥@这是天香的下发哦？4790",protocol)
	self.data:SetInfo(protocol)
end

function ClothAromaCtrl:SendInfo()
	-- LogDG("平哥哥@这是请求信息哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, 0)
end

function ClothAromaCtrl:SendBuy()
	-- LogDG("平哥哥@这是请求直购哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, 1)
end

function ClothAromaCtrl:SendDraw()
	-- LogDG("平哥哥@这是请求抽奖哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, 2)
end

function ClothAromaCtrl:SendOtherBuy()
	-- LogDG("平哥哥@这是其它购买")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, 3)
end
