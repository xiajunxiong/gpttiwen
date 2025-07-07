ViewMasker = ViewMasker or DeclareMono("ViewMasker")
---Bg==========Image
---BgCanvas====Canvas
---Block=======GameObject
---BlockCanvas=======Canvas



function ViewMasker:ViewMasker()
	self.bg_mask_view_tab = nil
	self.block_view_tab = nil
	self.click_close = false
end


function ViewMasker:OnClick()
	if self.click_close then
		if ViewMgr:IsOpen(self.block_view_tab) then
			ViewMgr:CloseView(self.block_view_tab)
		end
	end
end

---
function ViewMasker:BgMask(view_tab,sorting_order)
	if view_tab == self.bg_mask_view_tab then
		return
	end
	if view_tab == nil then
		-- if self.bg_mask_view_tab ~= nil then --from to mask
			self.Bg.enabled = false
			self.Anim:Play("mask_hide")
		-- end
	else
		-- if self.bg_mask_view_tab == nil then --from from no mask
			self.Bg.enabled = ViewMasker.ShowBg(view_tab.block_mask)
			if view_tab.block_mask ~= ViewMask.BgBlockNoAnim and view_tab.block_mask ~= ViewMask.BgBlockCloseNoAnim
				and ViewMasker.ShowBg(view_tab.block_mask)
				 then
				self.Anim:Play("mask_show")
			end
		-- end
		-- self.BgObj:SetParent(parent)
		-- if sibling ~= nil and sibling >= 0 then
		-- 	self.BgObj:SetSiblingIndex(sibling)
		-- else
		-- 	self.BgObj:SetAsLastSibling()
		-- end
		self.BgCanvas.sortingOrder = sorting_order - 1
	end
	self.bg_mask_view_tab = view_tab
end

function ViewMasker:BlockMask(view_tab,blank_close,sorting_order)
	if self.block_view_tab == nil then
		self.Block:SetActive(true)
	end
	self.click_close = blank_close
	self.block_view_tab = view_tab
	self.BlockCanvas.sortingOrder = sorting_order - 1
	-- self.Block:SetParent(parent)
	-- if sibling == nil or sibling < 0  then
	-- 	self.Block:SetAsLastSibling()
	-- else
	-- 	self.Block:SetSiblingIndex(sibling)
	-- end
end


function ViewMasker:CloseBlock()
	if self.block_view_tab ~= nil then
		self.Block:SetActive(false)
		self.block_view_tab = nil
	end
end

function ViewMasker:BlockViewTab()
	return self.block_view_tab
end

function ViewMasker:BgMaskViewTab()
	return self.bg_mask_view_tab
end

function ViewMasker.IsBlock(mask)
	return mask ~= nil and mask ~= ViewMask.None
end

function ViewMasker.ShowBg(mask)
	return mask == ViewMask.BgBlock or mask == ViewMask.BgBlockClose or mask == ViewMask.BgBlockNoAnim or mask == ViewMask.BgBlockCloseNoAnim
end

function ViewMasker.BlankClose(mask)
	return mask == ViewMask.BlockClose or mask == ViewMask.BgBlockClose or mask == ViewMask.BgBlockCloseNoAnim
end

function ViewMasker:BgVisible(value)
	self.Bg.enabled = value
end