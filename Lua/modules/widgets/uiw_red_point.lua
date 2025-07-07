-------------------------------------------------
--ui_red_point 控件，通用红点
-------------------------------------------------
UiRedPoint = UiRedPoint or BaseClass()

local red_point_path =  "ui/widgets/red_point.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            red_point_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("red_point register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false)
    end)

--创建对象实例,父对象,位置 默认就是大
function UiRedPoint:__init(root_node, v3, big, ui)
    self.root_obj = GameObjPool:Get(red_point_path, root_node)
    self.mono_com = self.root_obj:GetLuaComponent("UIWRedPoint")
    v3 = v3 or Vector3.zero
    UH.LocalPos(self.root_obj.transform, v3)
   --[[  if big == false then
        self.mono_com:SetSize(big)
    end ]]
    self.mono_com.ui_sp = ui
end

function UiRedPoint:__delete()
    GameObjPool:Release(self.root_obj)
	self.root_obj = nil
end

-- 1红点， 0不显示， 大于0显示数字(暂不支持)
function UiRedPoint:SetNum(num)
    self.num = num
    self.mono_com:SetNum(num)
end

--获取红点数量
function UiRedPoint:GetNum()
    return self.num or 0
end

UIWRedPoint = UIWRedPoint or DeclareMono("UIWRedPoint")
function UIWRedPoint:UIWRedPoint()
    self.num = 0
    self.SpName = {
        "XinXiDian_Xiao",
        "XinXiDian_Da",
    }
end
function UIWRedPoint:SetNum(num)
    self.num = num
    if self.ui_sp == 1 then
        self.PointM:SetActive(self.num > 0)
    else
        self.Point:SetActive(self.num > 0)
    end
end
function UIWRedPoint:SetSize()
    UH.SpriteName(self.PointSp, self.SpName[1])
end

--缓存红点
UIWRedPoint2 = UIWRedPoint2 or DeclareMono("UIWRedPoint2")
function UIWRedPoint2:UIWRedPoint2()
end

function UIWRedPoint2:SetNum(num, use_main)
    self.num = num
    if use_main and use_main== true then
        self.PointMain:SetActive(self.num > 0)
    else
        self.PointPublic:SetActive(self.num > 0)
    end
end

function UIWRedPoint2:GetNum()
    return self.num or 0
end