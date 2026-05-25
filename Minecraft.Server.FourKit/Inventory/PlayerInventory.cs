namespace Minecraft.Server.FourKit.Inventory;

using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using Minecraft.Server.FourKit.Enchantments;
using Minecraft.Server.FourKit.Entity;
using Minecraft.Server.FourKit.Inventory.Meta;

/// <summary>
/// Represents a player's inventory, including armor slots and the held item.
/// </summary>
public class PlayerInventory : Inventory
{
    private const int INVENTORY_SIZE = 40;
    private const int ARMOR_START = 36;
    private const int QUICKBAR_SIZE = 9;

    private int _heldItemSlot;
    internal HumanEntity? _holder;

    private byte[] syncBuffer;
    private GCHandle syncBufferHandle;

    internal PlayerInventory()
        : base("Player", InventoryType.PLAYER, INVENTORY_SIZE)
    {
        this.syncBuffer = new byte[4*1024]; //about 4kb of memory
        this.syncBufferHandle = GCHandle.Alloc(this.syncBuffer, GCHandleType.Pinned);
    }

    protected internal override void EnsureSynced()
    {
        if (_holder == null || NativeBridge.GetPlayerInventory == null)
            return;

        int entityId = _holder.getEntityId();

        NativeBridge.GetPlayerInventory(entityId, this.syncBufferHandle.AddrOfPinnedObject());

        int offset = 0;
        _heldItemSlot = this.syncBuffer[offset]; offset++;

        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            _items[i]?.UnbindFromInventory();

            _items[i] = ItemStack.ReadFromBuffer(this.syncBuffer, ref offset);
        }
    }

    /// <inheritdoc/>
    public override void setItem(int index, ItemStack? item)
    {
        base.setItem(index, item);
        _slotModifiedByPlugin = true;
        if (_holder != null && NativeBridge.SetPlayerInventorySlot != null &&
            index >= 0 && index < INVENTORY_SIZE)
        {

            byte[] singleItemBuffer = new byte[512]; //about half a kb of memory
            GCHandle singleItemBufferHandle = GCHandle.Alloc(singleItemBuffer, GCHandleType.Pinned);

            try
            {
                int offset = 0;
                ItemStack.WriteToBuffer(item, singleItemBuffer, ref offset);

                NativeBridge.SetPlayerInventorySlot(_holder.getEntityId(), index, singleItemBufferHandle.AddrOfPinnedObject());
            } finally
            {
                singleItemBufferHandle.Free();
            }
        }
    }

    /// <summary>
    /// Returns all ItemStacks from the armor slots.
    /// </summary>
    /// <returns>An array of ItemStacks for the armor slots.</returns>
    public ItemStack?[] getArmorContents()
    {
        EnsureSynced();
        var armor = new ItemStack?[4];
        for (int i = 0; i < 4; i++)
            armor[i] = _items[ARMOR_START + i];
        return armor;
    }

    /// <summary>
    /// Gets the ItemStack in the helmet slot.
    /// </summary>
    /// <returns>The helmet ItemStack.</returns>
    public ItemStack? getHelmet() => getItem(ARMOR_START + 3);

    /// <summary>
    /// Gets the ItemStack in the chestplate slot.
    /// </summary>
    /// <returns>The chestplate ItemStack.</returns>
    public ItemStack? getChestplate() => getItem(ARMOR_START + 2);

    /// <summary>
    /// Gets the ItemStack in the leggings slot.
    /// </summary>
    /// <returns>The leggings ItemStack.</returns>
    public ItemStack? getLeggings() => getItem(ARMOR_START + 1);

    /// <summary>
    /// Gets the ItemStack in the boots slot.
    /// </summary>
    /// <returns>The boots ItemStack.</returns>
    public ItemStack? getBoots() => getItem(ARMOR_START);

    /// <summary>
    /// Sets all four armor slots at once.
    /// </summary>
    /// <param name="items">An array of ItemStacks for the armor slots.</param>
    public void setArmorContents(ItemStack?[] items)
    {
        int len = Math.Min(items.Length, 4);
        for (int i = 0; i < len; i++)
            setItem(ARMOR_START + i, items[i]);
    }

    /// <summary>
    /// Sets the helmet slot.
    /// </summary>
    /// <param name="helmet">The ItemStack to set.</param>
    public void setHelmet(ItemStack? helmet) => setItem(ARMOR_START + 3, helmet);

    /// <summary>
    /// Sets the chestplate slot.
    /// </summary>
    /// <param name="chestplate">The ItemStack to set.</param>
    public void setChestplate(ItemStack? chestplate) => setItem(ARMOR_START + 2, chestplate);

    /// <summary>
    /// Sets the leggings slot.
    /// </summary>
    /// <param name="leggings">The ItemStack to set.</param>
    public void setLeggings(ItemStack? leggings) => setItem(ARMOR_START + 1, leggings);

    /// <summary>
    /// Sets the boots slot.
    /// </summary>
    /// <param name="boots">The ItemStack to set.</param>
    public void setBoots(ItemStack? boots) => setItem(ARMOR_START, boots);

    /// <summary>
    /// Gets the item the player is currently holding.
    /// </summary>
    /// <returns>The held ItemStack.</returns>
    public ItemStack? getItemInHand() {
        EnsureSynced();
        return getItem(_heldItemSlot);
    }

    /// <summary>
    /// Sets the item in the player's hand.
    /// </summary>
    /// <param name="stack">The ItemStack to set.</param>
    public void setItemInHand(ItemStack? stack)
    {
        EnsureSynced(); //we need to sync the current held slot, hate doing this here during a set call
        setItem(_heldItemSlot, stack);
    }

    /// <summary>
    /// Gets the slot number of the currently held item.
    /// </summary>
    /// <returns>The held item slot index (0-8).</returns>
    public int getHeldItemSlot()
    {
        EnsureSynced();
        return _heldItemSlot;
    }

    /// <summary>
    /// Sets the slot number of the currently held item.
    /// </summary>
    /// <param name="slot">The slot index (0-8).</param>
    public void setHeldItemSlot(int slot)
    {
        if (slot < 0 || slot >= QUICKBAR_SIZE)
            throw new ArgumentException($"Slot must be between 0 and {QUICKBAR_SIZE - 1} inclusive.");
        _heldItemSlot = slot;
        if (_holder != null)
            NativeBridge.SetHeldItemSlot?.Invoke(_holder.getEntityId(), slot);
    }

    /// <summary>
    /// Clears all matching items from the inventory. Setting either value
    /// to -1 will skip its check, while setting both to -1 will clear all
    /// items in your inventory unconditionally.
    /// </summary>
    /// <param name="id">The material id to match, or -1 for any.</param>
    /// <param name="data">The data value to match, or -1 for any.</param>
    /// <returns>The number of stacks cleared.</returns>
    public int clear(int id, int data)
    {
        EnsureSynced();
        int count = 0;
        for (int i = 0; i < getSize(); i++)
        {
            var item = _items[i];
            if (item == null) continue;
            if (id != -1 && item.getTypeId() != id) continue;
            if (data != -1 && item.getDurability() != data) continue;
            setItem(i, null);
            count++;
        }
        return count;
    }

    /// <summary>
    /// Gets the holder of this inventory.
    /// </summary>
    /// <returns>The HumanEntity that owns this inventory.</returns>
    public HumanEntity? getHolder() => _holder;

    /*private static ItemMeta? ReadMetaFromNative(int entityId, int slot, byte[]? buffer, GCHandle? bufferHandle)
    {
        if (NativeBridge.GetItemMeta == null)
            return null;

        if (buffer == null)
        {
            buffer = new byte[4096];
            bufferHandle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        }

        int bytesWritten = NativeBridge.GetItemMeta(entityId, slot, bufferHandle.GetValueOrDefault().AddrOfPinnedObject(), buffer!.Length);

        if (bytesWritten <= 0)
            return null;

        int offset = 0;
        int nameLen = BitConverter.ToInt32(buffer, offset);
        offset += 4;

        string? displayName = null;
        if (nameLen > 0)
        {
            displayName = Encoding.UTF8.GetString(buffer, offset, nameLen);
            offset += nameLen;
        }

        int loreCount = 0;
        if (offset + 4 <= bytesWritten)
        {
            loreCount = BitConverter.ToInt32(buffer, offset);
            offset += 4;
        }

        List<string>? lore = null;
        if (loreCount > 0)
        {
            lore = new List<string>(loreCount);
            for (int i = 0; i < loreCount; i++)
            {
                if (offset + 4 > bytesWritten) break;
                int lineLen = BitConverter.ToInt32(buffer, offset);
                offset += 4;
                if (lineLen > 0 && offset + lineLen <= bytesWritten)
                {
                    lore.Add(Encoding.UTF8.GetString(buffer, offset, lineLen));
                    offset += lineLen;
                }
                else
                {
                    lore.Add(string.Empty);
                }
            }
        }

        int enchantCount = 0;
        if (offset + 4 <= bytesWritten)
        {
            enchantCount = BitConverter.ToInt32(buffer, offset);
            offset += 4;
        }

        Dictionary<EnchantmentType, int>? enchants = null;
        if (enchantCount > 0)
        {
            enchants = new Dictionary<EnchantmentType, int>(enchantCount);
            for (int i = 0; i < enchantCount; i++)
            {
                if (offset + (4 + 4) > bytesWritten)
                    break;

                int type = BitConverter.ToInt32(buffer, offset);
                offset += 4;

                int level = BitConverter.ToInt32(buffer, offset);
                offset += 4;

                enchants.Add((EnchantmentType)type, level);
            }
        }

        if (displayName == null && (lore == null || lore.Count == 0) && (enchants == null || enchants.Count == 0))
            return null;

        var meta = new ItemMeta();
        if (displayName != null)
            meta.setDisplayName(displayName);
        if (lore != null && lore.Count > 0)
            meta.setLore(lore);
        if (enchants != null && enchants.Count > 0)
            meta.setEnchants(enchants);
        return meta;
    }

    private static void WriteMetaToNative(int entityId, int slot, ItemMeta? meta)
    {
        if (NativeBridge.SetItemMeta == null)
            return;

        if (meta == null || meta.isEmpty())
        {
            NativeBridge.SetItemMeta(entityId, slot, IntPtr.Zero, 0);
            return;
        }

        using var ms = new System.IO.MemoryStream(512);
        using var bw = new System.IO.BinaryWriter(ms);

        if (meta.hasDisplayName())
        {
            byte[] nameBytes = Encoding.UTF8.GetBytes(meta.getDisplayName());
            bw.Write(nameBytes.Length);
            bw.Write(nameBytes);
        }
        else
        {
            bw.Write(0);
        }

        if (meta.hasLore())
        {
            var lore = meta.getLore();
            bw.Write(lore.Count);
            foreach (var line in lore)
            {
                byte[] lineBytes = Encoding.UTF8.GetBytes(line ?? string.Empty);
                bw.Write(lineBytes.Length);
                bw.Write(lineBytes);
            }
        }
        else
        {
            bw.Write(0);
        }

        if (meta.hasEnchants())
        {
            var enchants = meta.getEnchants();
            bw.Write(enchants.Count);
            foreach (var enchant in enchants)
            {
                bw.Write((int)enchant.Key);
                bw.Write(enchant.Value);
            }
        }
        else
        {
            bw.Write(0);
        }

        bw.Flush();
        byte[] data = ms.ToArray();
        var gh = GCHandle.Alloc(data, GCHandleType.Pinned);
        try
        {
            NativeBridge.SetItemMeta(entityId, slot, gh.AddrOfPinnedObject(), data.Length);
        }
        finally
        {
            gh.Free();
        }
    }*/
}
