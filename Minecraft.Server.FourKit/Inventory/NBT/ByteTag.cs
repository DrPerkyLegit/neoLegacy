using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a byte, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class ByteTag : Tag
{
    public byte data;

    public ByteTag(string name, byte value) : base(name, Tag.TAG_Byte)
    {
        this.data = value;
    }

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is ByteTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override ByteTag clone()
    {
        return new ByteTag(this.name, this.data);
    }
}
