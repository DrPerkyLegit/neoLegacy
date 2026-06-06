using System;
using System.Collections.Generic;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory.NBT;

/// <summary>
/// An NBT tag that stores a int, see <see cref="ItemMeta"/> on how to use it
/// </summary>
public class IntTag : Tag
{
    public int data;

    public IntTag(string name, int value) : base(name, Tag.TAG_Int)
    {
        this.data = value;
    }

    /// <inheritdoc/>
    public override bool matches(Tag other)
    {
        if (other is IntTag otherTag)
        {
            if (data != otherTag.data)
                return false;

            return base.matches(other);
        }

        return false;
    }

    /// <inheritdoc/>
    public override IntTag clone()
    {
        return new IntTag(this.name, this.data);
    }
}
