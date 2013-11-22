class Placeable
{
    grid = null;
    x = 0;
    y = 0;

    constructor(_grid, _x, _y)
    {
        grid = _grid;
        x = _x;
        y = _y;
    }

    function setGrid(_grid)
    {
        grid = _grid;
    }

    function setGridPos(_x, _y)
    {
        x = _x;
        y = _y;
    }
};

class Door extends Placeable
{
    constructor(_grid, _x, _y)
    {
        base.constructor(_grid, _x, _y);
    }

    function update(entity, dt)
    {
    }

    function interact()
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));

        local tile = gridComp.getTile(x, y);
        tile.mWire = 128;
        tile.mSignal = 128;
        gridComp.setTile(x, y, tile, -1);
    }
};

class Thruster extends Placeable
{
    constructor(_grid, _x, _y)
    {
        base.constructor(_grid, _x, _y);
    }

    function update(entity, dt)
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local intent = castIntentComponent(entity.getComponent(IntentComponentType));
        local mousePos = intent.getMousePos();

        if (intent.isIntentActive("up"))
            gridComp.setVelocityY(-400);
        if (intent.isIntentActive("down"))
            gridComp.setVelocityY(400);
        if (intent.isIntentActive("left"))
            gridComp.setVelocityX(-200);
        if (intent.isIntentActive("right"))
            gridComp.setVelocityX(200);
    }

    function interact()
    {
    }
};
