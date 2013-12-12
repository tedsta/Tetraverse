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

function steelBlock(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    return grid.placeMid(x, y, 5);
}

function steelWall(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    return grid.placeBack(x, y, 5);
}

function crowbar(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    local placeable = grid.getPlaceableAt(x, y);

    if (grid.getTile(x, y).mMat != 0 || grid.getTile(x, y).mBack != 0 || placeable != null)
    {
        local empty = Tile();
        grid.setTile(x, y, empty, -1);
        if (placeable != null)
            grid.removePlaceable(placeable);
        return true;
	}

    return false;
}

function door(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 4) == true)
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/door.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Door", x, y, 1, 4));
        entity.addComponentSq(LightComponent(200));
        entity.addComponentSq(SignalComponent());
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(2);

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
        transform.setOrigin(Vector2f(24-8, 32-8));

        local intent = IntentComponent();
        intent.mapKeyToIntent("up", KeyI, BtnStateDown);
        intent.mapKeyToIntent("down", KeyK, BtnStateDown);
        intent.mapKeyToIntent("left", KeyJ, BtnStateDown);
        intent.mapKeyToIntent("right", KeyL, BtnStateDown);

        local entity = engine.getScene().createEntity();
        entity.addComponentSq(transform);
        entity.addComponentSq(intent);
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/engine.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Thruster", x, y, 1, 1));
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(5);

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

    return true;
}

function wirer(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));

    local first = grid.getPlaceableAt(coords[0].x, coords[0].y);
    local second = grid.getPlaceableAt(coords[1].x, coords[1].y);

    print("meh\n");
    if (first != null && second != null)
    {
        print("almost\n");
        local firstSignal = castSignalComponent(first.getComponent(SignalComponentType));
        local secondSignal = castSignalComponent(second.getComponent(SignalComponentType));

        if (firstSignal != null && secondSignal != null)
        {
            print("I'm an electrician!\n");
            firstSignal.addOutput(secondSignal);
        }

        return true;
    }

    return false;
}
function light(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 1) == true)
    {
        local transform = TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1));
        transform.setOrigin(Vector2f(0, 0));

        local entity = engine.getScene().createEntity();
        entity.addComponentSq(transform);
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/torch.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Light", x, y, 1,1));
        entity.addComponentSq(LightComponent(170));
        entity.addComponentSq(SignalComponent());
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(2);

        return true;
    }

    return false;
}

function swtch(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 1))
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/button_wall.png", 2, 2));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Swtch", x, y, 1, 1));
        entity.addComponentSq(LightComponent(15));
        entity.addComponentSq(SignalComponent());
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(2);

        return true;
    }

    return false;
}

function o2(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 1, 3))
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/o2.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Swtch", x, y, 1, 3));
        entity.addComponentSq(LightComponent(15));
        entity.addComponentSq(SignalComponent());
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(2);

        return true;
    }

    return false;
}

function reactor(gridEnt, coords)
{
    local grid = castGridComponent(gridEnt.getComponent(GridComponentType));
    local x = coords[0].x;
    local y = coords[0].y;

    if (grid.canPlace(x, y, 3, 2))
    {
        local entity = engine.getScene().createEntity();
        entity.addComponentSq(TransformComponent(Vector2f(0, 0), 0, Vector2f(1, 1)));
        entity.addComponentSq(SpriteComponent("Content/Textures/Placeables/reactor.png", 1, 1));
        entity.addComponentSq(PlaceableComponent(entity, gridEnt, "Swtch", x, y, 3, 2));
        entity.addComponentSq(LightComponent(15));
        entity.addComponentSq(SignalComponent());
        grid.addPlaceable(entity);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLit(false);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLayer(2);

        return true;
    }

    return false;
}
