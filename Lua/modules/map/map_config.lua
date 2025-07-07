
MapConfig = {
    ResPath = "map/",

    IRREGULAR_ID_LIST_NUM_MAX = 2,

    LocalW = {
        png_path = "map/world/%s.png",
        num_max = 24,
        half_width = 280,
        half_height = 280,
    },

    LocalL = {
        png_path = "map/local/%s.png",
    },

    LocalT = {
        half_width = 78,
        half_height = 78,
    },

    Mini = {
        width = 450,
        height = 300,
        max_size_w = 3360,
        max_size_h = 2240,
        png_path = "map/mini/%s.png"
    },
    IconColors = {
        [0] = "<color=#"..COLORSTR.Blue10..">%s</color>",
        [1] = "<color=#"..COLORSTR.Yellow13..">%s</color>",
        [2] = "<color=#"..COLORSTR.Yellow16..">%s</color>",
    },

    IconTypeChagnge = {
        [0] = 0,
        [1] = 1,
        [2] = 0,
        [3] = 2
    },

    GuideState = {
        hide = 1,
        show = 2,
        guide = 3,
    }
}