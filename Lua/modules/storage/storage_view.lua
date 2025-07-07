
FILE_REQ("modules/storage/storage_view_storage")
FILE_REQ("modules/storage/storage_view_bag")

StorageView = StorageView or DeclareView("StorageView", StorageConfig.ResPath .. "storage")
VIEW_DECLARE_LEVEL(StorageView,ViewLevel.L1)

function StorageView:StorageView()
	self.language = Language.Storage
end

function StorageView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.Storage)

	self.Currency:DefaultSet()
end