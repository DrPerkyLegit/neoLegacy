namespace Minecraft.Server.FourKit.Inventory.Meta;

using Minecraft.Server.FourKit.Enchantments;

/// <summary>
/// Represents the metadata of an <see cref="ItemStack"/>, including display name and lore.
/// </summary>
public class ItemMeta
{
    private string? _displayName;
    private Dictionary<EnchantmentType, int>? _enchants;
    private List<string>? _lore;

    /// <summary>
    /// Checks for existence of a display name.
    /// </summary>
    /// <returns>true if this has a display name.</returns>
    public bool hasDisplayName() => _displayName != null;

    /// <summary>
    /// Gets the display name that is set.
    /// Plugins should check that hasDisplayName() returns true before calling this method.
    /// </summary>
    /// <returns>The display name that is set.</returns>
    public string getDisplayName() => _displayName ?? string.Empty;

    /// <summary>
    /// Sets the display name.
    /// </summary>
    /// <param name="name">The name to set.</param>
    public void setDisplayName(string? name) => _displayName = name;

    /// <summary>
    /// Checks for existence of lore.
    /// </summary>
    /// <returns>true if this has lore.</returns>
    public bool hasLore() => _lore != null && _lore.Count > 0;

    /// <summary>
    /// Gets the lore that is set.
    /// Plugins should check if hasLore() returns true before calling this method.
    /// </summary>
    /// <returns>A list of lore that is set.</returns>
    public List<string> getLore() => _lore != null ? new List<string>(_lore) : new List<string>();

    /// <summary>
    /// Sets the lore for this item. Removes lore when given null.
    /// </summary>
    /// <param name="lore">The lore that will be set.</param>
    public void setLore(List<string>? lore)
    {
        _lore = lore != null ? new List<string>(lore) : null;
    }

    /// <summary>
    /// Adds the specified enchantment to this item meta.
    /// </summary>
    /// <param name="enchantment">Enchantment to add</param>
    /// <param name="level">Level for the enchantment</param>
    /// <param name="ignoreLevelRestriction">Indicates the enchantment should be applied, ignoring the level limit</param>
    /// <returns>true if the item meta changed as a result of this call, false otherwise</returns>
    public bool addEnchant(EnchantmentType enchantment, int level, bool ignoreLevelRestriction)
    {
        if (_enchants == null)
            _enchants = new Dictionary<EnchantmentType, int>();

        if (!ignoreLevelRestriction)
        {
            Enchantment enchant = Enchantment.getByType(enchantment);

            if (enchant.getMaxLevel() < level)
                return false;
        }

        try
        {
            _enchants.Add(enchantment, level);
            return true;
        }
        catch { }

        return false;
    }

    /// <summary>
    /// Removes the specified enchantment from this item meta.
    /// </summary>
    /// <param name="enchantment">Enchantment to remove</param>
    /// <returns>true if the item meta changed as a result of this call, false otherwise</returns>
    public bool removeEnchant(EnchantmentType enchantment)
    {
        if (!hasEnchant(enchantment))
            return false;

        return _enchants.Remove(enchantment);
    }

    /// <summary>
    /// Returns a copy of the enchantments in this ItemMeta. Returns an empty map if none.
    /// </summary>
    /// <returns>An immutable copy of the enchantments</returns>
    public Dictionary<EnchantmentType, int> getEnchants() => _enchants != null ? new Dictionary<EnchantmentType, int>(_enchants) { } : new Dictionary<EnchantmentType, int>();

    /// <summary>
    /// Checks for the level of the specified enchantment.
    /// </summary>
    /// <param name="enchantment">Enchantment to check</param>
    /// <returns>The level that the specified enchantment has, or 0 if none</returns>
    public int getEnchantLevel(EnchantmentType enchantment)
    {
        if (!hasEnchant(enchantment))
            return 0;

        return _enchants[enchantment]; //this cant be invalid, we check above
    }

    /// <summary>
    /// Checks if the specified enchantment conflicts with any enchantments in this ItemMeta.
    /// </summary>
    /// <param name="enchantment">Enchantment to test</param>
    /// <returns>true if the enchantment conflicts, false otherwise</returns>
    public bool hasConflictingEnchant(EnchantmentType enchantment)
    {
        Enchantment enchantmentClass = Enchantment.getByType(enchantment);
        if (enchantmentClass == null)
            return false; //this should never happen

        foreach (KeyValuePair<EnchantmentType, int> ench in _enchants)
        {
            Enchantment enchClass = Enchantment.getByType(ench.Key);

            if (enchClass == null)
                continue; //this should never happen

            if (enchClass.conflictsWith(enchantmentClass))
            {
                return true;
            }
        }

        return false;
    }

    /// <summary>
    /// Checks for existence of the specified enchantment.
    /// </summary>
    /// <param name="enchantment">Enchantment to check</param>
    /// <returns>true if this enchantment exists for this meta</returns>
    public bool hasEnchant(EnchantmentType enchantment) => hasEnchants() && _enchants.ContainsKey(enchantment);

    /// <summary>
    /// Checks for the existence of any enchantments.
    /// </summary>
    /// <returns>true if an enchantment exists on this meta</returns>
    public bool hasEnchants() => _enchants != null && _enchants.Count > 0;

    /// <summary>
    /// Sets the enchantments for this item meta.
    /// </summary>
    /// <param name="enchants">The enchantments to set.</param>
    public void setEnchants(Dictionary<EnchantmentType, int>? enchants)
    {
        _enchants = enchants != null ? new Dictionary<EnchantmentType, int>(enchants) : null;
    }


    public ItemMeta clone()
    {
        var copy = new ItemMeta();
        copy._displayName = _displayName;
        copy._enchants = _enchants != null ? new Dictionary<EnchantmentType, int>(_enchants) : null;
        copy._lore = _lore != null ? new List<string>(_lore) : null;
        return copy;
    }

    internal bool isEmpty()
    {
        return _displayName == null && (_lore == null || _lore.Count == 0) && (_enchants == null || _enchants.Count == 0);
    }

    internal static ItemMeta? ReadFromBuffer(byte[] buffer, ref int offset)
    {
        byte isMetadataEmpty = buffer[offset];
        offset++;

        if (isMetadataEmpty == 0)
        {
            ItemMeta meta = new ItemMeta();

            byte metadataFlags = buffer[offset];
            offset++;
            if ((metadataFlags & 0x1) != 0)
            {
                short displayNameLength = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                offset += 2;

                string displayName = "";
                for (int i = 0; i < displayNameLength; i++)
                {
                    ushort charCode = (ushort)((buffer[offset] << 8) | buffer[offset + 1]);
                    offset += 2;

                    displayName += char.ConvertFromUtf32(charCode);
                }
                meta.setDisplayName(displayName);
            }
            if ((metadataFlags & 0x2) != 0) // has lore
            {
                byte loreCount = buffer[offset];
                offset += 1;
                List<string> lore = new List<string>();
                for (int i = 0; i < loreCount; i++)
                {
                    string loreString = "";
                    short loreLineLength = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                    offset += 2;
                    for (int j = 0; j < loreLineLength; j++)
                    {
                        ushort charCode = (ushort)((buffer[offset] << 8) | buffer[offset + 1]);
                        offset += 2;

                        loreString += char.ConvertFromUtf32(charCode);
                    }
                    lore.Add(loreString);
                }
                meta.setLore(lore);
            }
            if ((metadataFlags & 0x4) != 0) // has enchantments
            {
                byte enchantmentCount = buffer[offset];
                offset += 1;
                Dictionary<EnchantmentType, int> enchants = new Dictionary<EnchantmentType, int>();
                for (int i = 0; i < enchantmentCount; i++)
                {
                    EnchantmentType enchantmentType = (EnchantmentType)(short)((buffer[offset] << 8) | buffer[offset + 1]);
                    offset += 2;
                    short enchantmentLevel = (short)((buffer[offset] << 8) | buffer[offset + 1]);
                    offset += 2;
                    enchants.Add(enchantmentType, enchantmentLevel);
                }
                meta.setEnchants(enchants);
            }

            return meta;
        }

        return null;
    }

    internal static void WriteToBuffer(ItemMeta? meta, byte[] buffer, ref int offset)
    {
        if (meta == null || meta.isEmpty())
        {
            buffer[offset] = 1;
            offset += 1;
        }
        else
        {
            buffer[offset] = 0;
            offset += 1;

            bool hasDisplayName = meta._displayName != null && meta._displayName.Length > 0;
            bool hasLore = meta._lore != null && meta._lore.Count > 0;
            bool hasEnchants = meta._enchants != null && meta._enchants.Count > 0;

            if (hasDisplayName || hasLore || hasEnchants)
            {
                int metadataFlagsOffset = offset;
                buffer[metadataFlagsOffset] = 0;
                offset += 1;

                if (hasDisplayName)
                {
                    buffer[metadataFlagsOffset] |= 0x1;

                    buffer[offset] = (byte)((meta._displayName.Length >> 0x8) & 0xFF);
                    offset += 1;
                    buffer[offset] = (byte)(meta._displayName.Length & 0xFF);
                    offset += 1;

                    for (int i = 0; i < meta._displayName.Length; i++)
                    {
                        int charCode = char.ConvertToUtf32(meta._displayName, i);
                        buffer[offset] = (byte)((charCode >> 0x8) & 0xFF);
                        offset += 1;
                        buffer[offset] = (byte)(charCode & 0xFF);
                        offset += 1;
                    }
                }

                if (hasLore)
                {
                    buffer[metadataFlagsOffset] |= 0x2;

                    byte loreCount = (byte)(meta._lore.Count & 0xFF);
                    buffer[offset] = loreCount;
                    offset += 1;

                    for (int i = 0; i < loreCount; i++)
                    {
                        string loreString = meta._lore[i];
                        short loreLineLength = (short)loreString.Length;
                        buffer[offset] = (byte)((loreLineLength >> 0x8) & 0xFF);
                        offset += 1;
                        buffer[offset] = (byte)(loreLineLength & 0xFF);
                        offset += 1;

                        for (int j = 0; j < loreLineLength; j++)
                        {
                            int charCode = char.ConvertToUtf32(loreString, j);
                            buffer[offset] = (byte)((charCode >> 0x8) & 0xFF);
                            offset += 1;
                            buffer[offset] = (byte)(charCode & 0xFF);
                            offset += 1;
                        }
                    }
                }

                if (hasEnchants)
                {
                    buffer[metadataFlagsOffset] |= 0x4;

                    byte enchantmentCount = (byte)(meta._enchants.Count & 0xFF);
                    buffer[offset] = enchantmentCount;
                    offset += 1;

                    for (int i = 0; i < enchantmentCount; i++)
                    {
                        KeyValuePair<EnchantmentType, int> enchantment = meta._enchants.ElementAt(i);
                        short enchantmentType = (short)enchantment.Key;
                        short enchantmentLevel = (short)enchantment.Value;

                        buffer[offset] = (byte)((enchantmentType >> 0x8) & 0xFF);
                        offset += 1;
                        buffer[offset] = (byte)(enchantmentType & 0xFF);
                        offset += 1;

                        buffer[offset] = (byte)((enchantmentLevel >> 0x8) & 0xFF);
                        offset += 1;
                        buffer[offset] = (byte)(enchantmentLevel & 0xFF);
                        offset += 1;
                    }
                }
            }
        }
    }
}
