UIWTextureSetter = UIWTextureSetter or DeclareMono("UIWTextureSetter")
--Image=====RawImage
--Path======String[可为空]
--AutoNative===Bool

function UIWTextureSetter:UIWTextureSetter()
	self.load_state = 0		--0:unload ,1:loading ,2:loaded
end

function UIWTextureSetter:Awake()
	if StringIsEmpty(self.Path) then
		return
	end
	self:SetByPath(self.Path)
end

--设置ui/textures/目录下的子路径
function UIWTextureSetter:SetByPath(tex_path)
	self.Path = tex_path
	self.load_state = 1
	local load_p = string.format("ui/textures/%s",self.Path)
	if self.Image.texture == nil then self.Image.enabled = false end
	ResMgr:LoadObject(load_p,function(tex)
			if self.Path ~= tex_path then
				return
			end
			if self.Image == nil then
				return
			end
			if self.Image:IsDestroyed() then
				return
			end
			if tex == nil then
				self.load_state = 2
				return
			end
			self.Image.texture = tex
			self.load_state = 2
			if self.AutoNative then self.Image:SetNativeSize() end
			self.Image.enabled = true
		end,false)
end

--同步装载贴图，不可用于不在首包里的资源，慎用
function UIWTextureSetter:SetByPathSync(tex_path)
	self.Path = tex_path
	local load_p = string.format("ui/textures/%s",self.Path)
	local asset_id = ResMgr:pathToAssetId(load_p)
	if BundleProviderCtrl.IsBundleCached(asset_id.bundle_name) then
		self.load_state = 1
		self.Image.texture = ResMgr:LoadObjectSync(asset_id.bundle_name, asset_id.asset_name, typeof(UnityEngine.Texture))
		self.load_state = 2
	else
		self:SetByPath(tex_path)
	end
end

function UIWTextureSetter:SetTextureNil()
	self.Image.enabled = false
	self.Image.texture = nil
	self.Path = ""
end

function UIWTextureSetter:TextureIsNil()
	return self.Image.texture == nil
end

function UIWTextureSetter:IsLoaded()
	return self.load_state ~= 1
end