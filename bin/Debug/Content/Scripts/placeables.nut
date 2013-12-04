class Placeable
{
    grid = null;
    entity = null;
    x = 0;
    y = 0;

    constructor(_entity, _grid, _x, _y)
    {
        entity = _entity;
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
    on = true;

    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);
    }

    function update(entity, dt)
    {
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));
        if (signal.hasSignal())
        {
            local sig = signal.getInt();
            if (sig == 0)
            {
                print("off\n")
                on = false;
            }
            else if (sig == 1)
            {
                print("on\n")
                on = true;
            }
        }
    }

    function interact()
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));

        on = !on;
        if (on)
            signal.fireInt(1);
        else
            signal.fireInt(0);

        local tile = gridComp.getTile(x, y);
        tile.mWire = 128;
        tile.mSignal = 128;
        gridComp.setTile(x, y, tile, -1);
    }
};

class Thruster extends Placeable
{
    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);
    }

    function update(entity, dt)
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local intent = castIntentComponent(entity.getComponent(IntentComponentType));
        local physics = castPhysicsComponent(entity.getComponent(PhysicsComponentType));
        local mousePos = intent.getMousePos();

        if (intent.isIntentActive("up"))
            physics.getBody().setVelocityY(-400);
        if (intent.isIntentActive("down"))
            physics.getBody().setVelocityY(400);
        if (intent.isIntentActive("left"))
            physics.getBody().setVelocityX(-200);
        if (intent.isIntentActive("right"))
            physics.getBody().setVelocityX(200);
    }

    function interact()
    {
    }
};
