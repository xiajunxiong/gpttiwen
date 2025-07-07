--赠送鲜花
SocietyViewFlower = SocietyViewFlower or DeclareView("SocietyViewFlower", SocietyConfig.ResPath .. "society_flower")

function SocietyViewFlower:SocietyViewFlower()
	self.data = SocietyCtrl.Instance:Data()
	self.handle = BagData.Instance:Care(BindTool.Bind(self.OnFlush, self))
end
 
function SocietyViewFlower:LoadCallback()
	self.info = self.data.cur_sel_role.info
	self.RoleName.text = self.info.name
	UH.SetAvatar(self.Head, self.info.avatar_type,self.info.avatar_id,self.info.avatar_quality)
	UH.SetText(self.Level,Format("Lv.%s",self.info.level))
	self:OnFlush()
end

function SocietyViewFlower:OnFlush()
	local data = self.data:GetFriendFlowerData()
	local num = BagData.Instance:GetNumByItemId(data.item_id)
	local item = Item.Init(data.item_id,num)
	self.FlowerCell:SetData(item)
	UH.SetText(self.FlowerName,item:Name())
end

function SocietyViewFlower:CloseCallback()
    BagData.Instance:UnCare(self.handle)
end

function SocietyViewFlower:OnClickClose()
	ViewMgr:CloseView(SocietyViewFlower)
end

function SocietyViewFlower:OnClickSend()
	local data = self.data:GetFriendFlowerData()
	local num = BagData.Instance:GetNumByItemId(data.item_id)
	if num == 0 then
		PublicPopupCtrl.Instance:Center(Language.Society.SendFlowerTip)
		return
	end
	SocietyCtrl.Instance:SendFlowerReq(self.info.uid,data.seq)
end