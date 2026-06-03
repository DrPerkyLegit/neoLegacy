namespace Minecraft.Server.FourKit.Inventory;

using Minecraft.Server.FourKit.Inventory.Meta;

/// <summary>
/// Represents a stack of items.
/// </summary>
public class ItemStack
{
    private Material _type;
    private int _amount;
    private short _durability;
    private ItemMeta? _meta;
    internal Inventory? _ownerInventory;
    internal int _ownerSlot = -1;

    /// <summary>
    /// Creates a new ItemStack of the specified material with amount 1.
    /// </summary>
    public ItemStack(Material type) : this(type, 1) { }

    /// <summary>
    /// Creates a new ItemStack of the specified material and amount.
    /// </summary>
    public ItemStack(Material type, int amount) : this(type, amount, 0) { }

    /// <summary>
    /// Creates a new ItemStack of the specified material, amount and durability.
    /// </summary>
    public ItemStack(Material type, int amount, short durability)
    {
        _type = type;
        _amount = amount;
        _durability = durability;
    }

    /// <summary>
    /// Creates a new ItemStack from a raw type ID with amount 1.
    /// </summary>
    public ItemStack(int typeId) : this(typeId, 1) { }

    /// <summary>
    /// Creates a new ItemStack from a raw type ID and amount.
    /// </summary>
    public ItemStack(int typeId, int amount) : this(typeId, amount, 0) { }

    /// <summary>
    /// Creates a new ItemStack from a raw type ID, amount and durability.
    /// </summary>
    public ItemStack(int typeId, int amount, short durability)
    {
        _type = Enum.IsDefined(typeof(Material), typeId) ? (Material)typeId : Material.AIR;
        _amount = amount;
        _durability = durability;
    }

    /// <summary>
    /// Gets the type of this item.
    /// </summary>
    /// <returns>Type of the items in this stack.</returns>
    public Material getType() => _type;

    /// <summary>
    /// Sets the type of this item.
    /// </summary>
    /// <param name="type">New type to set the items in this stack to.</param>
    public void setType(Material type) { _type = type; SyncToOwner(); }

    /// <summary>
    /// Gets the type id for this item.
    /// </summary>
    /// <returns>Type Id of the items in this stack.</returns>
    public int getTypeId() => (int)_type;

    /// <summary>
    /// Sets the type id for this item.
    /// </summary>
    /// <param name="type">New type id to set the items in this stack to.</param>
    public void setTypeId(int type)
    {
        _type = Enum.IsDefined(typeof(Material), type) ? (Material)type : Material.AIR;
        SyncToOwner();
    }

    /// <summary>
    /// Gets the amount of items in this stack.
    /// </summary>
    /// <returns>Amount of items in this stack.</returns>
    public int getAmount() => _amount;

    /// <summary>
    /// Sets the amount of items in this stack.
    /// </summary>
    /// <param name="amount">New amount of items in this stack.</param>
    public void setAmount(int amount) { _amount = amount; SyncToOwner(); }

    /// <summary>
    /// Gets the durability of this item.
    /// </summary>
    /// <returns>Durability of this item.</returns>
    public short getDurability() => _durability;

    /// <summary>
    /// Sets the durability of this item.
    /// </summary>
    /// <param name="durability">Durability of this item.</param>
    public void setDurability(short durability) { _durability = durability; SyncToOwner(); }

    /// <summary>
    /// Get a copy of this ItemStack's ItemMeta.
    /// </summary>
    /// <returns>A copy of the current ItemStack's ItemMeta.</returns>
    public ItemMeta getItemMeta() => _meta?.clone() ?? new ItemMeta();

    /// <summary>
    /// Checks to see if any meta data has been defined.
    /// </summary>
    /// <returns>Returns true if some meta data has been set for this item.</returns>
    public bool hasItemMeta() => _meta != null && !_meta.isEmpty();

    /// <summary>
    /// Set the ItemMeta of this ItemStack.
    /// </summary>
    /// <param name="itemMeta">New ItemMeta, or null to indicate meta data be cleared.</param>
    /// <returns>True if successfully applied ItemMeta.</returns>
    public bool setItemMeta(ItemMeta? itemMeta)
    {
        _meta = itemMeta?.clone();
        SyncToOwner();
        return true;
    }

    internal ItemMeta? getItemMetaInternal() => _meta;

    internal void setItemMetaInternal(ItemMeta? meta) => _meta = meta;

    internal void BindToInventory(Inventory inventory, int slot)
    {
        _ownerInventory = inventory;
        _ownerSlot = slot;
    }

    internal void UnbindFromInventory()
    {
        _ownerInventory = null;
        _ownerSlot = -1;
    }

    private void SyncToOwner()
    {
        if (_ownerInventory != null && _ownerSlot >= 0)
            _ownerInventory.setItem(_ownerSlot, this);
    }

    internal static ItemStack? ReadFromBuffer(byte[] buffer, ref int offset)
    {
        byte isItemNull = buffer[offset]; offset += 1;

        if (isItemNull == 0)
        {
            short itemId = (short)((buffer[offset] << 8) | buffer[offset + 1]);
            offset += 2;
            byte itemAmount = buffer[offset]; offset += 1;
            short itemAux = (short)((buffer[offset] << 8) | buffer[offset + 1]);
            offset += 2;

            ItemStack item = new ItemStack((int)itemId, (int)itemAmount, itemAux);
            item.setItemMetaInternal(ItemMeta.ReadFromBuffer(buffer, ref offset));

            return item;
        }

        return null;
    }

    internal static void WriteToBuffer(ItemStack? item, byte[] buffer, ref int offset)
    {
        if (item == null)
        {
            buffer[offset] = 0;
            offset += 1;
        }
        else
        {
            buffer[offset] = (byte)(item._type == Material.AIR ? 1 : 0);
            offset += 1;

            if (item._type != Material.AIR)
            {
                buffer[offset] = (byte)(((int)item._type >> 0x8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)((int)item._type & 0xFF);
                offset += 1;

                buffer[offset] = (byte)item._amount;
                offset += 1;

                buffer[offset] = (byte)((item._durability >> 0x8) & 0xFF);
                offset += 1;
                buffer[offset] = (byte)(item._durability & 0xFF);
                offset += 1;

                ItemMeta.WriteToBuffer(item._meta, buffer, ref offset);
            }
        }
    }
}
