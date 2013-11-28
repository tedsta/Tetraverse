function dirtBlock(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 1))
    {
        local dirt = Tile();
        dirt.mMat = 1;
        grid.setTile(x, y, dirt, -1);
        return true;
	}

    return false;
}

function crowbar(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.getTile(x, y).mMat != 0)
    {
        local empty = Tile();
        grid.setTile(x, y, empty, -1);
        return true;
	}

    return false;
}

function door(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 4, 6) == true)
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("robot.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(gridEnt, "Door", x, y, 4, 6));
        entity.addComponentSq(LightComponent(200));
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);

        return true;
    }

    return false;
}

function thruster(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 1) == true)
    {
        local transform = TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1));
        transform.setOrigin(Vector2f(12-8, 47-8));

        local intent = IntentComponent();
        intent.mapKeyToIntent("up", KeyI, BtnStateDown);
        intent.mapKeyToIntent("down", KeyK, BtnStateDown);
        intent.mapKeyToIntent("left", KeyJ, BtnStateDown);
        intent.mapKeyToIntent("right", KeyL, BtnStateDown);

        local entity = engine.getScene().createEntity();
        entity.addComponentSq(transform);
        entity.addComponentSq(intent);
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/thruster.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(gridEnt, "Thruster", x, y, 1, 1));
        grid.addPlaceable(entity);

        return true;
    }

    return false;
}

function water(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    local water = Tile();
    water.mMat = 4;
    water.mFluid = 1;
    grid.setTile(x, y, water, -1);
    return true;

    return false;
}

function gridCutter(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x1;
    local y1;
    local x2;
    local y2;

    if (coords[0].x <= coords[1].x)
    {
        x1 = coords[0].x;
        x2 = coords[1].x;
    }
    else
    {
        x1 = coords[1].x;
        x2 = coords[0].x;
    }

    if (coords[0].y <= coords[1].y)
    {
        y1 = coords[0].y;
        y2 = coords[1].y;
    }
    else
    {
        y1 = coords[1].y;
        y2 = coords[0].y;
    }

    local entity = engine.getScene().createEntity();
    grid.sliceInto(entity, x1, y1, x2, y2);

    return false;
}
