
WenXiangLouKeysView = WenXiangLouKeysView or DeclareView("WenXiangLouKeysView", WenXiangLouConfig.ResPath .. "task_wxl_keys")
VIEW_DECLARE_MASK(WenXiangLouKeysView, ViewMask.BgBlockNoAnim)

function WenXiangLouKeysView:WenXiangLouKeysView()
	self.data = WenXiangLouCtrl.Instance:Data()
end

function WenXiangLouKeysView:LoadCallback()
	self.KeysShow.text = self.data:GetKeysShow()
end

function WenXiangLouKeysView:OnFlush()
	self.cur_page = 1
	self.max_page = math.ceil(self.ItemPos.sizeDelta.y / WenXiangLouConfig.KEYS_PAGE_HEIGHT)
	self:FlushShow()
end

function WenXiangLouKeysView:FlushShow()
	self.ArrowLeft:SetActive(self.ContentPos.localPosition.y > 0)
	self.ArrowRight:SetActive(self.ItemPos.sizeDelta.y - self.ContentPos.localPosition.y > WenXiangLouConfig.KEYS_PAGE_HEIGHT)
	self.PageNum.text = string.format("%s/%s", self.cur_page, self.max_page)
end

function WenXiangLouKeysView:OnClickArrow(val)
	local pos = self.ContentPos.localPosition
	self.ContentPos.localPosition = Vector2.New(pos.x, pos.y + val * WenXiangLouConfig.KEYS_PAGE_HEIGHT)
	self.cur_page = self.cur_page + val
	self:FlushShow()
end

function WenXiangLouKeysView:OnClickClose()
	ViewMgr:CloseView(WenXiangLouKeysView)
end