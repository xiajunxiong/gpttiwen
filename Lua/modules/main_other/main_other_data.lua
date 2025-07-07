MainOtherData = MainOtherData or BaseClass()

function MainOtherData:__init()
    if MainOtherData.Instance ~= nil then
        Debuger.LogError("[MainOtherData] attempt to create singleton twice!")
        return
    end
    MainOtherData.Instance = self

    self.right_menu_data = SmartData.New({val = false,data = {}})

    self.choosel_all_data = SmartData.New({val = false})
end

--注册主界面view
function MainOtherData:RegisterViewFunc(key,func)
    if self.register_view_func == nil then
		self.register_view_func = {}
    end
    if self.register_view_func[key] == nil then
        self.register_view_func[key] = func
    end
end

function MainOtherData:MutualViewFunc(key)
    for k,v in pairs(self.register_view_func or {}) do
        if k ~= key then
            v()
        end
    end
end

--主界面一般按钮注册 {key 按钮名字 is_active 是否显示}
function MainOtherData:SetRightMenuData(data)
    self.right_menu_data.data = data
    self.right_menu_data.val = not self.right_menu_data.val
end

function MainOtherData:GetRightMenuData()
    return self.right_menu_data.data or {}
end

--礼包快捷使用按钮
function MainOtherData:QuickUseItem(item)
    if item then
        self.quick_use_item = item
    else
        return self.quick_use_item
    end
end

function MainOtherData:CheckChooselAllData()
    self.choosel_all_data.val = not self.choosel_all_data.val
end