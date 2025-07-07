 
BattleCinematics = BattleCinematics or DeclareView("BattleCinematics","public_popup/battle_cinematics", 0,ViewLayer.Top)

VIEW_DECLARE_CACHE(BattleCinematics)
VIEW_DECLARE_MASK(BattleCinematics,ViewMask.Block)

--TypeLableBgObj=====GameObject
--TypeLableImg=======UIImageSpriteSet
--DatiClose==========GameObject[]
BattleCinematics.Type = {
	Normal = "Normal",					--普通没有文字
	KaiShiPinTu = "_LocKaiShiPinTu",		--开始拼图
	KaiShiDaTi = "_LocKaiShiDaTi",			--开始答题
	FaZhenWeiJi = "_LocFaZhenWeiJi",			--开始答题
	ZhengYiShiMing = "_LocZhengYiShiMing",		--开始答题
	ShouChaoLaiXi = "_LocShouChaoLaiXi",		--开始答题
	XuanDuZhiWei = "_LocXuanDuZhiWei",			--开始答题
	XiuFuShenZhu = "_LocXiuFuShenZhu",			--开始答题
	MoZhuBaoDong = "_LocMoZhuBaoDong",			--开始答题
}

BattleCinematics.Medal = {
	["_LocFaZhenWeiJi"] = {"_LocDiMu","_LocYi"},
	["_LocZhengYiShiMing"] = {"_LocDiMu","_LocEr"},
	["_LocShouChaoLaiXi"] = {"_LocDiMu","_LocSan"},
	["_LocXuanDuZhiWei"] = {"_LocDiMu","_LocSi"},
	["_LocXiuFuShenZhu"] = {"_LocDiMu","_LocWu"},
	["_LocMoZhuBaoDong"] = {"_LocZui","_LocZhong"},
}
function BattleCinematics:BattleCinematics()

end

function BattleCinematics:LoadCallback()
	local data = PublicPopupData.Instance:BattleCinematicsData()
	for i = 1, self.DatiClose:Length() do
		self.DatiClose[i].enabled = true
	end
	self.MedalTitle:SetActive(false)
	if data.view_type == BattleCinematics.Type.Normal then
		self.TypeLableBgObj:SetActive(false)
		self.TypeLableImg.SpriteName = nil
		self.TypeLableImg:SetObjActive(false)
	elseif data.view_type == BattleCinematics.Type.KaiShiDaTi or 
	data.view_type == BattleCinematics.Type.KaiShiPinTu then
		self.TypeLableBgObj:SetActive(true)
		-- self.TypeLableImg:SetObjActive(true)
		self.TypeLableImg.SpriteName = data.view_type
		for i = 1, self.DatiClose:Length() do
			self.DatiClose[i].enabled = false
		end
	else
		self.TypeLableBgObj:SetActive(true)
		self.TypeLableImg.SpriteName = data.view_type
		self.TitleSpList[1].SpriteName = BattleCinematics.Medal[data.view_type][1]
		self.TitleSpList[2].SpriteName = BattleCinematics.Medal[data.view_type][2]
		self.MedalTitle:SetActive(true)
	end
end

function BattleCinematics:OpenCallback()
	local data = PublicPopupData.Instance:BattleCinematicsData()
	if data.opened_func ~= nil then
		local f = data.opened_func 
		data.opened_func = nil
		f()
	end

	if data.show_time ~= nil and data.show_time > 0 then
		TimeHelper:AddDelayTimer(function () 
			self:closeSelf()
		end,data.show_time)
	else
		self:closeSelf()
	end

	if data.close_func ~= nil and data.close_func_time ~= nil then
		TimeHelper:AddDelayTimer(function () 
			local f = data.close_func 
			data.close_func = nil
			f()
		end,data.close_func_time)
	end
end

function BattleCinematics:closeSelf()
	local data = PublicPopupData.Instance:BattleCinematicsData()
	ViewMgr:CloseView(BattleCinematics)
	if data.close_func ~= nil and data.close_func_time == nil then
		local f = data.close_func 
		data.close_func = nil
		f()
	end	
end

function BattleCinematics:CloseCallback()
	-- local data = PublicPopupData.Instance:BattleCinematicsData()
	-- if data.close_func ~= nil then
	-- 	local f = data.close_func 
	-- 	data.close_func = nil
	-- 	f()
	-- end
	self.TypeLableBgObj:SetActive(false)
	self.TypeLableImg:SetObjActive(false)
end