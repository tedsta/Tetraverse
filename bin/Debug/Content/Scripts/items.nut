function dirtBlock(grid, x, y)
{
    if (grid.canPlace(x, y, 1, 1))
    {
        local dirt = Tile();
        dirt.mMat = 1;
        grid.setTile(x, y, dirt, -1);
        return true;
	}

    return false;
}

function crowbar(grid, x, y)
{
    if (grid.getTile(x, y).mMat != 0)
    {
        local empty = Tile();
        grid.setTile(x, y, empty, -1);
        return true;
	}

    return false;
}

function door(grid, x, y)
{
    if (grid.canPlace(x, y, 4, 6) == true)
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("robot.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(grid, "Door", x, y, 4, 6));
        grid.addPlaceable(entity);
        return true;
    }

    return false;
}

function water(grid, x, y)
{
    local water = Tile();
    water.mMat = 4;
    water.mFluid = 1;
    grid.setTile(x, y, water, -1);
    return true;

    return false;
}
