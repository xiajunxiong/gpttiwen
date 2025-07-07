
MapViewMini = MapViewMini or DeclareMono("MapViewMini", UIWFlushPanel)

function MapViewMini:MapViewMini()
    self.data = MapCtrl.Instance:Data()
    self.mini = MapConfig.Mini

    self.data_cares = {
        {data = self.data.mini_center_pos, func = self.FlushCenter, init_call = false},
        {data = self.data.mini_view_rect, func = self.ViewRect, init_call = false},
        {data = self.data.local_coord, func = self.FlushMiniRect, init_call = false},
    }
end

function MapViewMini:FlushCenter()
	self.CenterPosition.localPosition = self.data.mini_center_pos
end

function MapViewMini:ViewRect()
    self.MiniViewRect.sizeDelta = self.data.mini_view_rect
    self:FlushMiniRect()
end

function MapViewMini:FlushMiniRect()
    local local_coord = self.data.local_coord
    self.MiniViewRect.localPosition = Vector2.New(-local_coord.x / self.mini.max_size_w * self.mini.width, -local_coord.y / self.mini.max_size_w * self.mini.width)
end