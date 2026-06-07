namespace Minecraft.Server.FourKit.Event.Player;

using Minecraft.Server.FourKit.Entity;

/// <summary>
/// Represents an event that is called when a player right clicks an entity that doesnt exist in the server.
/// </summary>
public class PlayerInteractFakeEntityEvent : PlayerEvent
{
    /// <summary>The id entity that was interected with.</summary>
    protected int entityId;
    protected int actionType;

    internal PlayerInteractFakeEntityEvent(Player who, int entityId, int actionType) : base(who)
    {
        this.entityId = entityId;
        this.actionType = actionType;
    }



    /// <summary>
    /// Gets the entity id that was interacted with by the player.
    /// </summary>
    /// <returns>entity id that was interacted by player</returns>
    public int getEntity() => entityId;


    public bool wasRightClick() => actionType == 0;
    public bool wasLeftClick() => actionType == 1;

}
