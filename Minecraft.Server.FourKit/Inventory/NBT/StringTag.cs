using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a string, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class StringTag : Tag
{
    public string data;

    public StringTag(string name, string value) : base(name, Tag.TAG_String)
    {
        this.data = value;
    }

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is StringTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override StringTag clone()
    {
        return new StringTag(this.name, this.data);
    }
}
