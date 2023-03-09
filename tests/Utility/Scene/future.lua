
local idMat3 = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
}

objects =
{
    {
        RenderSurface = {
            loadPath = "sphere.obj",
            position = "{1.277654567, 7.89845841887, 89.712348512867}",
            rotation = idMat3
        },
        NRPhysics = {
            position = "{1.277654567, 7.89845841887, 89.712348512867}",
            vertices = {
                {1.5, 1.2, -1.5},
                {1.7, -1.2, 2.5},
                {15.2, 0.2, -0.7}
            },
            struts = {
                {0, 1},
                {1, 2},
                {0, 2}
            }
        }
    },
    {
        RenderSurface = {
            loadPath = "world.obj",
            position = "{0.0, 0.0, 0.0}",
            rotation = idMat3
        },
        NRPhysics = {
            position = "{0.0, 0.0, 0.0}",
            vertices = {
                {1, 1, -1},
                {-1, 1, -1},
                {-1, -1, -1},
                {1, -1, -1},
                {1, 1, 1},
                {-1, 1, 1},
                {-1, -1, 1},
                {1, -1, 1}
            },
            struts = {
                {0, 1},
                {1, 2},
                {2, 3},
                {0, 3},
                {4, 5},
                {5, 6},
                {6, 7},
                {4, 7}
            }
        }
    }
}
