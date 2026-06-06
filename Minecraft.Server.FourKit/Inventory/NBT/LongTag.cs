using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a long, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class LongTag : Tag
{
    public long data;

    public LongTag(string name, long value) : base(name, Tag.TAG_Long)
    {
        this.data = value;
    }

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is LongTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override LongTag clone()
    {
        return new LongTag(this.name, this.data);
    }
}
