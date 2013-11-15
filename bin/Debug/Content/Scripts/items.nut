function dirtBlock(grid, x, y)
{
    if (grid.getTile(x, y).mMat == 0)
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
    local entity = engine.getScene().createEntity();
    entity.addComponentSq(TransformComponent(Vector2f(0, y*16), 0, Vector2f(1, 1)));
    entity.addComponentSq(SpriteComponent("robot.png", 1, 1));
    entity.addComponentSq(PlaceableComponent(grid, "Door"));

    return true;
}
