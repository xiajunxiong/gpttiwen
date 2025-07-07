
WenXiangLouKeyView = WenXiangLouKeyView or DeclareView("WenXiangLouKeyView", WenXiangLouConfig.ResPath .. "task_wxl_key")
VIEW_DECLARE_MASK(WenXiangLouKeyView, ViewMask.BgBlockNoAnim)

function WenXiangLouKeyView:WenXiangLouKeyView()
	self.data = WenXiangLouCtrl.Instance:Data()
end

function WenXiangLouKeyView:LoadCallback(param_t)
	self.item_seq = param_t.seq
	local co = Cfg.WenXiangLouItemBySeq(self.item_seq)
	if nil == co then return end
	self.Name.text = co.name
	self.Desc.text = co.desc
	self.Pic.SpriteName = co.icon
end

function WenXiangLouKeyView:OnClickConfirm()
	WenXiangLouCtrl.Instance:SendWenXiangLouFindItem(self.item_seq)
	ViewMgr:CloseView(WenXiangLouKeyView)
end